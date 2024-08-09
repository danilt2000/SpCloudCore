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
	std::cout << "SpCloud start\n";

	httplib::Server svr;

	svr.Get("/ping", [](const httplib::Request& req, httplib::Response& res)
		{
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
