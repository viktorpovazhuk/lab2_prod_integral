#ifndef OPTIONS_PARSER_CONFIG_FILE_H
#define OPTIONS_PARSER_CONFIG_FILE_H

#include <boost/program_options.hpp>
#include <string>
#include <exception>
#include <stdexcept>

class OptionsParseException : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class command_line_options_t {
public:
    command_line_options_t();

    command_line_options_t(int ac, char **av);

    //! Explicit is better than implicit:
    command_line_options_t(const command_line_options_t &) = default;

    command_line_options_t &operator=(const command_line_options_t &) = delete;

    command_line_options_t(command_line_options_t &&) = default;

    command_line_options_t &operator=(command_line_options_t &&) = delete;

    ~command_line_options_t() = default;

    void parse(int ac, char **av);

    double abs_err = 0, rel_err = 0;
    int n_threads = 0;
    double x_start = 0, x_end = 0, y_start = 0, y_end = 0;
    int init_steps_x = 0, init_steps_y = 0, max_iter = 0;

private:
    boost::program_options::variables_map var_map{};
    boost::program_options::options_description general_opt{};
    boost::program_options::options_description file_opt{};
    boost::program_options::positional_options_description positional_opt{};
};

#endif //OPTIONS_PARSER_CONFIG_FILE_H

