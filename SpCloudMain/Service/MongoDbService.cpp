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
			"--header 'api-key: Q1NfSCrruUAzsxdrjhZd3sjSwiqbdSFmCLeaCatZiuohUXsvEq9RtEAeG0JL2Jd7' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		return response;
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
			"--header 'api-key: Q1NfSCrruUAzsxdrjhZd3sjSwiqbdSFmCLeaCatZiuohUXsvEq9RtEAeG0JL2Jd7' "
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
			"--header 'api-key: Q1NfSCrruUAzsxdrjhZd3sjSwiqbdSFmCLeaCatZiuohUXsvEq9RtEAeG0JL2Jd7' "
			"--data-raw '" + json_data + "'";

		auto request = std::async(std::launch::async, &MongoDbService::execute_command, this, command);

		std::string response = request.get();

		if (response == "{\"document\":null}")
		{
			return "Success";
		}

		return "App name isn't free please select another one";
	}

	std::string add_app(std::string name, std::string user_id, std::string url, std::string url_on_local_mahcine, std::string target)
	{
		std::string json_data = R"({
		"dataSource": "SpCloudCluster",
		"database": "SpCloud",
		"collection": "Apps",
		"document": {
			"name": ")" + name + R"(",
			"user_id": ")" + user_id + R"(",
			"url": ")" + url + R"(",
			"url_on_local_machine": ")" + url_on_local_mahcine + R"(",
			"target": ")" + target + R"("
			}
		})";

		std::string command = "curl --location 'https://eu-central-1.aws.data.mongodb-api.com/app/data-zvcqvrr/endpoint/data/v1/action/insertOne' "
			"--header 'Content-Type: application/json' "
			"--header 'api-key: Q1NfSCrruUAzsxdrjhZd3sjSwiqbdSFmCLeaCatZiuohUXsvEq9RtEAeG0JL2Jd7' "
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
