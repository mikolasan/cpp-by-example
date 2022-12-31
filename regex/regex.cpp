#include <iostream>
#include <regex>
#include <string>

int main(int argc, char const *argv[])
{
    // std::string message = "play|cauldrons_of_fortune|Sound.ogg";
    std::string message = "play|cauldrons_of_fortune|";
    std::cout << "Message: " << message << std::endl;

    // (select|play|stop):(game_name):(sound_name)
    const std::regex frontend_sound_regex("(\\w*)\\|([a-z_]*)\\|(.*)", std::regex_constants::ECMAScript);
    std::smatch match;
    if (!std::regex_match(message, match, frontend_sound_regex)) {
        return 1;
    }
    
    if (match.size() < 4) {
        return 2;
    }

    std::ssub_match action_sub_match = match[1];
    std::string action = action_sub_match.str();
    
    std::ssub_match game_name_sub_match = match[2];
    std::string game_name = game_name_sub_match.str();

    std::ssub_match sound_name_sub_match = match[3];
    std::string sound_name = sound_name_sub_match.str();

    std::cout << "Parsed [action: " << action
        << " | game: " << game_name
        << " | sound: " << sound_name 
        << "]" << std::endl;
        
    return 0;
}
