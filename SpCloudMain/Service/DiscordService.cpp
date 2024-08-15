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

		//std::string body = "client_id=1273414933874479185&"
		//	"client_secret=S_vG4frjlxWoi8mic_GlcxUO0aWxXwRJ&"
		//	"grant_type=authorization_code&"
		//	"code=" + auth_code_processed + "&"
		//	"redirect_uri=https://www.sp-donate.ru/pay/Hepatir";

		//httplib::Headers headers = {
		//    {"Content-Type", "application/x-www-form-urlencoded"}
		//};

		//httplib::Client client("discord.com/api");
		////// Выполняем простой GET-запрос на http://httpbin.org/get
		////auto res = client.Get("/get");
		//auto res = client.Post("/oauth2/token", headers, body, "application/x-www-form-urlencoded");
		////auto res = client_.Post("/oauth2/token", headers, body, "application/x-www-form-urlencoded");

		//if (res && res->status == 200) {
		//	return res->body;
		//}
		//else {
		//	return "Error: ";//Todo write handling this 
		//}


		std::string command = "curl --location https://discord.com/api/oauth2/token "
			"--header \"Content-Type: application/x-www-form-urlencoded\" "
			"--data-urlencode \"client_id=1273414933874479185\" "
			"--data-urlencode \"client_secret=S_vG4frjIxWoi8mic_GlcxUO0aWxXwRJ\" "
			"--data-urlencode \"grant_type=authorization_code\" "
			"--data-urlencode \"code=" + auth_code_processed + "\" "
			"--data-urlencode \"redirect_uri=https://www.sp-donate.ru/pay/Hepatir\"";

		auto future = std::async(std::launch::async, &DiscordService::execute_command, this, command);
		return future.get();
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
			// Пропускаем любые пробелы или символы ':'
			while (start < json_str.length() && (json_str[start] == ' ' || json_str[start] == '\"' || json_str[start] == ':')) {
				start++;
			}

			// Найти конец строки
			size_t end = json_str.find('\"', start);
			if (end != std::string::npos) {
				return json_str.substr(start, end - start);
			}
		}
		return "";
	}
};
