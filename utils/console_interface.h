#pragma once

#include "bmp_reader.h"
#include "exceptions.h"
#include "filters.h"

#include <iostream>
#include <queue>

namespace console_interface {
void Help();

size_t ParseArguments(char** arguments, size_t start, size_t size, std::string& current_filter,
                      std::queue<std::string>& parameters);

template <typename T>
void Clear(T& container) {
    while (!container.empty()) {
        container.pop();
    }
}
}  // namespace console_interface