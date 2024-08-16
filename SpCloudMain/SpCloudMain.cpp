// SpCloudMain.cpp : Defines the entry point for the application.
//

// ReSharper disable CppClangTidyBugproneSuspiciousInclude

#include "SpCloudMain.h"
#include "httplib.h"
#include "Controllers/PublishController.cpp"
#include "Service/DiscordService.cpp"
#include "Service/MongoDbService.cpp"

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

	std::cout << "Server is running at http://localhost:8081" << '\n';

	AuthorizationService authorization_service;

	auto file_processing = std::make_shared<FileProcessingService>(logger);

	PublishController publish_controller(svr, authorization_service, file_processing, logger);

	DiscordService discord_service;

	MongoDbService mongo_service;

	svr.Post("/publish", [&](const httplib::Request& req, httplib::Response& res)
		{
			logger.log(INFO, "Start publish from main");

			string is_user_can_publish_response = mongo_service.is_user_can_publish("khBuvDWPHOhPSiQNVQZm9PM0VF29dqAaDBjWX4BnxJKzRvg0Gm");//TODO UNCOMMENT AND FIX

			if (is_user_can_publish_response != "Success")
			{
				res.set_content(is_user_can_publish_response, "text/plain");//Todo add app address showing

				return;
			}

			//publish_controller.process_publish(req, res);//TODO UNCOMMENT AND FIX

				//mongo_service.add_app("test", "test", "test", "test", "test");//TODO UNCOMMENT AND FIX


			res.set_content("App is running on address ????", "text/plain");//Todo add app address showing 
		});


	svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res)
		{
			string discord_id = discord_service.get_discord_id(req.body);

			std::string result = mongo_service.get_user_info(discord_id);

			res.set_content(result, "text/plain");
		});


	/*httplib::Client cli("https://discord.com/api/oauth2/token");

	std::string jsonData = R"({
	"dataSource": "Cluster0",
	"database": "myDatabase",
	"collection": "items",
	"document": {
	    "name": "Item Name",
	    "value": "Item Value"
	}
    })";

	auto res = cli.Post("/app/data-abcde/endpoint/data/v1/action/insertOne", jsonData, "application/json");

	if (res && res->status == 200) {
		std::cout << "Success: " << res->body << std::endl;
	}
	else {
		std::cerr << "Error: " << res.error() << std::endl;
	}*/

	svr.listen("0.0.0.0", 8081);
}
