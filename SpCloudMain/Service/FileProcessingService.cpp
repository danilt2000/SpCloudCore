// ReSharper disable CppClangTidyBugproneSuspiciousInclude
#include <filesystem>
#include <fstream>
#include <future>
#include <string>
#include <thread>
#include <sys/stat.h>
#include "CommandService.cpp"
#include "Logger.cpp"
//#include <Poco/Mutex.h>
//#include <Poco/Path.h>
//#include <Poco/FileStr*/eam.h >
class FileProcessingService
{

public:
	FileProcessingService(Logger& logger) : logger_(logger)
	{


	}

	bool save_file(const std::string& filename, const std::string& content) {

		//std::lock_guard<std::mutex> lock(file_mutex); // Блокируем мьютекс//Todo TEST STABILITY OF THIS

		std::ofstream ofs(filename, std::ios::binary);

		if (!ofs) return false;

		logger_.log(INFO, "Start saving file");

		ofs << content;

		return ofs.good();
	}

	//bool save_file(const std::string& filename, const std::string& content) {
	//	Mutex::ScopedLock lock(file_mutex); // Блокируем мьютекс

	//	try {
	//		Poco::File file(filename);
	//		Poco::FileOutputStream ofs(filename, std::ios::binary | std::ios::trunc);

	//		if (!ofs.good()) {
	//			std::cerr << "Error opening file: " << filename << std::endl;
	//			return false;
	//		}

	//		ofs.write(content.c_str(), content.size());

	//		if (!ofs.good()) {
	//			std::cerr << "Error writing to file: " << filename << std::endl;
	//			return false;
	//		}

	//		ofs.close(); // Явно закрываем файл
	//	}
	//	catch (const Poco::Exception& ex) {
	//		std::cerr << "Poco exception: " << ex.displayText() << std::endl;
	//		return false;
	//	}

	//	return true;
	//}


	//bool save_file_with_timeout(const std::string& filename, const std::string& content, std::chrono::milliseconds timeout) {
	//	// Запускаем асинхронную задачу для записи файла
	//	auto future = std::async(std::launch::async, &FileProcessingService::save_file, this, filename, content);

	//	// Ожидаем завершения задачи или истечения тайм-аута
	//	if (future.wait_for(timeout) == std::future_status::ready) {
	//		return future.get(); // Возвращаем результат записи
	//	}
	//	else {
	//		std::cerr << "Timeout occurred while saving file: " << filename << std::endl;
	//		return false; // Тайм-аут
	//	}
	//}


	//bool save_file_with_retry(const std::string& filename, const std::string& content, int max_retries = 5, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
	//	for (int i = 0; i < max_retries; ++i) {
	//		if (save_file_with_timeout(filename, content, timeout)) {
	//			return true;
	//		}
	//		std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Пауза перед повторной попыткой
	//	}
	//	std::cerr << "Failed to save file after " << max_retries << " attempts: " << filename << std::endl;
	//	return false;
	//}

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
/*private:
	std::mutex file_mutex; */// Мьютекс для синхронизации доступа к файлу

	Logger& logger_;
};
