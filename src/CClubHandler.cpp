#include "../include/CClubHandler.h"

CClubHandler::CClubHandler(CClubLog log) : parsed_log(log), table_occupant(log.tables_number), table_hours(log.tables_number, {0,0}) {}



void CClubHandler::id1_client_arrival(const Event &e) {
    if(client_in.count(e.client)) {
        id13_gen_error(e, "YouShallNotPass");
        return;
    }
    if(!is_working_hours(e.time)) {
        id13_gen_error(e, "NotOpenYet");
        return;
    }
    client_in.insert(e.client);
}

void CClubHandler::id2_client_sit(const Event &e) {
    if (!client_in.count(e.client)) {
        id13_gen_error(e, "ClientUnknown");
        return;
    }
    if (table_occupant[e.pc_number-1].has_value()) {
        id13_gen_error(e, "PlaceIsBusy");
        return;
    }
    if (sessions.count(e.client)) {
        int old_table = sessions[e.client].table;
        update_hours(e.client, e.time);
        table_occupant[old_table - 1] = std::nullopt;
        sessions.erase(e.client);
    }
    sessions[e.client].table = e.pc_number;
    table_occupant[e.pc_number-1] = e.client;
    sessions[e.client].start = e.time;
}

void CClubHandler::id3_client_wait(const Event &e) {
    bool free_table = false;
    for(auto occupant : table_occupant) {
        if(!occupant.has_value()){
            free_table = true;
            break;
        }
    }
    if (free_table) {
        id13_gen_error(e, "ICanWaitNoLonger!");
        return;
    }
    if(clients_waiting.size() >= parsed_log.tables_number) {
        id11_client_left(e);
        return;
    }
    clients_waiting.push(e.client);
}

void CClubHandler::id4_client_left(const Event &e) {
    if (!client_in.count(e.client)) {
        id13_gen_error(e, "ClientUnknown");
        return;
    }
    if (sessions.count(e.client)) {
        int table = sessions[e.client].table;
        update_hours(e.client, e.time);
        table_occupant[table - 1] = std::nullopt;
        sessions.erase(e.client);
        client_in.erase(e.client);

        id12_client_sit(e.time, table);
    } else {
        client_in.erase(e.client);
    }
}

void CClubHandler::id11_client_left(const std::optional<Event> &oe) {
    if (oe.has_value()) {
        const Event &e = oe.value();
        client_in.erase(e.client);
        sessions.erase(e.client);
        final_report.push_back(clock_to_string(e.time) + " 11 " + e.client);
        return;
    }
    
    for (const std::string& client : client_in) {
        if (sessions.count(client)) {
            update_hours(client, parsed_log.time_close);
            int table = sessions[client].table;
            table_occupant[table - 1] = std::nullopt;
        }
        final_report.push_back(clock_to_string(parsed_log.time_close) + " 11 " + client);
        sessions.erase(client);
    }
    client_in.clear();
}

void CClubHandler::id12_client_sit(const Clock &time, const int &table) {
    if (clients_waiting.empty()) return;
    std::string client = clients_waiting.front();
    clients_waiting.pop();
    table_occupant[table - 1] = client;
    sessions[client] = {table, time};

    final_report.push_back(clock_to_string(time) + " 12 " + client + " " + std::to_string(table));
}

void CClubHandler::id13_gen_error(const Event &e, const std::string &error) {
    final_report.push_back(clock_to_string(e.time) + " 13 " + error);
}

bool CClubHandler::is_working_hours(const Clock &time) {
    return parsed_log.time_open <= time && time <= parsed_log.time_close;
}

std::string CClubHandler::clock_to_string(const Clock &time) {
    std::string hh = std::to_string(time.hours), mm = std::to_string(time.minutes);

    if(hh.length() < 2) hh = "0" + hh;
    if(mm.length() < 2) mm = "0" + mm;

    return hh + ":" + mm;
}

std::string CClubHandler::event_to_string(const Event &e) {
    std::string ev = clock_to_string(e.time) + " " + std::to_string(e.event_id) + " " + e.client;
    if (e.event_id == 2) ev = ev + " " + std::to_string(e.pc_number);

    return ev;
}

void CClubHandler::update_hours(const std::string &client, const Clock &now) {
    if(!sessions.count(client))return;
    Clock start = sessions[client].start;
    int tstart, tnow, dif;
    tstart = start.hours*60 + start.minutes;
    tnow = now.hours*60 + now.minutes;
    dif = tnow - tstart;
    if(!dif) return;
    table_hours[sessions[client].table - 1].minutes += dif;
    if(dif % 60) dif = (dif / 60) + 1;
    else dif = dif / 60;
    table_hours[sessions[client].table - 1].revenue += dif*parsed_log.hour_cost;
}

Clock CClubHandler::minutes_to_clock(const int &mins) {
    return {mins/60, mins%60};
}

std::vector<std::string> CClubHandler::process() {
    final_report.push_back(clock_to_string(parsed_log.time_open));
    for(Event e : parsed_log.event_list) {
        final_report.push_back(event_to_string(e));
        switch(e.event_id) {
            case(1): id1_client_arrival(e); break;
            case(2): id2_client_sit(e); break;
            case(3): id3_client_wait(e); break;
            case(4): id4_client_left(e); break;
        }
    }
    final_report.push_back(clock_to_string(parsed_log.time_close));
    id11_client_left();
    for(int table = 0; table < table_hours.size(); table++) {
        final_report.push_back(
            std::to_string(table + 1) + " " + 
            std::to_string(table_hours[table].revenue) + " " + 
            clock_to_string(minutes_to_clock(table_hours[table].minutes))
        );
    }

    return final_report;
}