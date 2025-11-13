#include <iostream>
#include <string>
#include <vector>
#include <optional>

#include "../include/CClubParser.h"
#include "../include/CClubHandler.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];

    CClubParser parser(input_filename);

    std::optional<CClubLog> parsed_log = parser.get_parsed();

    if (!parsed_log.has_value()) {
        std::string error_msg = parser.error_msg();
        std::cout << error_msg << std::endl;
        return 1;
    }

    CClubHandler handler(parsed_log.value());
    std::vector<std::string> results = handler.process();

    for (const std::string& line : results) {
        std::cout << line << std::endl;
    }

    return 0;
}