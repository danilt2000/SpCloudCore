// SpCloudMain.cpp : Defines the entry point for the application.
//

// ReSharper disable CppClangTidyBugproneSuspiciousInclude

#include "SpCloudMain.h"
#include "httplib.h"
#include "Controllers/PublishController.cpp"
//#include "Service/AuthorizationService.cpp"
//#include "Service/FileProcessingService.cpp"
using namespace std;

enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

class Logger {
public:
        // Constructor: Opens the log file in append mode 
        Logger(const string& filename)
        {
                logFile.open(filename, ios::app);
                if (!logFile.is_open()) {
                        cerr << "Error opening log file." << endl;
                }
        }

        // Destructor: Closes the log file 
        ~Logger() { logFile.close(); }

        // Logs a message with a given log level 
        void log(LogLevel level, const string& message)
        {
                // Get current timestamp 
                time_t now = time(0);
                tm* timeinfo = localtime(&now);
                char timestamp[20];
                strftime(timestamp, sizeof(timestamp),
                        "%Y-%m-%d %H:%M:%S", timeinfo);

                // Create log entry 
                ostringstream logEntry;
                logEntry << "[" << timestamp << "] "
                        << levelToString(level) << ": " << message
                        << endl;

                // Output to console 
                cout << logEntry.str();

                // Output to log file 
                if (logFile.is_open()) {
                        logFile << logEntry.str();
                        logFile
                                .flush(); // Ensure immediate write to file 
                }
        }

private:
        ofstream logFile; // File stream for the log file 

        // Converts log level to a string for output 
        string levelToString(LogLevel level)
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

int main()
{
        Logger logger("logfile.txt"); //

	std::cout << "SpCloud start\n";

	httplib::Server svr;

	logger.log(INFO, "Program started.");

	svr.Get("/ping", [](const httplib::Request& req, httplib::Response& res)
		{
			std::cout << "Ping-\n";
                        logger.log(INFO, "App was pinged.");

			res.set_content("Pong", "text/plain");

			httplib::Headers test = req.headers;

		});

	AuthorizationService authorization_service;

	FileProcessingService file_processing;

	//PublishController publish_controller(svr, authorization_service, file_processing);

	PublishController publish_controller(svr, authorization_service, file_processing);

	std::cout << "Server is running at http://localhost:8080" << '\n';

	svr.listen("0.0.0.0", 8080);
}
