#include "../httplib.h"
//#include <windows.h>
//#include "Service/AuthorizationService.cpp"
//#include "Service/FileProcessingService.cpp"

#include "../Service/AuthorizationService.cpp"
#include "../Service/FileProcessingService.cpp"
//#include "Service/Logger.cpp"

class PublishController
{

private:
	AuthorizationService authorization;

	std::shared_ptr<FileProcessingService> file_processing;//Smart pointer

	//std::string publish_app_path = "/mnt/c/Users/Danil/SpCloudApp";
	std::string publish_app_path = "/home/danilt2000/SpCloud/";

	Logger& logger_;

	//std::string publish_app_path = "C:/Temps/";// Todo delete if not needed 

public:
	PublishController(httplib::Server& svr, AuthorizationService authorization, std::shared_ptr<FileProcessingService> file_processing, Logger& logger)
		: authorization(authorization), file_processing(file_processing), logger_(logger)
	{
	}

public:
	void process_publish(const httplib::Request& req, httplib::Response& res)
	{
		if (this->authorization.is_user_authorized())
		{
			const auto& content = req.files.begin()->second.content;

			const auto& filename = this->publish_app_path + req.files.begin()->second.filename;

			if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".rar") {
				if (file_processing->save_file(filename, content)) {

					std::string random_string = generate_random_string(20);//TODO VERY IMPORTANT CHANGE THIS RANDOM GENERATING TO GENERATE UNIQUE STRING

					file_processing->unzip(filename, this->publish_app_path + random_string);

					this->dotnet_publish(this->publish_app_path + random_string);

					res.set_content("File uploaded successfully: " + filename, "text/plain");
				}
				else {
					res.status = 500;
					res.set_content("Failed to save file, please ensure you are putting rar file"
						+ filename, "text/plain");
				}
			}
			else {
				res.status = 400;
				res.set_content("Invalid file type. Only .rar files are allowed.",
					"text/plain");
			}
		}
		else
		{
			//Todo add logging and exiting from function with bead request 
		}
	}

private:
	void dotnet_publish(const std::string& path)
	{
		std::string dll_file_name = file_processing->find_file_by_suffix(path, "dll");

		std::string command = R"(dotnet )" + path + "/" + dll_file_name;

		std::thread commandThread(&CommandService::execute_command, command);

		commandThread.detach();
	}

	static std::string generate_random_string(size_t length, const std::string& char_set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") {
		std::random_device rd;
		std::mt19937 generator(rd());
		std::uniform_int_distribution<> distribution(0, char_set.size() - 1);

		std::string random_string;
		for (size_t i = 0; i < length; ++i) {
			char random_char = char_set[distribution(generator)];
			random_string += random_char;
		}

		return random_string;
	}
};