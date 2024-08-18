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

			App* app = new App(name, user_id, "url", "local_url", target, "service_name");

			publish_controller.process_publish(req, app);

			mongo_service.add_app(app->get_name(), app->get_user_id(), app->get_url(),
				app->get_url_on_local_machine(), app->get_target(), app->get_service_name());

			mongo_service.increase_user_app_count_(app->get_user_id());

			res.set_content("App is running on address:" + app->get_url(), "text/plain");

			delete app;
		});


	svr.Put("/update", [&](const httplib::Request& req, httplib::Response& res)
		{
			logger.log(INFO, "Start updating app");

			std::string user_id = req.get_file_value("UserId").content;

			std::string name = req.get_file_value("Name").content;

			std::string authorization_token = req.get_header_value("Authorization");

			string is_user_banned = mongo_service.is_user_banned(authorization_token);

			if (is_user_banned != "Success")
			{
				res.set_content(is_user_banned, "text/plain");

				return;
			}

			string is_user_app_owner = mongo_service.is_user_app_owner(authorization_token, name);

			if (is_user_app_owner != "Success")
			{
				res.set_content(is_user_app_owner, "text/plain");

				return;
			}

			App* app = new App(name, user_id, "url", "local_url", "target", "service_name");

			publish_controller.process_update(req, app);

			res.set_content("App was updated", "text/plain");

			delete app;
		});

	svr.Delete("/delete", [&](const httplib::Request& req, httplib::Response& res)
		{
			logger.log(INFO, "Start updating app");

			std::string user_id = req.get_file_value("UserId").content;

			std::string name = req.get_file_value("Name").content;

			std::string authorization_token = req.get_header_value("Authorization");

			string is_user_banned = mongo_service.is_user_banned(authorization_token);

			if (is_user_banned != "Success")
			{
				res.set_content(is_user_banned, "text/plain");

				return;
			}

			string is_user_app_owner = mongo_service.is_user_app_owner(authorization_token, name);

			if (is_user_app_owner != "Success")
			{
				res.set_content(is_user_app_owner, "text/plain");

				return;
			}

			App* app = new App(name, user_id, "url", "local_url", "target", "service_name");

			publish_controller.process_delete(req, app);

			res.set_content("App was deleted", "text/plain");

			mongo_service.decrease_user_app_count(app->get_user_id());

			mongo_service.delete_document("Apps", "name", app->get_name());

			delete app;
		});

	svr.Get("/apps", [&](const httplib::Request& req, httplib::Response& res)
		{
			std::string authorization_token = req.get_header_value("Authorization");

			string is_user_banned = mongo_service.is_user_banned(authorization_token);

			if (is_user_banned != "Success")
			{
				res.set_content(is_user_banned, "text/plain");

				return;
			}

			string user_id = mongo_service.get_user_id_by_auth_token(authorization_token);

			string app_list = mongo_service.get_app_list(user_id);

			res.set_content(app_list, "text/plain");
		});

	svr.Post("/login", [&](const httplib::Request& req, httplib::Response& res)
		{
			string discord_id = discord_service.get_discord_id(req.body);

			std::string result = mongo_service.get_user_info(discord_id);

			res.set_content(result, "text/plain");
		});

	svr.listen("0.0.0.0", 8081);
}
