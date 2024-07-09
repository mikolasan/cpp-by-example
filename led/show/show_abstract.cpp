#include <ryml_std.hpp>

#include "show_abstract.hpp"

void AbstractShow::apply_config(const ryml::NodeRef& config) {
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

    std::string priority = "normal";
    if (config.has_child(ryml::to_csubstr("priority"))) {
        config["priority"] >> priority;
    }
    state.set_priority(priority);

    do_apply_config(config);
}