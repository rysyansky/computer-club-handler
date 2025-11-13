#include <gtest/gtest.h>
#include "CClubHandler.h"
#include "models.h"
#include <vector>
#include <string>

CClubLog create_test_log(int tables, int cost, int open_h, int open_m, int close_h, int close_m) {
    CClubLog log;
    log.tables_number = tables;
    log.hour_cost = cost;
    log.time_open = {open_h, open_m};
    log.time_close = {close_h, close_m};
    return log;
}

TEST(HandlerTest, InSitOut) {
    CClubLog log = create_test_log(2, 10, 9, 0, 21, 0);

    log.event_list.push_back({{9, 15}, 1, "client", -1});
    log.event_list.push_back({{9, 20}, 2, "client", 1});
    log.event_list.push_back({{9, 45}, 4, "client", -1});

    CClubHandler handler(log);
    std::vector<std::string> report = handler.process();

    ASSERT_GE(report.size(), 6);

    EXPECT_EQ(report[0], "09:00");
    EXPECT_EQ(report[1], "09:15 1 client");
    EXPECT_EQ(report[2], "09:20 2 client 1");
    EXPECT_EQ(report[3], "09:45 4 client");
    EXPECT_EQ(report[4], "21:00");

    bool id11 = false;
    for (auto str : report) {
        if (str.find("21:00 11 client1") == 0) {
            id11 = true;
            break;
        }
    }
    EXPECT_FALSE(id11);

    bool table1_report = false;
    for (auto str : report) {
        if (str.find("1 10 00:25") == 0) {
            table1_report = true;
            break;
        }
    }
    EXPECT_TRUE(table1_report);

    bool table2_report = false;
    for (auto str : report) {
        if (str.find("2 0 00:00") == 0) {
            table2_report = true;
            break;
        }
    }
    EXPECT_TRUE(table2_report);
}

TEST(HandlerTest, YouShallNotPass) {
    CClubLog log = create_test_log(2, 10, 9, 0, 21, 0);
    log.event_list.push_back({{9, 15}, 1, "client1", -1});
    log.event_list.push_back({{9, 16}, 1, "client1", -1});

    CClubHandler handler(log);
    std::vector<std::string> report = handler.process();

    ASSERT_GE(report.size(), 4);
    EXPECT_EQ(report[0], "09:00");
    EXPECT_EQ(report[1], "09:15 1 client1");
    EXPECT_EQ(report[2], "09:16 1 client1");
    EXPECT_EQ(report[3], "09:16 13 YouShallNotPass");
    EXPECT_EQ(report[4], "21:00");
}

TEST(HandlerTest, NotOpenYet) {
    CClubLog log = create_test_log(2, 10, 9, 0, 21, 0);
    log.event_list.push_back({{8, 21}, 1, "client1", -1});

    CClubHandler handler(log);
    std::vector<std::string> report = handler.process();

    ASSERT_GE(report.size(), 3);
    EXPECT_EQ(report[0], "09:00");
    EXPECT_EQ(report[1], "08:21 1 client1");
    EXPECT_EQ(report[2], "08:21 13 NotOpenYet");
    EXPECT_EQ(report[3], "21:00");
}

TEST(HandlerTest, PlaceIsBusy) {
    CClubLog log = create_test_log(1, 10, 9, 0, 21, 0);
    log.event_list.push_back({{9, 15}, 1, "client1", -1});
    log.event_list.push_back({{9, 16}, 2, "client1", 1});
    log.event_list.push_back({{9, 20}, 1, "client2", -1});
    log.event_list.push_back({{9, 21}, 2, "client2", 1});

    CClubHandler handler(log);
    std::vector<std::string> report = handler.process();

    ASSERT_GE(report.size(), 6);
    EXPECT_EQ(report[0], "09:00");
    EXPECT_EQ(report[1], "09:15 1 client1");
    EXPECT_EQ(report[2], "09:16 2 client1 1");
    EXPECT_EQ(report[3], "09:20 1 client2");
    EXPECT_EQ(report[4], "09:21 2 client2 1");
    EXPECT_EQ(report[5], "09:21 13 PlaceIsBusy");
    EXPECT_EQ(report[6], "21:00");
}

TEST(HandlerTest, ClientUnknown) {
    CClubLog log = create_test_log(2, 10, 9, 0, 21, 0);
    log.event_list.push_back({{9, 15}, 2, "somebodythatiusedtoknow", 1});

    CClubHandler handler(log);
    std::vector<std::string> report = handler.process();

    ASSERT_GE(report.size(), 3);
    EXPECT_EQ(report[0], "09:00");
    EXPECT_EQ(report[1], "09:15 2 somebodythatiusedtoknow 1");
    EXPECT_EQ(report[2], "09:15 13 ClientUnknown");
    EXPECT_EQ(report[3], "21:00");
}

TEST(HandlerTest, QueueTestSit) {
    CClubLog log = create_test_log(1, 10, 9, 0, 21, 0);
    log.event_list.push_back({{9, 15}, 1, "client1", -1});
    log.event_list.push_back({{9, 16}, 2, "client1", 1});
    log.event_list.push_back({{9, 20}, 1, "waiting", -1});
    log.event_list.push_back({{9, 21}, 3, "waiting"});
    log.event_list.push_back({{9, 25}, 4, "client1"});

    CClubHandler handler(log);
    std::vector<std::string> report = handler.process();

    bool id12 = false;
    for (auto line : report) {
        if (line.find("09:25 12 waiting 1") == 0) {
            id12 = true;
            break;
        }
    }
    EXPECT_TRUE(id12);
}

TEST(HandlerTest, QueueOverflow) {
    CClubLog log = create_test_log(1, 10, 9, 0, 21, 0);
    log.event_list.push_back({{9, 15}, 1, "client1", -1});
    log.event_list.push_back({{9, 16}, 2, "client1", 1});
    log.event_list.push_back({{9, 20}, 1, "client2", -1});
    log.event_list.push_back({{9, 21}, 3, "client2"});
    log.event_list.push_back({{9, 22}, 1, "3rdwheel", -1});
    log.event_list.push_back({{9, 23}, 3, "3rdwheel"});

    CClubHandler handler(log);
    std::vector<std::string> report = handler.process();

    bool id11 = false;
    for (auto line : report) {
        if (line.find("09:23 11 3rdwheel") == 0) {
            id11 = true;
            break;
        }
    }
    EXPECT_TRUE(id11);
}

TEST(HandlerTest, EndOfDayId11AlphabeticalOrder) {
    CClubLog log = create_test_log(2, 10, 9, 0, 21, 0);
    log.event_list.push_back({{9, 15}, 1, "sorrow", -1});
    log.event_list.push_back({{9, 16}, 1, "midnight", -1});

    CClubHandler handler(log);
    std::vector<std::string> report = handler.process();

    std::vector<std::string> id11_lines;
    for (auto str : report) {
        if (str.find("21:00 11") == 0) {
            id11_lines.push_back(str);
        }
    }

    ASSERT_GE(id11_lines.size(), 2);
    EXPECT_EQ(id11_lines[0], "21:00 11 midnight");
    EXPECT_EQ(id11_lines[1], "21:00 11 sorrow");
}
