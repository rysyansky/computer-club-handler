#pragma once

#include "models.h"

#include <set>
#include <map>
#include <queue>
#include <optional>
#include <algorithm>
#include <sstream>

struct TableInfo {
    int minutes;
    int revenue;
};

struct Session {
    int table;
    Clock start;
};

class CClubHandler {
    

    std::set<std::string> client_in;
    std::vector<std::optional<std::string>> table_occupant;
    //std::map<std::string, int> client_table;
    //std::map<std::string, Clock> session_start;
    std::map<std::string, Session> sessions;
    std::queue<std::string> clients_waiting;
    std::vector<std::string> final_report;
    std::vector<TableInfo> table_hours;

    CClubLog parsed_log;

    

    // Входящие события
    void id1_client_arrival(const Event &e);
    void id2_client_sit(const Event &e);
    void id3_client_wait(const Event &e);
    void id4_client_left(const Event &e);

    // Выходящие события
    void id11_client_left(const std::optional<Event> &oe = std::nullopt);
    void id12_client_sit(const Clock &time, const int &table);
    void id13_gen_error(const Event &e, const std::string &error);

    bool is_working_hours(const Clock &time);
    std::string clock_to_string(const Clock &time);
    std::string event_to_string(const Event &e);
    void update_hours(const std::string &client, const Clock &now);
    Clock minutes_to_clock(const int &mins);
    
public:
    CClubHandler(CClubLog log);

    std::vector<std::string> process();
};