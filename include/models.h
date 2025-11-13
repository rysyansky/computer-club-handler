#pragma once

#include <string>
#include <vector>

struct Clock {
    int hours;
    int minutes;
};

inline bool operator<(const Clock &a, const Clock &b) {
    int at, bt;
    at = 60*a.hours + a.minutes;
    bt = 60*b.hours + b.minutes;

    return at < bt;
}
inline bool operator>(const Clock &a, const Clock &b) {
    return b < a;
}
inline bool operator>=(const Clock &a, const Clock &b) {
    return !(a < b);
}
inline bool operator<=(const Clock &a, const Clock &b) {
    return !(b < a);
}

struct Event {
    Clock time;
    int event_id;
    std::string client;
    int pc_number;
};

struct CClubLog {
    int tables_number, hour_cost;
    Clock time_open, time_close;
    std::vector<Event> event_list;
};