// ReSharper disable CppClangTidyBugproneSuspiciousInclude
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>

#include "CommandService.cpp"

class FileProcessingService
{
public:
	FileProcessingService()
	{

	}

	bool save_file(const std::string& filename, const std::string& content) {
		std::ofstream ofs(filename, std::ios::binary);
		if (!ofs) return false;
		ofs << content;
		return ofs.good();
	}

	void create_directory(const std::string& path) {
		std::filesystem::create_directories(path);
	}

	void unzip(const std::string& file_path, const std::string& final_files_directory) {
		create_directory(final_files_directory);

		//Windows version
		//std::string command = R"(powershell -Command "& \"C:\Program Files\WinRAR\WinRAR.exe\" x \")" + file_path + R"(\" \")" + final_files_directory + R"(\")";

		//Linux version
		//std::string command = "unzip " + file_path + " -d " + final_files_directory;
		std::string command = "unrar x" + file_path + " " + final_files_directory;

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

	//static void execute_command(const std::string& command) {//todo delete if not needed 
	//	int result = std::system(command.c_str());  // NOLINT(concurrency-mt-unsafe)
	//	if (result != 0) {
	//		std::cerr << "Command failed with code: " << result << std::endl;
	//	}
	//}

	//void unzip(const std::string& file_path, const std::string& final_files_directory)
	//{
	//	std::string createDirCommand = "mkdir \"" + final_files_directory + "\"";

	//	system(createDirCommand.c_str());//Todo solve unsafe warning

	//	const std::string command = "WinRAR x \"" + file_path + "\" \"" + final_files_directory + "\"";

	//	int result = system(command.c_str());//Todo solve unsafe warning
	//}
};
