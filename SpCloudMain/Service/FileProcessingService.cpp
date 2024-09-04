// ReSharper disable CppClangTidyBugproneSuspiciousInclude
#include <filesystem>
#include <fstream>
#include <future>
#include <string>
#include <thread>
#include <sys/stat.h>
#include "CommandService.cpp"
#include "Logger.cpp"
#include <string.h>
#include <array>
#include <iostream>
#include "../httplib.h"
#include <string>
#include <future>
class FileProcessingService
{
	Logger& logger_;

	std::mutex nginx_config_mutex;

public:
	FileProcessingService(Logger& logger) : logger_(logger)
	{


	}

	void adjust_nginx_configuration_and_reload(const std::string& filename, std::string port)
	{
		std::lock_guard<std::mutex> lock(nginx_config_mutex);

		std::string file_path = "/etc/nginx/nginx.conf";

		std::ifstream file_in(file_path);
		if (!file_in.is_open()) {
			logger_.log(INFO, "Error: Could not open file " + file_path + strerror(errno) + '\n');
			return;
		}

		std::string content;
		std::string line;
		std::string temp_content;

		while (std::getline(file_in, line)) {
			temp_content += line + "\n";
		}
		file_in.close();

		size_t last_brace_pos = temp_content.rfind("}");
		if (last_brace_pos != std::string::npos) {
			temp_content.erase(last_brace_pos);
		}

		std::string new_text =
			"\nserver {\n"
			"    listen 443 ssl;\n"
			"    server_name " + filename + ".almavid.ru;\n\n"
			"    ssl_certificate /etc/letsencrypt/live/almavid.ru/fullchain.pem;\n"
			"    ssl_certificate_key /etc/letsencrypt/live/almavid.ru/privkey.pem;\n"
			"    ssl_protocols       TLSv1 TLSv1.1 TLSv1.2;\n"
			"    ssl_ciphers         HIGH:!aNULL:!MD5;\n\n"
			"    client_max_body_size 2G;  # Allow file uploads up to 2GB\n\n"
			"    location / {\n"
			"        proxy_pass http://localhost:" + port + ";\n"
			"        proxy_set_header Host $host;\n"
			"        proxy_set_header X-Real-IP $remote_addr;\n"
			"        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;\n"
			"        proxy_set_header X-Forwarded-Proto $scheme;\n\n"
			"        # Support for WebSocket\n"
			"        proxy_http_version 1.1;\n"
			"        proxy_set_header Upgrade $http_upgrade;\n"
			"        proxy_set_header Connection \"upgrade\";\n"
			"    }\n"
			"}\n";

		temp_content += new_text;

		temp_content += "}\n";

		std::ofstream file_out(file_path);
		if (!file_out.is_open()) {
			logger_.log(INFO, "Error: Could not open file " + file_path + strerror(errno) + '\n');
			return;
		}
		file_out << temp_content;
		file_out.close();

		std::string command = "cd ~/SpCloud/Infrastructure/ && docker compose -f nginx.yml up --build -d ";

		std::thread commandThread(&CommandService::execute_command, command);

		commandThread.join();

		logger_.log(INFO, "Nginx reloaded successfully.");
	}

