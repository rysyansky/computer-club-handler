#include "../include/CClubParser.h"


CClubParser::CClubParser(std::string filename) {
    log_file.open(filename);
    if(log_file.is_open()) {
        dead_file = false;
        error_string = "";
        parsed_log.tables_number = -1;
    }
    else {
        dead_file = true;
        error_string = "No such file or directory!";
    }
}

CClubParser::~CClubParser(){
    log_file.close();
}


bool CClubParser::is_digits(const std::string &str) {
    auto found = str.find_first_not_of("0123456789");
    if(found != std::string::npos) return 0;
    return 1;
}

bool CClubParser::is_event_id(const std::string &str) {
    return str.size() == 1 && (str == "1" || str == "2" || str == "3" || str == "4");
}

bool CClubParser::is_client(const std::string &str) {
    if (str.empty()) return false;
    auto found = str.find_first_not_of("abcdefghijklmnopqrstuvwxyz_0123456789-");
    if(found != std::string::npos) return 0;
    return 1;
}

bool CClubParser::validate_table(const std::string &str) {
    if(!is_digits(str)) return 0;
    int cur_number = std::stoi(str);

    if(cur_number < 1 || cur_number > parsed_log.tables_number) return 0;
    return 1;
}

bool CClubParser::validate_clock(const std::string &str) {
    std::string hours, minutes;
    if(str.length() == 5) {
        std::string hours = str.substr(0, 2);
        std::string minutes = str.substr(3, 2);
        if( is_digits(hours) && str[2] == ':' && is_digits(minutes)) {
            if ( (std::stoi(hours) >= 0 && std::stoi(hours) < 24) && 
            (std::stoi(minutes) >= 0 && std::stoi(minutes) < 60)) {
                return 1;
            }
        }
    }
    return 0;
}

std::optional<Clock> CClubParser::string_to_clock(const std::string &str) {
    if(!validate_clock(str)) return std::nullopt;
    return Clock{
        std::stoi(str.substr(0, 2)),
        std::stoi(str.substr(3, 2)),
    };
}

std::vector<std::string> CClubParser::split_by_space (const std::string &line) {
    std::vector<std::string> words;
    std::istringstream iss(line);
    std::string word;

    while(iss >> word){
        words.push_back(word);
    }

    return words;
}

std::optional<Event> CClubParser::parse_event_line(const std::string &line) {
    std::vector<std::string> words = split_by_space(line);
    if(words.size() < 3 || words.size() > 4) {
        error_string = line;
        return std::nullopt;
    }

    auto it = words.begin();
    bool error_handle = string_to_clock(*it).has_value() && is_event_id(*(it+1)) && is_client(*(it+2));
    if(error_handle && words.size() == 4) {
        if(is_digits(*(it+3))) {
            error_handle = validate_table(*(it+3));
        }
        else {
            error_handle = 0;
        }
    }

    if(!error_handle) {
        error_string = line;
        return std::nullopt;
    }

    Event cur = {
        string_to_clock(*it).value(),
        std::stoi(*(it+1)),
        *(it+2),
        -1
    };
    if(words.size() == 4) cur.pc_number = std::stoi(*(it+3));
    else cur.pc_number = -1;
    return cur;
}

bool CClubParser::parse_file() {
    std::string line;
    Event cur_event;
    bool err = false;
    int hour_cost;

    // первая строка - n столов
    std::getline(log_file, line);
    if(!is_digits(line)) {
        error_string = line;
        return 0;
    }
    int n = std::stoi(line);
    if (n <= 0) {
        error_string = line;
        return false;
    }
    parsed_log.tables_number = n;

    // вторая строка - время:начала время:конца
    std::getline(log_file, line);
    std::vector<std::string> words = split_by_space(line);
    std::optional<Clock> first, second;
    if (words.size() != 2) {
        error_string = line;
        return false;
    }
    first = string_to_clock(words[0]);
    second = string_to_clock(words[1]);
    if (!first.has_value() || !second.has_value()) {
        error_string = line;
        return false;
    }
    if (second.value() < first.value()) {
        error_string = line;
        return false;
    }
    parsed_log.time_open = first.value();
    parsed_log.time_close = second.value();

    // третья строка - почасовая стоимость
    std::getline(log_file, line);
    if(!is_digits(line)) {
        error_string = line;
        return 0;
    }
    int cost = std::stoi(line);
    if (cost <= 0) {
        error_string = line;
        return false;
    }
    parsed_log.hour_cost = cost;

    // 4 --- EOF строки - <event:время> <id> <client> ?[<pcnumber>]
    std::optional<Event> test;
    while(std::getline(log_file, line)) {
        if(line.empty()) continue;
        test = parse_event_line(line);
        if(!test.has_value()) {
            error_string = line;
            return 0;
        }
        if (!parsed_log.event_list.empty()) {
            const Event& last_event = parsed_log.event_list.back();
            if (test->time < last_event.time) {
                error_string = line;
                return false;
            }
        }
        parsed_log.event_list.push_back(test.value());
    }



    return 1;
}

std::optional<CClubLog> CClubParser::get_parsed() {
    if(dead_file || !parse_file()) { //сломался вызов файла или нераспарсился файлик
        return std::nullopt;
    }
    return std::optional<CClubLog>(parsed_log);
}

std::string CClubParser::error_msg() {
    return error_string;
}