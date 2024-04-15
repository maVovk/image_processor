#pragma once

#include <ios>
#include <string>
#include <stdexcept>

class FileNotFoundError : public std::ios_base::failure {
public:
    explicit FileNotFoundError() : std::ios_base::failure("File not found error") {
    }
};

class FileCreationError : public std::ios_base::failure {
public:
    explicit FileCreationError() : std::ios_base::failure("File couldn't be created") {
    }
};

class UnsupportedFileFormat : public std::logic_error {
public:
    explicit UnsupportedFileFormat(const std::string& filename)
        : std::logic_error(filename + " file format support is not implemented yet") {
    }
};

class NotImplementedError : public std::logic_error {
public:
    explicit NotImplementedError() : std::logic_error("Function/method is not implemented yet") {
    }
};

class InvalidArgumentsError : public std::runtime_error {
public:
    explicit InvalidArgumentsError() : std::runtime_error("Invalid programm arguments") {
    }
};

class InvalidFilterParametersError : public std::runtime_error {
public:
    explicit InvalidFilterParametersError(const std::string& filter_name)
        : std::runtime_error("Invalid parameters for " + filter_name + " filter") {
    }
};
