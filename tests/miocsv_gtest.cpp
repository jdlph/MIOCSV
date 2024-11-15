#include <stdcsv.h>
#include <miocsv.h>

#include <gtest/gtest.h>

#include <iostream>
#include <string>
#include <vector>

/**
 * @brief TO DO : test writer
 */

namespace
{
const std::vector<std::string> PARSED_HEADERS {
    "name",
    "link_id",
    "from_node_id",
    "to_node_id",
    "facility_type",
    "dir_flag",
    "length",
    "lanes",
    "capacity",
    "free_speed",
    "link_type",
    "cost",
    "VDF_fftt1",
    "VDF_cap1",
    "VDF_alpha1",
    "VDF_beta1",
    "VDF_theta1",
    "VDF_gamma1",
    "VDF_mu1",
    "RUC_rho1",
    "RUC_resource1",
    "RUC_type"
};

const std::vector<std::string> PARSED_ROW1 {
    "",
    "1",
    "1",
    "547",
    "Highway",
    "1",
    "0.86267",
    "1",
    "49500",
    "60",
    "1",
    "0",
    "0.86267",
    "49500",
    "0.15",
    "4",
    "1",
    "1",
    "100",
    "10",
    "0",
    "1"
};

const std::vector<std::string> PARSED_ROW2 {
    "",
    "1024",
    "554",
    "437",
    "Highway",
    "1",
    "1.037",
    "1",
    "4000",
    "60",
    "1",
    "0",
    "1.037",
    "4000",
    "0.15",
    "4",
    "1",
    "1",
    "100",
    "10",
    "0",
    "1"
};

const std::vector<std::string> PARSED_ROW3 {
    "",
    "2950",
    "933",
    "534",
    "Highway",
    "1",
    "6.10762",
    "1",
    "3500",
    "60",
    "1",
    "0",
    "6.10762",
    "3500",
    "0.15",
    "4",
    "1",
    "1",
    "100",
    "10",
    "0",
    "1"
};

void compare(const miocsv::Row& parsed_line, const std::vector<std::string>& expected_line)
{
    ASSERT_EQ(parsed_line.size(), expected_line.size());
    for (miocsv::size_type i = 0, sz = parsed_line.size(); i != sz; ++i)
    {
        EXPECT_EQ(parsed_line[i], expected_line[i]);
    }

    ASSERT_THROW(parsed_line[-1], miocsv::NoRecord);
    ASSERT_THROW(parsed_line[parsed_line.size()], miocsv::NoRecord);
}

void compare(const miocsv::Row& parsed_line)
{
    EXPECT_EQ(parsed_line[0], parsed_line["name"]);
    EXPECT_EQ(parsed_line[1], parsed_line["link_id"]);
    EXPECT_EQ(parsed_line[2], parsed_line["from_node_id"]);
    EXPECT_EQ(parsed_line[3], parsed_line["to_node_id"]);
    EXPECT_EQ(parsed_line[4], parsed_line["facility_type"]);
    EXPECT_EQ(parsed_line[5], parsed_line["dir_flag"]);
    EXPECT_EQ(parsed_line[6], parsed_line["length"]);
    EXPECT_EQ(parsed_line[7], parsed_line["lanes"]);
    EXPECT_EQ(parsed_line[8], parsed_line["capacity"]);
    EXPECT_EQ(parsed_line[9], parsed_line["free_speed"]);
    EXPECT_EQ(parsed_line[10], parsed_line["link_type"]);
    EXPECT_EQ(parsed_line[11], parsed_line["cost"]);
    EXPECT_EQ(parsed_line[12], parsed_line["VDF_fftt1"]);
    EXPECT_EQ(parsed_line[13], parsed_line["VDF_cap1"]);
    EXPECT_EQ(parsed_line[14], parsed_line["VDF_alpha1"]);
    EXPECT_EQ(parsed_line[15], parsed_line["VDF_beta1"]);
    EXPECT_EQ(parsed_line[16], parsed_line["VDF_theta1"]);
    EXPECT_EQ(parsed_line[17], parsed_line["VDF_gamma1"]);
    EXPECT_EQ(parsed_line[18], parsed_line["VDF_mu1"]);
    EXPECT_EQ(parsed_line[19], parsed_line["RUC_rho1"]);
    EXPECT_EQ(parsed_line[20], parsed_line["RUC_resource1"]);
    EXPECT_EQ(parsed_line[21], parsed_line["RUC_type"]);
}

void validate_parsed_content(miocsv::BaseReader* p)
{
    auto& reader = *p;
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        ASSERT_LE(row_num, 2951);

        if (row_num == 1)
            compare(line, PARSED_HEADERS);

        if (row_num == 2)
            compare(line, PARSED_ROW1);

        if (row_num == 1025)
            compare(line, PARSED_ROW2);

        if (row_num == 2951)
            compare(line, PARSED_ROW3);
    }
}

