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
	std::string process_publish(const httplib::Request& req, App* app)
	{
		const auto& content = req.files.begin()->second.content;

		const auto& filename = this->publish_app_path + req.files.begin()->second.filename;

		if (filename.size() >= 4) {
			if (file_processing->save_file(filename, content)) {

				std::string app_final_file_path = app->get_name() + "_" + app->get_user_id();

				logger_.log(INFO, "app_final_file_path: " + app_final_file_path);

				file_processing->unzip(filename, this->publish_app_path + app_final_file_path);

				if (app->get_target() == "dotnet network")
				{
					check_port_and_increase_if_not_available();

					file_processing->adjust_nginx_configuration_and_reload(app->get_name(), std::to_string(last_available_port));//

					this->dotnet_publish(this->publish_app_path + app_final_file_path, app_final_file_path, last_available_port);//Test

					app->set_url("https://" + app->get_name() + ".almavid.ru/");

					app->set_url_on_local_machine("http://localhost:" + std::to_string(last_available_port));
				}

				if (app->get_target() == "dotnet")
				{
					this->dotnet_publish(this->publish_app_path + app_final_file_path, app_final_file_path);

					app->set_url("Worker Service");

					app->set_url_on_local_machine("Worker Service");
				}

				file_processing->delete_file(filename);

				app->set_service_name(app_final_file_path);

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

	std::string process_update(const httplib::Request& req, App* app)
	{
		const auto& content = req.files.begin()->second.content;

		const auto& filename = this->publish_app_path + req.files.begin()->second.filename;
		
		
		if (file_processing->save_file(filename, content)) {
			
			std::string app_final_file_path = app->get_name() + app->get_user_id();

			logger_.log(INFO, "app_final_file_path: " + app_final_file_path);

			file_processing->delete_file(this->publish_app_path + app_final_file_path);

			file_processing->unzip(filename, this->publish_app_path + app_final_file_path);

			file_processing->stop_service_file(app_final_file_path);
			
			if (app->get_target() == "dotnet network") {
				check_port_and_increase_if_not_available();
				this->dotnet_publish(this->publish_app_path + app_final_file_path, app_final_file_path, last_available_port);
			}else {
				this->dotnet_publish(this->publish_app_path + app_final_file_path, app_final_file_path);
			}
			
			file_processing->delete_file(filename);
		}
		else {
			return "Invalid file type. Only .rar files are allowed." + filename;
		}
		

		return "Success";
	}

	std::string process_delete(const httplib::Request& req, App* app)
	{
		std::string app_final_file_path = app->get_name() + app->get_user_id();

		logger_.log(INFO, "app_final_file_path: " + app_final_file_path);

		file_processing->stop_service_file(app_final_file_path);

		file_processing->delete_file(this->publish_app_path + app_final_file_path);

		file_processing->delete_file("/etc/systemd/system/" + app_final_file_path + ".service");

		file_processing->remove_nginx_configuration_block_and_reload(app->get_name());

		return "Success";
	}

private:
	void dotnet_publish(const std::string& path, const std::string& folderName, int port)
	{
		std::thread commandThreadBuild(&CommandService::execute_command, "docker build -t " + folderName + ".");
		std::thread commandThreadRun(&CommandService::execute_command, "docker run -d -e ASPNETCORE_URLS=http://0.0.0.0:" + std::to_string(port) + "-p" + std::to_string(port)+":"+std::to_string(port) + "--name " + folderName + " " + folderName);

		logger_.log(INFO, "docker running container : " + folderName + " with port : " + std::to_string(port));

		commandThreadBuild.detach();
		commandThreadRun.detach();
	}

	void dotnet_publish(const std::string& path, const std::string& folderName)
	{
		std::thread commandThreadBuild(&CommandService::execute_command, "docker build -t " + folderName + ".");
		std::thread commandThreadRun(&CommandService::execute_command, "docker run -d --name " + folderName + " " + folderName);

		logger_.log(INFO, "docker running container : " + folderName);

		commandThreadBuild.detach();
		commandThreadRun.detach();
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