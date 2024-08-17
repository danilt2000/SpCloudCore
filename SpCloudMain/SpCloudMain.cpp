// ReSharper disable CppClangTidyBugproneSuspiciousInclude

#include "SpCloudMain.h"
#include "httplib.h"
#include "Controllers/PublishController.cpp"
#include "Service/DiscordService.cpp"
#include "Service/MongoDbService.cpp"
#include "Models/App.cpp"

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

			std::string user_id = req.get_file_value("UserId").content;
			std::string name = req.get_file_value("Name").content;
			ranges::transform(name, name.begin(), [](unsigned char c) { return std::tolower(c); });
			std::string target = req.get_file_value("Target").content;
			std::string authorization_token = req.get_header_value("Authorization");

			string is_user_can_publish_response = mongo_service.is_user_can_publish(authorization_token);

			if (is_user_can_publish_response != "Success")
			{
				res.set_content(is_user_can_publish_response, "text/plain");

				return;
			}

			string is_app_name_response = mongo_service.is_app_name_free(name);

			if (is_app_name_response != "Success")
			{
				res.set_content(is_app_name_response, "text/plain");

				return;
			}

			App* app = new App(name, user_id, "url", "local_url", target/*,"service_name"*/);

			publish_controller.process_publish(req, app);

			//mongo_service.add_app(app->get_name(), app->get_user_id(), app->get_url(),
			//	app->get_url_on_local_machine(), app->get_target()/*, app->get_service_name()*/);//TODO UNCOMMENT AND FIX

			delete app;

			res.set_content("App is running on address ????", "text/plain");//Todo add app address showing 
		});


	svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res)
		{
			string discord_id = discord_service.get_discord_id(req.body);

			std::string result = mongo_service.get_user_info(discord_id);

			res.set_content(result, "text/plain");
		});

	svr.listen("0.0.0.0", 8081);
}
