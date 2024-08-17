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
class FileProcessingService
{
	Logger& logger_;

public:
	FileProcessingService(Logger& logger) : logger_(logger)
	{


	}

	void adjust_nginx_configuration_and_reloud(const std::string& filename, std::string port)
	{
		/*std::string file_path = "/etc/nginx/nginx.conf";

		std::string new_text =
			"server {\n"
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
			"    }\n\n"
			"}\n";

		std::ofstream file(file_path, std::ios::app);

		if (!file.is_open()) {
			logger_.log(INFO, "Error: Could not open file " + file_path + strerror(errno) + '\n');
			return;
		}

		file << new_text << '\n';

		file.close();*/


		std::string file_path = "/etc/nginx/nginx.conf";

		// Open the nginx.conf file in read mode first
		std::ifstream file_in(file_path);
		if (!file_in.is_open()) {
			logger_.log(INFO, "Error: Could not open file " + file_path + strerror(errno) + '\n');
			return;
		}

		std::string content;
		std::string line;
		std::string temp_content;

		// Read the file content and store it in a temporary string
		while (std::getline(file_in, line)) {
			temp_content += line + "\n";
		}
		file_in.close();

		// Remove the last occurrence of "}" in the content
		size_t last_brace_pos = temp_content.rfind("}");
		if (last_brace_pos != std::string::npos) {
			temp_content.erase(last_brace_pos);
		}

		// Now create the new server block
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

		// Append the new server block to the existing content
		temp_content += new_text;

		// Add the final closing brace
		temp_content += "}\n";

		// Write the updated content back to the file
		std::ofstream file_out(file_path);
		if (!file_out.is_open()) {
			logger_.log(INFO, "Error: Could not open file " + file_path + strerror(errno) + '\n');
			return;
		}
		file_out << temp_content;
		file_out.close();









		//TODO FIX BUG WITH }}}}}

		std::string command = "sudo systemctl reload nginx";

		std::thread commandThread(&CommandService::execute_command, command);

		commandThread.join();

		logger_.log(INFO, "Nginx reloaded successfully.");
	}

	void delete_file(const std::string& file_path) const
	{
		try {
			// Delete the original file
			if (std::filesystem::exists(file_path)) {
				std::filesystem::remove(file_path);
				logger_.log(INFO, "Deleted file: " + file_path);
			}

			// Delete the directory recursively//Todo test if method will not work
			/*if (std::filesystem::exists(final_files_directory)) {
				std::filesystem::remove_all(final_files_directory);
				logger_.log(INFO, "Deleted directory: " + final_files_directory);
			}*/
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

	void create_service_file(std::string path, std::string name)
	{
		logger_.log(INFO, "Start create_service_file");

		std::string dll_file_name = find_file_by_suffix(path, "exe");
		size_t pos = dll_file_name.find(".exe");
		if (pos != std::string::npos) {
			dll_file_name.replace(pos, 4, ".dll");
		}

		std::string filename = "/etc/systemd/system/" + name + ".service";
		std::ofstream serviceFile(filename);

		if (serviceFile.is_open()) {
			serviceFile << "[Unit]\n";
			serviceFile << "Description=" << name << " Service\n";
			serviceFile << "After=network.target\n\n";

			serviceFile << "[Service]\n";
			serviceFile << "ExecStart=/usr/bin/dotnet /home/danilt2000/SpCloud/" + name + "/" + dll_file_name + "\n";
			//serviceFile << "ExecStart=/home/danilt2000/SpCloud/SpCloudMain/build/SpCloudMain\n";
			serviceFile << "WorkingDirectory=/home/danilt2000/SpCloud/" + name + "\n";
			//serviceFile << "WorkingDirectory=/home/danilt2000/SpCloud/SpCloudMain/build\n";
			serviceFile << "Restart=always\n";
			serviceFile << "User=danilt2000\n";
			serviceFile << "Environment=PATH=/usr/bin\n";
			serviceFile << "Environment=NODE_ENV=production\n\n";

			serviceFile << "[Install]\n";
			serviceFile << "WantedBy=multi-user.target\n";

			std::string command_reload = "sudo systemctl daemon-reload";

			std::thread commandThreadReload(&CommandService::execute_command, command_reload);

			commandThreadReload.join();

			std::string command_start = "sudo systemctl start " + name + ".service";

			std::thread commandThreadStart(&CommandService::execute_command, commandThreadStart);

			commandThreadStart.join();

			//Todo check service ->sudo systemctl status <service-name>.service

			serviceFile.close();

			logger_.log(INFO, "Service file " + filename + " created successfully.\n");
		}
		else {
			logger_.log(INFO, "Unable to open file " + filename + " for writing: " + strerror(errno) + "\n");
		}
	}

	void create_directory(const std::string& path) {
		std::filesystem::create_directories(path);
	}


	void unzip(const std::string& file_path, const std::string& final_files_directory) {
		create_directory(final_files_directory);

		std::string command = "unrar x " + file_path + " " + final_files_directory;

		logger_.log(INFO, "Start unzip command" + command);

		std::thread commandThread(&CommandService::execute_command, command);

		commandThread.join();//Todo check if unzipping happening before of deleting file 
	}

	std::string find_file_by_suffix(const std::string& directory, const std::string& suffix) {
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
			if (entry.is_regular_file() && entry.path().filename().string().ends_with(suffix)) {
				return entry.path().filename().string();
			}
		}
		return ""; //todo add throwing exception 
	}
};
