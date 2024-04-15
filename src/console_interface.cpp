#include "../utils/console_interface.h"

void console_interface::Help() {
    std::cout << "Wrong programm call arguments. You should follow the instruction:" << std::endl;
    std::cout << "{executable file name} {input image path} {output image path} [-{filter alias 1} [filter parameter "
                 "1] [filter parameter 2] ...] [-{filter alias 2} [filter parameter 1] [filter parameter 2] ...] ..."
              << std::endl;
}

size_t console_interface::ParseArguments(char** arguments, size_t start, size_t size, std::string& current_filter,
                                         std::queue<std::string>& parameters) {
    for (size_t i = start; i != size; ++i) {
        if (arguments[i][0] == '-' && current_filter.empty()) {
            current_filter = arguments[i];
        } else if (arguments[i][0] == '-' && !current_filter.empty()) {
            return i;
        } else if (arguments[i][0] != '-' && current_filter.empty()) {
            throw InvalidArgumentsError{};
        } else {
            parameters.push(arguments[i]);
        }
    }

    return size;
}
