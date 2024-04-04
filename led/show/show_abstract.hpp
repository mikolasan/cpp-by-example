#pragma once

#include <atomic>
#include <chrono>
#include <list>
#include <map>
#include <string>
#include <string_view>
#include <tuple>

#include <ryml.hpp>

class ShowState {
public:
    ShowState() :
        priority("normal"),
        background(false),
        loop(false),
        ignore_stop(false),
        visible(true),
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

    bool needs_to_play() const {
        return background || loop;
    }

    // bool is_playing_in_background() const {
    //     background && is_playing
    // }

    ////

    void set_playing(bool flag) {
        show_playing = flag;
    }
    
    bool is_playing() const {
        return show_playing;
    }

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

    void set_visible(bool flag) {
        visible = flag;
    }

    bool get_visible_flag() const {
        return visible;
    }

    void set_priority(const std::string& value) {
        priority = value;
    }

    std::string get_priority() const {
        return priority;
    }

private:
    std::string priority;
    bool background;
    bool loop;
    bool ignore_stop;
    bool visible; // used for keep backgrounds playing but not sending anything
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
    
    void apply_config(const ryml::NodeRef& config);

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
