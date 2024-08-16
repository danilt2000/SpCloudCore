// ReSharper disable CppClangTidyBugproneSuspiciousInclude
//#include <curl/curl.h>/*
#include <iostream>
#include "../httplib.h"
#include <string>
#include <future>
//#include "../src/mongocxx/"s

class MongoDbService
{

public:
	MongoDbService()
	{
		//mongocxx::instance inst{};
		//const auto uri = mongocxx::uri{ "mongodb+srv://loker:<password>@spcloudcluster.xwpnw.mongodb.net/?retryWrites=true&w=majority&appName=SpCloudCluster" };
		//// Set the version of the Stable API on the client
		//mongocxx::options::client client_options;
		//const auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
		//client_options.server_api_opts(api);
		//// Setup the connection and get a handle on the "admin" database.
		//mongocxx::client conn{ uri, client_options };
		//mongocxx::database db = conn["admin"];
		//// Ping the database.
		//const auto ping_cmd = bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("ping", 1));
		//db.run_command(ping_cmd.view());
		//std::cout << "Pinged your deployment. You successfully connected to MongoDB!" << std::endl;
	}

	std::string get_user_info(std::string user_id)
	{
		/*std::string json_data = R"({
	"dataSource": "SpCloudCluster",
	"database": "SpCloud",
	"collection": "User",
	"document": {
	    "name": "Item Name",
	    "value": "Item Value"
	}
    })";
		std::string command = "curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/insertOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: Q1NfSCrruUAzsxdrjhZd3sjSwiqbdSFmCLeaCatZiuohUXsvEq9RtEAeG0JL2Jd7' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);
*/

		std::string json_data = R"({
    "dataSource": "SpCloudCluster",
    "database": "SpCloud",
    "collection": "AllowedUsers",
    "filter": {
        "discord_id": ")" + user_id + R"("
    }
})";

		std::string command = "curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/findOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: Q1NfSCrruUAzsxdrjhZd3sjSwiqbdSFmCLeaCatZiuohUXsvEq9RtEAeG0JL2Jd7' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		return response;
		//return response.find("\"document\": null") == std::string::npos;
	}

	std::string execute_command(const std::string& command) {
		std::array<char, 128> buffer;
		std::string result;
		std::shared_ptr<FILE> pipe(popen(command.c_str(), "r"), pclose);
		if (!pipe) throw std::runtime_error("popen() failed!");
		while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
			result += buffer.data();
		}
		return result;
	}
};
