#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

class Logger {
public:
	// Constructor: Opens the log file in append mode 
	Logger( std::string& filename)
	{
		logFile.open(filename, std::ios::app);
		if (!logFile.is_open()) {
			std::cerr << "Error opening log file." << '\n';
		}
	}

	// Destructor: Closes the log file 
	~Logger() { logFile.close(); }

	// Logs a message with a given log level 
	void log(LogLevel level, const std::string& message)
	{
		// Get current timestamp 
		time_t now = time(0);
		tm* timeinfo = localtime(&now);
		char timestamp[20];
		strftime(timestamp, sizeof(timestamp),
			"%Y-%m-%d %H:%M:%S", timeinfo);

		// Create log entry 

		std::ostringstream logEntry;
		logEntry << "[" << timestamp << "] "
			<< levelToString(level) << ": " << message
			<< std::endl;

		// Output to console 
		std::cout << logEntry.str();

		// Output to log file 
		if (logFile.is_open()) {
			logFile << logEntry.str();
			logFile.flush(); // Ensure immediate write to file 
		}
	}

private:
	std::ofstream logFile; // File stream for the log file 

	// Converts log level to a string for output 
	std::string levelToString(LogLevel level)
	{
		switch (level) {
		case DEBUG:
			return "DEBUG";
		case INFO:
			return "INFO";
		case WARNING:
			return "WARNING";
		case ERROR:
			return "ERROR";
		case CRITICAL:
			return "CRITICAL";
		default:
			return "UNKNOWN";
		}
	}
};