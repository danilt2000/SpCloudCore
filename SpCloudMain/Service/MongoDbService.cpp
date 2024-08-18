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
	}

	std::string get_user_info(std::string user_id)
	{
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
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		return response;
	}

	std::string increase_user_app_count_(std::string user_id)
	{
		std::string json_data = "{"
			"\"dataSource\": \"SpCloudCluster\","
			"\"database\": \"SpCloud\","
			"\"collection\": \"AllowedUsers\","
			"\"filter\": {"
			"\"discord_id\": \"" + user_id + "\""
			"},"
			"\"update\": {"
			"\"$inc\": { \"app_count\": 1 }"
			"}"
			"}";

		std::string command =
			"curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/updateOne' "
			"--header 'Content-Type: application/json' "
			"--header 'Access-Control-Request-Headers: *' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		return response;
	}

	std::string decrease_user_app_count(std::string user_id)
	{
		std::string json_data = "{"
			"\"dataSource\": \"SpCloudCluster\","
			"\"database\": \"SpCloud\","
			"\"collection\": \"AllowedUsers\","
			"\"filter\": {"
			"\"discord_id\": \"" + user_id + "\""
			"},"
			"\"update\": {"
			"\"$inc\": { \"app_count\": -1 }"
			"}"
			"}";

		std::string command =
			"curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/updateOne' "
			"--header 'Content-Type: application/json' "
			"--header 'Access-Control-Request-Headers: *' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		return response;
	}

	std::string delete_document( std::string collection, std::string filter_field, std::string filter_value)
	{
		std::string json_data = "{"
			"\"dataSource\": \"SpCloudCluster\","
			"\"database\": \"SpCloud\","
			"\"collection\": \"" + collection + "\","
			"\"filter\": {"
			"\"" + filter_field + "\": \"" + filter_value + "\""
			"}"
			"}";

		std::string command =
			"curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/deleteOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		return response;
	}

	std::string get_user_id_by_auth_token(std::string auth_token)
	{
		std::string json_data = R"({
        "dataSource": "SpCloudCluster",
        "database": "SpCloud",
        "collection": "AllowedUsers",
        "filter": {
            "auth_token": ")" + auth_token + R"("
        }
    })";

		std::string command =
			"curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/findOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		size_t pos_user_id = response.find("\"discord_id\":");
		if (pos_user_id == std::string::npos) {
			// Handle the case where "discord_id" is not found in the response
			return ""; // or some other error handling
		}

		pos_user_id += 13; // 12 characters for "discord_id": and 1 for the opening quote

		// Find the end of the user_id, either at a comma or the closing bracket
		size_t end_pos = response.find_first_of(",}", pos_user_id);
		if (end_pos == std::string::npos) {
			// Handle malformed JSON
			return ""; // or some other error handling
		}

		// Extract the user_id from the response, making sure to strip any surrounding quotes
		std::string user_id = response.substr(pos_user_id, end_pos - pos_user_id);
		if (!user_id.empty() && user_id.front() == '"' && user_id.back() == '"') {
			user_id = user_id.substr(1, user_id.length() - 2);
		}

		return user_id;
	}

	std::string is_user_app_owner(std::string auth_token, std::string name_app)//Todo check it 
	{
		std::string user_id = get_user_id_by_auth_token(auth_token);

		std::string json_data = R"({
		"dataSource": "SpCloudCluster",
		"database": "SpCloud",
		"collection": "Apps",
		"filter": {
		"user_id": ")" + user_id + R"(",
		"name": ")" + name_app + R"("
		}
		})";

		std::string command = "curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/findOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		if (response != "{\"document\":null}")
		{
			return "Success";
		}

		return "User isn't app owner";
	}


	std::string is_user_can_publish(std::string auth_token)
	{
		std::string json_data = R"({
		"dataSource": "SpCloudCluster",
		"database": "SpCloud",
		"collection": "AllowedUsers",
		"filter": {
			"auth_token": ")" + auth_token + R"("
		}
		})";

		std::string command = "curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/findOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		size_t pos_is_user_banned = response.find("\"is_banned\":");

		std::string is_banned_value = response.substr(pos_is_user_banned + 12, response.find_first_of(",}", pos_is_user_banned) - pos_is_user_banned - 12);

		if (is_banned_value == "true")
		{
			return "Fail publish: user is banned";
		}

		size_t pos_apps_limit_count = response.find("\"apps_limit_count\":");

		std::string apps_limit_count = response.substr(pos_apps_limit_count + 19, response.find_first_of(",}", pos_apps_limit_count) - pos_apps_limit_count - 19);

		int apps_limit_count_int = std::stoi(apps_limit_count);

		size_t pos_app_count = response.find("\"app_count\":");

		std::string app_count = response.substr(pos_app_count + 12, response.find_first_of(",}", pos_app_count) - pos_app_count - 12);

		int app_count_int = std::stoi(app_count);

		if (app_count_int >= apps_limit_count_int)
		{
			return "Fail publish: user reached publish limit user have " + app_count + " apps with user limit " + apps_limit_count;
		}

		return "Success";
	}

	std::string is_user_banned(std::string auth_token)
	{
		std::string json_data = R"({
		"dataSource": "SpCloudCluster",
		"database": "SpCloud",
		"collection": "AllowedUsers",
		"filter": {
			"auth_token": ")" + auth_token + R"("
		}
		})";

		std::string command = "curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/findOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		size_t pos_is_user_banned = response.find("\"is_banned\":");

		std::string is_banned_value = response.substr(pos_is_user_banned + 12, response.find_first_of(",}", pos_is_user_banned) - pos_is_user_banned - 12);

		if (is_banned_value == "true")
		{
			return "Fail publish: user is banned";
		}

		return "Success";
	}

	std::string is_app_name_free(std::string name)
	{
		std::string json_data = R"({
		"dataSource": "SpCloudCluster",
		"database": "SpCloud",
		"collection": "Apps",
		"filter": {
			"name": ")" + name + R"("
		}
		})";

		std::string command = "curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/findOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		if (response == "{\"document\":null}")
		{
			return "Success";
		}

		return "App name isn't free please select another one";
	}

	std::string add_app(std::string name, std::string user_id, std::string url, std::string url_on_local_mahcine, std::string target, std::string service_name)
	{
		std::string json_data = R"({
		"dataSource": "SpCloudCluster",
		"database": "SpCloud",
		"collection": "Apps",
		"document": {
			"name": ")" + name + R"(",
			"user_id": ")" + user_id + R"(",
			"url": ")" + url + R"(",
			"service_name": ")" + service_name + R"(",
			"url_on_local_machine": ")" + url_on_local_mahcine + R"(",
			"target": ")" + target + R"("
			}
		})";

		std::string command = "curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/insertOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		return response;
	}

	std::string get_app_list(std::string user_id)//Todo test this method 
	{
		std::string json_data = R"({
		"dataSource": "SpCloudCluster",
		"database": "SpCloud",
		"collection": "Apps",
		"filter": {
			"user_id": ")" + user_id + R"("
		}
		})";

		std::string command = "curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/find' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: " + std::string(std::getenv("MongoDbApiKey")) + "' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		return response;
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
