// ReSharper disable CppClangTidyBugproneSuspiciousInclude

#include <string>
#include "../httplib.h"
#include <future>

class DiscordService
{


private:
	httplib::Client client_;

public:
	DiscordService() : client_(httplib::Client("https://discord.com/api"))
	{

	}

	std::string get_discord_id(std::string auth_code)
	{
		std::string auth_code_processed = extract_code(auth_code);

		std::string command = "curl --location https://discord.com/api/oauth2/token "
			"--header \"Content-Type: application/x-www-form-urlencoded\" "
			"--data-urlencode \"client_id=1273414933874479185\" "
			"--data-urlencode \"client_secret=" + std::string(std::getenv("DiscordAppSecret")) + "\" "
			"--data-urlencode \"grant_type=authorization_code\" "
			"--data-urlencode \"code=" + auth_code_processed + "\" "
			"--data-urlencode \"redirect_uri=https://spcloud.almavid.ru\"";

		auto code_request = std::async(std::launch::async, &DiscordService::execute_command, this, command);

		std::string access_token = parse_access_token(code_request.get());


		command = "curl --location https://discord.com/api/users/@me "
			"--header \"Authorization: Bearer " + access_token + "\" ";

		auto me_request = std::async(std::launch::async, &DiscordService::execute_command, this, command);

		std::string user_id = extract_user_id(me_request.get());

		return user_id;
	}

	std::string extract_user_id(const std::string& input) {
		std::string search_pattern = "\"id\":\"";
		std::size_t start_pos = input.find(search_pattern);
		if (start_pos == std::string::npos) {
			throw std::runtime_error("discord_id not found");
		}

		start_pos += search_pattern.length();
		std::size_t end_pos = input.find("\"", start_pos);
		if (end_pos == std::string::npos) {
			throw std::runtime_error("End of discord_id not found");
		}

		return input.substr(start_pos, end_pos - start_pos);
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

	std::string extract_code(const std::string& json_str) {
		std::string key = "\"code\":";
		size_t start = json_str.find(key);

		if (start != std::string::npos) {
			start += key.length();
			while (start < json_str.length() && (json_str[start] == ' ' || json_str[start] == '\"' || json_str[start] == ':')) {
				start++;
			}

			size_t end = json_str.find('\"', start);
			if (end != std::string::npos) {
				return json_str.substr(start, end - start);
			}
		}
		return "";
	}

	std::string parse_access_token(const std::string& response) {
		std::string token_label = "\"access_token\": \"";
		size_t start_pos = response.find(token_label);
		if (start_pos != std::string::npos) {
			start_pos += token_label.length();
			size_t end_pos = response.find("\"", start_pos);
			if (end_pos != std::string::npos) {
				return response.substr(start_pos, end_pos - start_pos);
			}
		}
		return ""; 
	}
};
