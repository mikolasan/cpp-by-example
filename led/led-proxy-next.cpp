#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <future>
#include <map>
#include <mutex>
#include <regex>
#include <stack>
#include <string>
#include <thread>
#include <vector>

#include <ryml_std.hpp>
#include <ryml.hpp>
#include <zmq.hpp>

#include "show.h"
#include "show_udp_cached.hpp"
#include "config.hpp"

using Show = ShowUdpCached;
using namespace std::chrono_literals;

// (select|play|stop):(game_name):(sound_name)
const std::regex message_regex("(\\w*)\\|([a-z_]*)\\|(.*)", std::regex_constants::ECMAScript);

template <typename R>
bool is_future_ready(std::future<R> const& f)
{
    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}

using ShowPlayingFuture = std::future<std::string>;

// (action, show_name)
using ParsedMessage = std::tuple<std::string, std::string>;

ParsedMessage receive_and_parse(zmq::socket_t& socket)
{
    zmq::message_t request;
    // receive a request from client
    const zmq::recv_result_t res = socket.recv(request, zmq::recv_flags::none);
    std::string message = request.to_string();

    std::smatch match;
    if (!std::regex_match(message, match, message_regex)) {
        return {"", ""};
    }
    
    if (match.size() < 4) {
        return {"", ""};
    }

    std::ssub_match action_sub_match = match[1];
    std::string action = action_sub_match.str();
    
    std::ssub_match game_name_sub_match = match[2];
    std::string game_name = game_name_sub_match.str();

    std::ssub_match sound_name_sub_match = match[3];
    std::string sound_name = sound_name_sub_match.str();

    // std::cout << "Parsed [action: " << action
    //     << " | game: " << game_name
    //     << " | sound: " << sound_name 
    //     << "]" << std::endl;

    std::string show_name = game_name + "|" + sound_name;
    return {action, show_name};
}

void process_command(
    const std::string& action,
    const std::string& show_name,
    AbstractShow** current_show,
    std::deque<ShowPlayingFuture>& show_futures,
    std::stack<AbstractShow*>& background_shows,
    std::map<std::string, Show*>& show_buffer,
    TimePoint& last_message_time)
{
    AbstractShow* show = *current_show;

    if (!show_buffer.contains(show_name)) {
        // std::cout << "No mapping for '" << show_name << "'" << std::endl;
        return;
    }
    TimePoint message_time = Clock::now();
    uint64_t time_past = std::chrono::duration_cast<std::chrono::milliseconds>(
            message_time - last_message_time).count();
    last_message_time = message_time;
    std::cout << ">> [" 
        << time_past
        << "] <<" << action
        << ">> " << show_name
        << std::endl;

    // std::lock_guard<std::mutex> lock(show_mutex);
    
    if (action == "select" || action == "play") {
        // if (action == "select") {
        //     // clear the stack
        //     std::stack<AbstractShow*>().swap(background_shows);
        // }
        
        // the same show
        if (show_name == show->get_codename()) {

            if (show->state.get_background_flag()) {
                std::cout << "[STRANGE] NEXT SHOW Keep old show playing..." << std::endl;
                return;
            } else {
                if (show->state.get_ignore_stop_flag()) {
                    std::cout << "NO REWIND" << std::endl;
                } else {
                    dynamic_cast<Show*>(show)->rewind();
                }
                return;
            }


        }
        
        // stop or hide current
        if (show->state.get_background_flag()) {
            std::cout << "set background show " 
                << show->get_codename() << " (visible to FALSE)" 
                << std::endl;
            show->state.set_visible(false);
        } else {
            if (show_buffer[show_name]->state.get_background_flag()) {
                std::cout << "finish current show, start background hidden" << std::endl;
                // ignore background show
                Show* bg_show = show_buffer[show_name];
                background_shows.push(bg_show);
                bg_show->state.set_visible(false);
                show_futures.emplace_back(
                    std::async(std::launch::async, &Show::send_blocking, dynamic_cast<Show*>(bg_show))
                );
                return;
            } else {
                show->stop();
            }
        }
        
        std::cout << "Next show: " << show_name << std::endl;
        *current_show = show_buffer[show_name];
        show = *current_show;
        // show->state.set_playing(true);
        if (show->state.get_background_flag()) {
            std::cout << "PUSH background show " << show->get_codename() << std::endl;
            background_shows.push(show);
        }
        
        // ShowPlayingFuture show_ended = \
        //         std::async(std::launch::async, &Show::send_blocking, dynamic_cast<Show*>(show));
        // show_futures.push_back(std::move(show_ended));

        show_futures.emplace_back(
                std::async(std::launch::async, &Show::send_blocking, dynamic_cast<Show*>(show))
        );

    } else if (action == "stop") {
        if (show_name == show->get_codename()) {
            if (show->state.get_ignore_stop_flag()) {
                std::cout << "Ignore stop (!)" << std::endl;
                return;
            }
            std::cout << "FORCE STOP show " << show_name << std::endl;
            show->state.force_stop();
        } else if (!background_shows.empty() && show_name == background_shows.top()->get_codename()) {
            std::cout << "FORCE STOP background show " << show_name << std::endl;
            background_shows.top()->state.force_stop();
        }
    } else {
        std::cout << "Unknown action " << action << std::endl;
    }
}

