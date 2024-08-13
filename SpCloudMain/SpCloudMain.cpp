// SpCloudMain.cpp : Defines the entry point for the application.
//

// ReSharper disable CppClangTidyBugproneSuspiciousInclude

#include "SpCloudMain.h"
#include "httplib.h"
#include "Controllers/PublishController.cpp"
//#include "Service/AuthorizationService.cpp"
//#include "Service/FileProcessingService.cpp"
using namespace std;

int main()
{
	string logger_name = "logfile.txt";

	Logger logger(logger_name);

	std::cout << "SpCloud start\n";

	httplib::Server svr;

	logger.log(INFO, "Program started.");

	svr.Get("/ping", [&](const httplib::Request& req, httplib::Response& res)
		{
			std::cout << "Ping-\n";

			logger.log(INFO, "App was pinged.");

			res.set_content("Pong", "text/plain");

			httplib::Headers test = req.headers;
		});

	AuthorizationService authorization_service;

	auto file_processing = std::make_shared<FileProcessingService>(logger);

	PublishController publish_controller(svr, authorization_service, file_processing, logger);

	std::cout << "Server is running at http://localhost:8081" << '\n';

	svr.Post("/publish", [&](const httplib::Request& req, httplib::Response& res)
		{
			logger.log(INFO, "Start publish from main");

			publish_controller.process_publish(req, res);

			res.set_content("App is running on address ????", "text/plain");//Todo add app address showing 
		});

	svr.listen("0.0.0.0", 8081);
}
