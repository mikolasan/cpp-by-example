#pragma once

#include <atomic>
#include <chrono>
#include <list>
#include <map>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

#include <ryml.hpp>
#include <ryml_std.hpp>


class ShowState {
public:
    ShowState() :
        background(false),
        loop(false),
        ignore_stop(false),
        show_playing(false),
        force_stopped(false)
    {}

    virtual void play() {
        show_playing = true;
        force_stopped = false;
    }

    virtual void stop() {
        show_playing = false;
    }
    
    void force_stop() {
        if (ignore_stop) {
            return;
        }
        // std::cout << "FORCE STOP " << codename << std::endl;
        show_playing = false;
        force_stopped = true;
    }

    bool is_force_stopped() const {
        return force_stopped;
    }

    bool is_playing() const {
        return show_playing;
    }

    bool needs_to_play() const {
        return background || loop;
    }

    // bool is_playing_in_background() const {
    //     background && is_playing
    // }

    void set_loop(bool flag) {
        loop = flag;
    }

    bool get_loop_flag() const {
        return loop;
    }

    void set_ignore_stop(bool flag) {
        ignore_stop = flag;
    }

    bool get_ignore_stop_flag() const {
        return ignore_stop;
    }

    void set_background(bool flag) {
        background = flag;
    }

    bool get_background_flag() const {
        return background;
    }

private:
    bool background;
    bool loop;
    bool ignore_stop;
    std::atomic<bool> show_playing;
    std::atomic<bool> force_stopped;
};


class AbstractShow {
public:
    AbstractShow(
        const std::string& codename,
        const std::string& filename
    ) :
        codename(codename),
        filename(filename)
    {}
    
    virtual void play() {};
    virtual void send() = 0;
    virtual void send_to(const std::string& hostname) {}
    
    void apply_config(const ryml::NodeRef& config) {
        bool background = false;
        if (config.has_child(ryml::to_csubstr("background"))) {
            config["background"] >> background;
        }
        state.set_background(background);

        bool loop = false;
        if (config.has_child(ryml::to_csubstr("loop"))) {
            config["loop"] >> loop;
        }
        state.set_loop(loop);

        bool ignore_stop = false;
        if (config.has_child(ryml::to_csubstr("ignore_stop"))) {
            config["ignore_stop"] >> ignore_stop;
        }
        state.set_ignore_stop(ignore_stop);

        do_apply_config(config);
    }

    virtual void stop() {
        state.stop();
    }
    
    std::string get_codename() const {
        return codename;
    }
    
    std::string get_filename() const {
        return filename;
    }

protected:
    virtual size_t do_send(const std::string_view& data) = 0;
    virtual void do_apply_config(const ryml::NodeRef& config) {}

public:
    ShowState state;

protected:
    std::string codename;
    std::string filename;
    // bool background;
    // bool loop;
    // bool ignore_stop;
    // std::atomic<bool> show_playing;
    // std::atomic<bool> force_stopped;
};

using Duration = std::chrono::microseconds;
using Data = std::string_view;
// <packet data, packet duration, elapsed from start>
using PacketList = std::list<std::tuple<Data, Duration, Duration>>;

std::string get_file_contents(const char *filename);
PacketList cache_file(const std::string& filename);

template <
    typename Show,
    typename = typename std::enable_if<
        std::is_base_of<AbstractShow, Show>::value, Show
    >::type
>
std::map<std::string, Show*> read_show_mapping(std::string config) {
    namespace fs = std::filesystem;

    ryml::Tree tree = ryml::parse(ryml::to_csubstr(config));
    std::string show_dir;
    tree["show_dir"] >> show_dir;

    ryml::NodeRef show_mapping = tree["show_mapping"];
    std::map<std::string, Show*> show_buffer;

    // cache all animations
    for (ryml::NodeRef const& child : show_mapping.children()) {
        
        std::string show_id;
        ryml::from_chars(child.key(), &show_id);
        
        bool background = false;
        if (child.has_child(ryml::to_csubstr("background"))) {
            child["background"] >> background;
        }

        bool loop = false;
        if (child.has_child(ryml::to_csubstr("loop"))) {
            child["loop"] >> loop;
        }

        bool ignore_stop = false;
        if (child.has_child(ryml::to_csubstr("ignore_stop"))) {
            child["ignore_stop"] >> ignore_stop;
        }

        // std::cout << child.has_child(ryml::to_csubstr("show_file")) << std::endl;
        std::string show_file;
        child["show_file"] >> show_file;

        fs::path show_path{show_dir};
        show_path /= show_file;
        std::string fname = show_path.string();

        show_buffer[show_id] = new Show(show_id, fname);
        show_buffer[show_id]->apply_config(child);
    }

    return show_buffer;
}