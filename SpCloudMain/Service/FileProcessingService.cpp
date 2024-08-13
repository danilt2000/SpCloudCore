// ReSharper disable CppClangTidyBugproneSuspiciousInclude
#include <filesystem>
#include <fstream>
#include <future>
#include <string>
#include <thread>
#include <sys/stat.h>
#include "CommandService.cpp"
#include "Logger.cpp"
class FileProcessingService
{
	Logger& logger_;

public:
	FileProcessingService(Logger& logger) : logger_(logger)
	{


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

	void create_directory(const std::string& path) {
		std::filesystem::create_directories(path);
	}


	void unzip(const std::string& file_path, const std::string& final_files_directory) {
		create_directory(final_files_directory);

		std::string command = "unrar x " + file_path + " " + final_files_directory;

		logger_.log(INFO, "Start unzip command" + command);

		std::thread commandThread(&CommandService::execute_command, command);

		commandThread.join();
	}

	std::string find_file_by_suffix(const std::string& directory, const std::string& suffix) {
		for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
			if (entry.is_regular_file() && entry.path().filename().string().ends_with(suffix)) {
				return entry.path().filename().string();
			}
		}
		return ""; //todo add throwing exception 
	}

/*private:
	std::mutex file_mutex; */// Мьютекс для синхронизации доступа к файлу

};
