#include <string>
#ifndef APP_H
#define APP_H
class App {
private:
        std::string name;
        std::string user_id;
        std::string url;
        std::string url_on_local_machine;
        std::string target;

public:
        App(const std::string& name, const std::string& user_id, const std::string& url,
                const std::string& url_on_local_machine, const std::string& target)
                : name(name), user_id(user_id), url(url), url_on_local_machine(url_on_local_machine), target(target) {}

        std::string get_name() const { return name; }
        std::string get_user_id() const { return user_id; }
        std::string get_url() const { return url; }
        std::string get_url_on_local_machine() const { return url_on_local_machine; }
        std::string get_target() const { return target; }

        void set_name(const std::string& name) { this->name = name; }
        void set_user_id(const std::string& user_id) { this->user_id = user_id; }
        void set_url(const std::string& url) { this->url = url; }
        void set_url_on_local_machine(const std::string& url_on_local_machine) { this->url_on_local_machine = url_on_local_machine; }
        void set_target(const std::string& target) { this->target = target; }
};
#endif // APP_H
