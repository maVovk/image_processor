#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include "utils/exceptions.h"
#include "utils/console_interface.h"

TEST_CASE("console_interface::ParseArguments test") {  // NOLINT
    char** argv = new char*[8];                        // NOLINT

    for (size_t i = 0; i < 8; ++i) {  // NOLINT
        argv[i] = new char[12];       // NOLINT
    }

    strcpy(argv[0], "-gs");           // NOLINT
    strcpy(argv[1], "-crop");         // NOLINT
    strcpy(argv[2], "100");           // NOLINT
    strcpy(argv[3], "200.5");         // NOLINT
    strcpy(argv[4], "-blur");         // NOLINT
    strcpy(argv[5], "0.7");           // NOLINT
    strcpy(argv[6], "abacaba");       // NOLINT
    strcpy(argv[7], "cringe_param");  // NOLINT

    size_t start = 0;
    std::string filter_alias;
    std::queue<std::string> parameters;

    start = console_interface::ParseArguments(argv, start, 8, filter_alias, parameters);  // NOLINT

    REQUIRE("-gs" == filter_alias);
    REQUIRE(parameters.empty());
    filter_alias.clear();

    start = console_interface::ParseArguments(argv, start, 8, filter_alias, parameters);  // NOLINT

    REQUIRE("-crop" == filter_alias);
    REQUIRE(2 == parameters.size());
    REQUIRE("100" == parameters.front());
    parameters.pop();
    REQUIRE("200.5" == parameters.front());
    parameters.pop();
    filter_alias.clear();

    start = console_interface::ParseArguments(argv, start, 8, filter_alias, parameters);  // NOLINT

    REQUIRE("-blur" == filter_alias);
    REQUIRE(3 == parameters.size());
    REQUIRE("0.7" == parameters.front());
    parameters.pop();
    REQUIRE("abacaba" == parameters.front());
    parameters.pop();
    REQUIRE("cringe_param" == parameters.front());
}
