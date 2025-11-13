#pragma once

#include <fstream>
#include <algorithm>
#include <optional>
#include <sstream>

#include "models.h"

class CClubParser {
    std::ifstream log_file;
    std::string error_string;
    bool dead_file;

    CClubLog parsed_log;

    bool is_digits(const std::string &str);
    bool is_event_id(const std::string &str);
    bool is_client(const std::string &str);
    bool validate_table(const std::string &str);
    bool validate_clock(const std::string &str);
    std::optional<Clock> string_to_clock(const std::string &str);
    std::vector<std::string> split_by_space (const std::string &line);
    std::optional<Event> parse_event_line(const std::string &line);
    bool parse_file();
public:
    CClubParser(std::string filename);
    ~CClubParser();

    std::optional<CClubLog> get_parsed();
    std::string error_msg();
};