	void remove_nginx_configuration_block_and_reload(const std::string& server_name) {
		std::lock_guard<std::mutex> lock(nginx_config_mutex);

		std::string file_path = "/etc/nginx/nginx.conf";

		std::ifstream inFile(file_path);
		std::ostringstream oss;
		std::string line;
		bool insideTargetServerBlock = false;
		bool shouldRemoveBlock = false;

		if (!inFile) {
			std::cerr << "Error opening file: " << file_path << '\n';
			return;
		}

		while (std::getline(inFile, line)) {
			std::string trimmedLine = line;
			trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t")); // Убираем ведущие пробелы и табуляции

			// Определяем начало блока server
			if (trimmedLine.find("server {") != std::string::npos) {
				insideTargetServerBlock = true;
				shouldRemoveBlock = false; // Сбрасываем флаг удаления блока
			}

			// Ищем server_name внутри блока server
			if (insideTargetServerBlock && trimmedLine.find("server_name") != std::string::npos) {
				if (trimmedLine.find(server_name) != std::string::npos) {
					shouldRemoveBlock = true;
				}
			}

			// Если текущая строка завершает блок server
			if (insideTargetServerBlock && trimmedLine == "}") {
				insideTargetServerBlock = false;
				if (!shouldRemoveBlock) {
					oss << line << "\n"; // Если блок не должен быть удалён, записываем его
				}
				continue;
			}

			// Если мы не внутри блока, или блок не подлежит удалению, записываем строку
			if (!insideTargetServerBlock || !shouldRemoveBlock) {
				oss << line << "\n";
			}
		}

		inFile.close();

		// Записываем результат обратно в файл
		std::ofstream outFile(file_path);
		if (!outFile) {
			std::cerr << "Error opening file for writing: " << file_path << '\n';
			return;
		}

		outFile << oss.str();

		outFile.close();

		std::string command = "cd ~/SpCloud/Infrastructure/ && docker compose -f nginx.yml up --build -d ";

		std::thread commandThread(&CommandService::execute_command, command);

		commandThread.join();

		logger_.log(INFO, "Nginx reloaded successfully.");
	}



	void delete_file(const std::string& file_path) const {
		try {
			if (std::filesystem::exists(file_path)) {
				if (std::filesystem::is_directory(file_path)) {
					std::filesystem::remove_all(file_path);
					logger_.log(INFO, "Deleted directory and all contents: " + file_path);
				}
				else {
					std::filesystem::remove(file_path);
					logger_.log(INFO, "Deleted file: " + file_path);
				}
			}
			else {
				logger_.log(ERROR, "File or directory does not exist: " + file_path);
			}
		}
		catch (const std::filesystem::filesystem_error& e) {
			logger_.log(ERROR, "Error during deletion: " + std::string(e.what()));
		}
	}

	bool save_file(const std::string& filename, const std::string& content) {

		logger_.log(INFO, "Start saving file method");

		std::ofstream ofs(filename, std::ios::binary);

		logger_.log(INFO, "Create file stream");

		if (!ofs) return false;

		logger_.log(INFO, "Start save content");

		ofs << content;

		return ofs.good();
	}

	void stop_and_start_service_file(std::string name)
	{
		std::string command_reload = "docker stop " + name;

		std::string response_reload = execute_and_log_command(command_reload);

	}

	//Outdated
	void stop_service_file(std::string name)
	{
		std::string command_stop = "docker restart " + name;

		std::string response_reload = execute_and_log_command(command_stop);
	}

	void create_service_file_dotnet(std::string path, std::string name, std::string port, bool is_asp)
	{
		logger_.log(INFO, "Outdated. Please didnt use create_service_file_dotnet");

		
	}
	std::string execute_and_log_command(const std::string& command) {
		std::string result = execute_command(command);
		logger_.log(INFO, "Executed command: " + command + "\nResult: " + result);
		return result;
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

	void create_directory(const std::string& path) {
		std::filesystem::create_directories(path);
	}


	void unzip(const std::string& file_path, const std::string& final_files_directory) {
		create_directory(final_files_directory);

		std::string command = "";

		if (std::filesystem::path(file_path).extension() == ".rar") {
			command = "unrar x " + file_path + " " + final_files_directory;
		}else {
			command = "tar -xzf " + file_path + " -C " + final_files_directory;
		}
		logger_.log(INFO, "Start unzip command" + command);

		std::thread commandThread(&CommandService::execute_command, command);

		commandThread.join();//Todo check if unzipping happening before of deleting file 
	}

	std::string find_file_by_suffix(const std::string& directory, const std::string& suffix) {
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
			if (entry.is_regular_file() && std::filesystem::path(entry.path().filename()).extension() == suffix) {
				return entry.path().filename().string();
			}
		}
		return ""; //todo add throwing exception 
	}
};
