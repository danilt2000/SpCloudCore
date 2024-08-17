#include "../httplib.h"
//#include <windows.h>
//#include "Service/AuthorizationService.cpp"
//#include "Service/FileProcessingService.cpp"

#include "../Service/AuthorizationService.cpp"
#include "../Service/FileProcessingService.cpp"
#include "../Models/App.cpp"
//#include "Service/Logger.cpp"

class PublishController
{

private:
	AuthorizationService authorization;

	std::shared_ptr<FileProcessingService> file_processing;//Smart pointer

	//std::string publish_app_path = "/mnt/c/Users/Danil/SpCloudApp";
	std::string publish_app_path = "/home/danilt2000/SpCloud/";

	Logger& logger_;

	int last_available_port = 8081;

public:
	PublishController(httplib::Server& svr, AuthorizationService authorization, std::shared_ptr<FileProcessingService> file_processing, Logger& logger)
		: authorization(authorization), file_processing(file_processing), logger_(logger)
	{
	}

public:
	std::string  process_publish(const httplib::Request& req, App* app)
	{
		const auto& content = req.files.begin()->second.content;

		const auto& filename = this->publish_app_path + req.files.begin()->second.filename;

		if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".rar") {
			//if (true) {//TODO UNCOMMIT WHEN STARING TO WRITE PUBLISH PROCESS
			if (file_processing->save_file(filename, content)) {

				std::string app_final_file_path = app->get_name() + app->get_user_id();//TODO VERY IMPORTANT CHANGE THIS RANDOM GENERATING TO GENERATE UNIQUE STRING

				logger_.log(INFO, "app_final_file_path: " + app_final_file_path);

				file_processing->unzip(filename, this->publish_app_path + app_final_file_path);//TODO UNCOMMIT WHEN STARING TO WRITE PUBLISH PROCESS

				check_port_and_increase_if_not_available();

				file_processing->adjust_nginx_configuration_and_reloud(app->get_name(), std::to_string(last_available_port));

				file_processing->create_service_file(this->publish_app_path, app_final_file_path);//TODO UNCOMMIT WHEN STARING TO WRITE PUBLISH PROCESS
				//file_processing->create_service_file(app->get_name());//TODO UNCOMMIT WHEN STARING TO WRITE PUBLISH PROCESS

				if (app->get_target() == "dotnet network")
				{
					this->dotnet_publish(this->publish_app_path + app_final_file_path, last_available_port);//TODO UNCOMMIT WHEN STARING TO WRITE PUBLISH PROCESS
				}

				if (app->get_target() == "dotnet")
				{
					this->dotnet_publish(this->publish_app_path + app_final_file_path);//TODO UNCOMMIT WHEN STARING TO WRITE PUBLISH PROCESS
				}

				//Todo introduce increase counter of available app for user in mongo db

				file_processing->delete_file(filename);

				/*app->set_url("https://" + app->get_name() + ".almavid.ru/");

				app->set_url_on_local_machine("http://localhost:" + std::to_string(last_available_port));*/

				//app->set_service_name(app_final_file_path);

				return "Publish successfully: " + filename;
			}
			else {
				return "Failed to save file, please ensure you are putting rar file" + filename;
			}
		}
		else {
			return "Invalid file type. Only .rar files are allowed." + filename;

		}
	}

private:
	void dotnet_publish(const std::string& path, int port)
	{//Todo adjust to build setting from mongodb
		std::string dll_file_name = file_processing->find_file_by_suffix(path, "exe");
		size_t pos = dll_file_name.find(".exe");
		if (pos != std::string::npos) {
			dll_file_name.replace(pos, 4, ".dll");
		}

		std::string command = R"(dotnet )" + path + "/" + dll_file_name + " --urls http://localhost:" + std::to_string(port);

		logger_.log(INFO, "dotnet_publish command : " + command);

		std::thread commandThread(&CommandService::execute_command, command);

		commandThread.detach();
	}

	void dotnet_publish(const std::string& path)
	{//Todo adjust to build setting from mongodb
		std::string dll_file_name = file_processing->find_file_by_suffix(path, "exe");
		size_t pos = dll_file_name.find(".exe");
		if (pos != std::string::npos) {
			dll_file_name.replace(pos, 4, ".dll");
		}

		std::string command = R"(dotnet )" + path + "/" + dll_file_name;

		std::thread commandThread(&CommandService::execute_command, command);

		commandThread.detach();
	}

	void check_port_and_increase_if_not_available()
	{
		while (true)
		{
			std::string port_str = std::to_string(last_available_port);

			std::string command = "ss -tuln | grep :" + port_str;

			auto request = std::async(std::launch::async, &PublishController::execute_command, this, command);

			std::string response = request.get();

			if (!response.empty())
			{
				last_available_port++;
			}
			else
			{
				break;
			}
		}

	}

	std::string execute_command(const std::string& command) {
		std::array<char, 128> buffer;
		std::string result;
		std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
		if (!pipe) throw std::runtime_error("popen() failed!");
		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}
		return result;
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