void validate_parsed_content(miocsv::BaseDictReader* p)
{
    static const std::string RAW_HEADERS =
        "name,link_id,from_node_id,to_node_id,facility_type,dir_flag,length,lanes,capacity,free_"
        "speed,link_type,cost,VDF_fftt1,VDF_cap1,VDF_alpha1,VDF_beta1,VDF_theta1,VDF_gamma1,VDF_"
        "mu1,RUC_rho1,RUC_resource1,RUC_type";

    auto& reader = *p;
    std::stringstream ss;

    ss << reader.get_fieldnames();
    ASSERT_EQ(ss.str(), RAW_HEADERS);

    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        ASSERT_LE(row_num, 2951);

        if (row_num == 1)
        {
            compare(line, PARSED_ROW1);
            compare(line);
        }

        if (row_num == 1025)
        {
            compare(line, PARSED_ROW2);
            compare(line);
        }

        if (row_num == 2951)
        {
            compare(line, PARSED_ROW3);
            compare(line);
        }
    }
}

void validate_Reader(const std::string& filename)
{
    auto reader = miocsv::Reader {filename};
    validate_parsed_content(&reader);
}

void validate_DictReader(const std::string& filename)
{
    auto reader = miocsv::DictReader {filename};
    validate_parsed_content(&reader);
}

void validate_MIOReader(const std::string& filename)
{
    auto reader = miocsv::MIOReader {filename};
    validate_parsed_content(&reader);
}

void validate_MIODictReader(const std::string& filename)
{
    auto reader = miocsv::MIODictReader {filename};
    validate_parsed_content(&reader);
}

void validate_all_readers(const std::string& filename)
{
    validate_Reader(filename);
    validate_DictReader(filename);
    validate_MIOReader(filename);
    validate_MIODictReader(filename);
}

void parse_through_Reader(const std::string& filename)
{
    auto reader = miocsv::Reader {filename};
    for (const auto& line : reader)
    {
        // do nothing
    }
}

void parse_through_DictReader(const std::string& filename)
{
    auto reader = miocsv::DictReader {filename};
    for (const auto& line : reader)
    {
        // do nothing
    }
}

void parse_through_MIOReader(const std::string& filename)
{
    auto reader = miocsv::MIOReader {filename};
    for (const auto& line : reader)
    {
        // do nothing
    }
}

void parse_through_MIODictReader(const std::string& filename)
{
    auto reader = miocsv::MIODictReader {filename};
    for (const auto& line : reader)
    {
        // do nothing
    }
}

bool sniff_cr(const std::string& filename)
{
    static constexpr char CR = '\r';

    std::ifstream ist {filename};
    if (!ist)
    {
        std::cerr << "invalid input! no " << filename << '\n';
        std::terminate();
    }

    std::string s;
    if (!std::getline(ist, s))
    {
        std::cerr << "EOF has reached for" << filename << '\n';
        std::terminate();
    }
    else if (s.empty())
        return false;

    return s.back() == CR;
}

struct EOLCase {
    std::string filename;
};

class EOLTest : public testing::TestWithParam<EOLCase> {

};

// TEST_FILE (test.csv) is '\n' terminated while TEST_FILE_CRLF(test_CRLF) is '\r\n' terminated
INSTANTIATE_TEST_SUITE_P(
    MIOCSVTest,
    EOLTest,
    testing::Values(EOLCase{TEST_FILE}, EOLCase{TEST_FILE_CRLF})
);

TEST_P(EOLTest, AllReadersRunSeamlessly)
{
    EOLCase ec = GetParam();
    validate_all_readers(ec.filename);
}

TEST(MIOCSVTest, SniffEOL)
{
    ASSERT_FALSE(sniff_cr(TEST_FILE));
    ASSERT_TRUE(sniff_cr(TEST_FILE_CRLF));
}

TEST(MIOCSVTest, FileNotFound)
{
    // there is no such mock.csv file
    std::string filename {"mock.csv"};

    ASSERT_DEATH(miocsv::Reader{filename}, "invalid input! no mock.csv");
    ASSERT_DEATH(miocsv::DictReader{filename}, "invalid input! no mock.csv");
    ASSERT_THROW(miocsv::MIOReader{filename}, std::system_error);
    ASSERT_THROW(miocsv::MIODictReader{filename}, std::system_error);
}

TEST(MIOCSVTest, ParseThroughIllFormedFile)
{
    ASSERT_NO_THROW(parse_through_Reader(ILLFORMED_FILE));
    ASSERT_NO_THROW(parse_through_DictReader(ILLFORMED_FILE));
    ASSERT_NO_THROW(parse_through_MIOReader(ILLFORMED_FILE));
    ASSERT_NO_THROW(parse_through_MIODictReader(ILLFORMED_FILE));
}

} // namespace