void on_show_ended(
    const std::string& show_name,
    AbstractShow** current_show,
    std::stack<AbstractShow*>& background_shows,
    std::map<std::string, Show*>& show_buffer)
{
    AbstractShow* show = *current_show;

    std::cout << "on_show_ended " << show_name << std::endl;
    // another show is playing
    std::cout << "current show " << show->get_codename() << std::endl;
    if (show->get_codename() != show_name) {
        if (show->state.get_background_flag()) {
            if (!show->state.get_visible_flag()) {
                std::cout << "[WARNING] switching to background show (visible to TRUE)" << std::endl;
                show->state.set_visible(true);
            }
        } else {
            std::cout << "nothing" << std::endl;
        }

        return;
    }

    if (background_shows.size() > 1) {
        AbstractShow* background_show = background_shows.top();
        if (background_show->state.is_force_stopped()) {
            std::cout << "POP background show " << background_show->get_codename() << std::endl;
            background_shows.pop();
        }
    }

    if (!background_shows.empty()) {
        AbstractShow* background_show = background_shows.top();
        std::cout << "Return to background show " << background_show->get_codename() << " (visible to TRUE)" << std::endl;
        *current_show = background_show;
        show = *current_show;
        show->state.set_visible(true);
    }
}

int main(int argc, char const *argv[])
{
    std::string contents = get_file_contents("config.yml");
    std::map<std::string, Show*> show_buffer = read_show_mapping<Show>(contents);

    ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(contents));
    std::string hostname;
    tree["hostname"] >> hostname;

    const uint16_t port = 6454;
    std::shared_ptr<UdpSender> udp_sender(new UdpSender);
    udp_sender->set_destination(hostname, port);

    for (const auto&[show_name, show] : show_buffer) {
        show->udp_sender = udp_sender;
        show->set_sender(udp_sender);
    }

    // get signals from the game and create next show
    auto message_listener = std::thread([&show_buffer](){
        // initialize the zmq context with a single IO thread
        zmq::context_t context{1};
        // construct a SUB socket and bind to interface
        zmq::socket_t socket{context, zmq::socket_type::sub};
        socket.set(zmq::sockopt::subscribe, "");

        std::cout << "Started listening on port 5555" << std::endl;
        socket.bind("tcp://*:5555");

        AbstractShow* show = nullptr;
        std::stack<AbstractShow*> background_shows;
        show = show_buffer["background_show"];
        background_shows.push(show);
        dynamic_cast<Show*>(show)->send_async();

        // AbstractShow* next_show = nullptr;
        TimePoint last_message_time = TimePoint();
        
        std::future<ParsedMessage> message_parsed;
        std::deque<ShowPlayingFuture> show_futures;

        while (true) {
            if (!message_parsed.valid()) {
                message_parsed = \
                    std::async(std::launch::async, receive_and_parse, std::ref(socket));
            }

            if (is_future_ready(message_parsed)) {
                const auto [action, show_name] = message_parsed.get();
                process_command(action, show_name, &show, show_futures, background_shows, show_buffer, last_message_time);
                // and start receiving already
                message_parsed = \
                    std::async(std::launch::async, receive_and_parse, std::ref(socket));
            }
            
            if (!show_futures.empty()) {
                for (auto it = show_futures.begin(); it != show_futures.end();) {
                    ShowPlayingFuture& show_ended = *it;
                    if (is_future_ready(show_ended)) {
                        std::string show_name = show_ended.get();
                        on_show_ended(show_name, &show, background_shows, show_buffer);
                        it = show_futures.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16)); // FPS 60 // TODO
        }
    });
    message_listener.join();

    return 0;
}