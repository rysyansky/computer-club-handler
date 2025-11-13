#include <gtest/gtest.h>
#include "CClubParser.h"
#include <sstream>
#include <fstream>
#include <string>

std::string create_temp_file(const std::vector<std::string>& lines) {
    static int counter = 0;
    std::string filename = "/tmp/test_input_" + std::to_string(counter++) + ".txt";
    std::ofstream file(filename);
    for (const auto& line : lines) {
        file << line << "\n";
    }
    return filename;
}

TEST(ParserTest, ValidInput) {
    std::vector<std::string> input_lines = {
        "2",
        "09:00 21:00",
        "15",
        "09:30 1 client_a",
        "09:35 2 client_a 1"
    };
    std::string filename = create_temp_file(input_lines);

    CClubParser parser(filename);
    auto result = parser.get_parsed();

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->tables_number, 2);
    EXPECT_EQ(result->hour_cost, 15);
    EXPECT_EQ(result->time_open.hours, 9);
    EXPECT_EQ(result->time_open.minutes, 0);
    EXPECT_EQ(result->time_close.hours, 21);
    EXPECT_EQ(result->time_close.minutes, 0);
    EXPECT_EQ(result->event_list.size(), 2);
    EXPECT_EQ(result->event_list[0].time.hours, 9);
    EXPECT_EQ(result->event_list[0].time.minutes, 30);
    EXPECT_EQ(result->event_list[0].event_id, 1);
    EXPECT_EQ(result->event_list[0].client, "client_a");
    EXPECT_EQ(result->event_list[1].pc_number, 1);
}

TEST(ParserTest, InvalidNumberOfTables) {
    std::vector<std::string> input_lines = {
        "0",
        "09:00 21:00",
        "15"
    };
    std::string filename = create_temp_file(input_lines);

    CClubParser parser(filename);
    auto result = parser.get_parsed();

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(parser.error_msg(), "0");
}

TEST(ParserTest, InvalidTimeFormat) {
    std::vector<std::string> input_lines = {
        "2",
        "09:00 25:00",
        "15"
    };
    std::string filename = create_temp_file(input_lines);

    CClubParser parser(filename);
    auto result = parser.get_parsed();

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(parser.error_msg(), "09:00 25:00");
}

TEST(ParserTest, InvalidTimeOrder) {
    std::vector<std::string> input_lines = {
        "2",
        "21:00 09:00",
        "15"
    };
    std::string filename = create_temp_file(input_lines);

    CClubParser parser(filename);
    auto result = parser.get_parsed();

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(parser.error_msg(), "21:00 09:00");
}

TEST(ParserTest, InvalidHourCost) {
    std::vector<std::string> input_lines = {
        "2",
        "09:00 21:00",
        "0"
    };
    std::string filename = create_temp_file(input_lines);

    CClubParser parser(filename);
    auto result = parser.get_parsed();

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(parser.error_msg(), "0");
}

TEST(ParserTest, InvalidEventTime) {
    std::vector<std::string> input_lines = {
        "2",
        "09:00 21:00",
        "15",
        "09:30 1 client_a",
        "09:25 2 client_a 1"
    };
    std::string filename = create_temp_file(input_lines);

    CClubParser parser(filename);
    auto result = parser.get_parsed();

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(parser.error_msg(), "09:25 2 client_a 1");
}

TEST(ParserTest, InvalidClientId) {
    std::vector<std::string> input_lines = {
        "2",
        "09:00 21:00",
        "15",
        "09:30 1 client@A"
    };
    std::string filename = create_temp_file(input_lines);

    CClubParser parser(filename);
    auto result = parser.get_parsed();

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(parser.error_msg(), "09:30 1 client@A");
}

TEST(ParserTest, InvalidEventId) {
    std::vector<std::string> input_lines = {
        "2",
        "09:00 21:00",
        "15",
        "09:30 5 client_a"
    };
    std::string filename = create_temp_file(input_lines);

    CClubParser parser(filename);
    auto result = parser.get_parsed();

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(parser.error_msg(), "09:30 5 client_a");
}

TEST(ParserTest, InvalidTableNumber) {
    std::vector<std::string> input_lines = {
        "2",
        "09:00 21:00",
        "15",
        "09:30 2 client_a 3"
    };
    std::string filename = create_temp_file(input_lines);

    CClubParser parser(filename);
    auto result = parser.get_parsed();

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(parser.error_msg(), "09:30 2 client_a 3");
}

TEST(ParserTest, FileNotFound) {
    CClubParser parser("/non/existent/file.txt");
    auto result = parser.get_parsed();

    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(parser.error_msg(), "No such file or directory!");
}
