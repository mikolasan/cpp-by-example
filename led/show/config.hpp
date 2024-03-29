#include <filesystem>
#include <map>
#include <string>
#include <type_traits>

#include <ryml.hpp>
#include <ryml_std.hpp>

#include <show_abstract.hpp>

template <
    typename Show,
    typename = typename std::enable_if<
        std::is_base_of<AbstractShow, Show>::value, Show
    >::type
>
std::map<std::string, Show*> read_show_mapping(std::string config) {
    namespace fs = std::filesystem;

    ryml::Tree tree = ryml::parse_in_arena(ryml::to_csubstr(config));
    std::string show_dir;
    tree["show_dir"] >> show_dir;

    ryml::NodeRef show_mapping = tree["show_mapping"];
    std::map<std::string, Show*> show_buffer;

    std::string bg_show_id = "background_show";
    std::string show_file;
    tree["background_show_file"] >> show_file;
    show_buffer[bg_show_id] = new Show(bg_show_id, (fs::path{show_dir} / show_file).string());
    show_buffer[bg_show_id]->state.set_background(true);
    show_buffer[bg_show_id]->state.set_loop(true);
    show_buffer[bg_show_id]->state.set_ignore_stop(true);

    // cache all animations
    for (ryml::NodeRef const& child : show_mapping.children()) {
        
        std::string show_id;
        ryml::from_chars(child.key(), &show_id);

        // std::cout << child.has_child(ryml::to_csubstr("show_file")) << std::endl;
        child["show_file"] >> show_file;

        fs::path show_path{show_dir};
        show_path /= show_file;
        std::string fname = show_path.string();

        show_buffer[show_id] = new Show(show_id, fname);
        show_buffer[show_id]->apply_config(child);
    }

    return show_buffer;
}