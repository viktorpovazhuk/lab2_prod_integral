// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <filesystem>
#include <string>
#include <fstream>
#include <iostream>
#include "options_parser.h"

namespace po = boost::program_options;

using std::string;

command_line_options_t::command_line_options_t() {
    general_opt.add_options()
            ("help,h",
             "Show help message")
            ("config_file",
             po::value<string>(),
             "File with configuration");

    positional_opt.add("config_file", 1);

    file_opt.add_options()
            ("abs_err", po::value<double>(), "Absolute error")
            ("rel_err", po::value<double>(), "Relative error")
            ("n_threads", po::value<int>(), "Number of threads")
            ("x_start", po::value<double>(), "Start x")
            ("x_end", po::value<double>(), "End x")
            ("y_start", po::value<double>(), "Start y")
            ("y_end", po::value<double>(), "End y")
            ("init_steps_x", po::value<int>(), "Init steps x")
            ("init_steps_y", po::value<int>(), "Init steps y")
            ("max_iter", po::value<int>(), "Max iterations");
}

command_line_options_t::command_line_options_t(int ac, char **av) :
        command_line_options_t() // Delegate constructor
{
    parse(ac, av);
}

void command_line_options_t::parse(int ac, char **av) {
    try {
        po::parsed_options parsed = po::command_line_parser(ac, av)
                .options(general_opt)
                .positional(positional_opt)
                .run();
        po::store(parsed, var_map);

        if (var_map.count("config_file")) {
            std::ifstream ifs{var_map["config_file"].as<std::string>().c_str()};
            if (ifs)
                store(parse_config_file(ifs, file_opt), var_map);
        }
        notify(var_map);

        if (var_map.count("help")) {
            std::cout << general_opt << "\n";
            exit(EXIT_SUCCESS);
        }

        abs_err = var_map["abs_err"].as<double>();
        rel_err = var_map["rel_err"].as<double>();
        n_threads = var_map["n_threads"].as<int>();
        x_start = var_map["x_start"].as<double>();
        x_end = var_map["x_end"].as<double>();
        y_start = var_map["y_start"].as<double>();
        y_end = var_map["y_end"].as<double>();
        init_steps_x = var_map["init_steps_x"].as<int>();
        init_steps_y = var_map["init_steps_y"].as<int>();
        max_iter = var_map["max_iter"].as<int>();


    } catch (std::exception &ex) {
        throw OptionsParseException(ex.what()); // Convert to our error type
    }
}