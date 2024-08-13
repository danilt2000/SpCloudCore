#include <string>

class User {
private:
        int id;
        std::string name;
        std::string authToken;
        bool isBanned;

public:
        // Конструктор
        User(int id, const std::string& name, const std::string& authToken, bool isBanned)
                : id(id), name(name), authToken(authToken), isBanned(isBanned) {}

        // Геттеры и сеттеры для Id
        int getId() const {
                return id;
        }

        void setId(int id) {
                this->id = id;
        }

        // Геттеры и сеттеры для Name
        std::string getName() const {
                return name;
        }

        void setName(const std::string& name) {
                this->name = name;
        }

        // Геттеры и сеттеры для AuthToken
        std::string getAuthToken() const {
                return authToken;
        }

        void setAuthToken(const std::string& authToken) {
                this->authToken = authToken;
        }

        // Геттеры и сеттеры для IsBanned
        bool getIsBanned() const {
                return isBanned;
        }

        void setIsBanned(bool isBanned) {
                this->isBanned = isBanned;
        }
};
