#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char const *argv[])
{
    std::string game_name = "the_last_samurai";
    std::string game_version = "0.1.0";
    std::string token = "cc1e240aacb641fed0050d2c2f16db918b4a7c10";

    json j = {
        {"command", "register"},
        {"register", {
            {"game_name", game_name},
            {"game_version", game_version},
            {"token", token},
        }}
    };

    std::string data_to_send = j.dump();

    std::cout << j.dump(4) << std::endl;

    return 0;
}
