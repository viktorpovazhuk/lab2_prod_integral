//
// Created by vityha on 23.02.22.
//

#include <memory>
#include <iostream>
#include "../src/options_parser.h"

using std::cout;
using std::endl;

int main(int argc, char *argv[]) {
    std::unique_ptr<command_line_options_t> command_line_options;
    try {
        command_line_options = std::make_unique<command_line_options_t>(argc, argv);
    }
    catch (std::exception &ex) {
        cout << ex.what() << endl;
        return 1;
    }

    cout << command_line_options->abs_err << '\n' << command_line_options->max_iter << endl;

    return 0;
}