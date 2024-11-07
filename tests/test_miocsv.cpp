#include <gtest/gtest.h>

#include <stdcsv.h>
#include <miocsv.h>

#include <string>

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

TEST(MIOCSV, Reader) 
{
    auto reader = miocsv::Reader {INPUT_FILE};
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        ASSERT_LE(row_num, 2951);

        if (row_num == 0) 
        {
            EXPECT_EQ(line[0], "name");
            EXPECT_EQ(line[1], "link_id");
            EXPECT_EQ(line[2], "from_node_id");
            EXPECT_EQ(line[3], "to_node_id");
            EXPECT_EQ(line[4], "facility_type");
            EXPECT_EQ(line[5], "dir_flag");
            EXPECT_EQ(line[6], "length");
            EXPECT_EQ(line[7], "lanes");
            EXPECT_EQ(line[8], "capacity");
            EXPECT_EQ(line[9], "free_speed");
            EXPECT_EQ(line[10], "link_type");
            EXPECT_EQ(line[11], "cost");
            EXPECT_EQ(line[12], "VDF_fftt1");
            EXPECT_EQ(line[13], "VDF_cap1");
            EXPECT_EQ(line[14], "VDF_alpha1");
            EXPECT_EQ(line[15], "VDF_beta1");
            EXPECT_EQ(line[16], "VDF_theta1");
            EXPECT_EQ(line[17], "VDF_gamma1");
            EXPECT_EQ(line[18], "VDF_mu1");
            EXPECT_EQ(line[19], "RUC_rho1");
            EXPECT_EQ(line[20], "RUC_resource1");
            EXPECT_EQ(line[21], "RUC_type");

            ASSERT_THROW(line[22], miocsv::NoRecord);
        }

        if (row_num == 1)
        {
            EXPECT_EQ(line[0], "");
            EXPECT_EQ(line[1], "1");
            EXPECT_EQ(line[2], "1");
            EXPECT_EQ(line[3], "547");
            EXPECT_EQ(line[4], "Highway");
            EXPECT_EQ(line[5], "1");
            EXPECT_EQ(line[6], "0.86267");
            EXPECT_EQ(line[7], "1");
            EXPECT_EQ(line[8], "49500");
            EXPECT_EQ(line[9], "60");
            EXPECT_EQ(line[10], "1");
            EXPECT_EQ(line[11], "0");
            EXPECT_EQ(line[12], "0.86267");
            EXPECT_EQ(line[13], "49500");
            EXPECT_EQ(line[14], "0.15");
            EXPECT_EQ(line[15], "4");
            EXPECT_EQ(line[16], "1");
            EXPECT_EQ(line[17], "1");
            EXPECT_EQ(line[18], "100");
            EXPECT_EQ(line[19], "10");
            EXPECT_EQ(line[20], "0");
            EXPECT_EQ(line[21], "1");

            ASSERT_THROW(line[22], miocsv::NoRecord);
        }

        if (row_num == 1024)
        {
            EXPECT_EQ(line[0], "");
            EXPECT_EQ(line[1], "1024");
            EXPECT_EQ(line[2], "554");
            EXPECT_EQ(line[3], "437");
            EXPECT_EQ(line[4], "Highway");
            EXPECT_EQ(line[5], "1");
            EXPECT_EQ(line[6], "1.037");
            EXPECT_EQ(line[7], "1");
            EXPECT_EQ(line[8], "4000");
            EXPECT_EQ(line[9], "60");
            EXPECT_EQ(line[10], "1");
            EXPECT_EQ(line[11], "0");
            EXPECT_EQ(line[12], "1.037");
            EXPECT_EQ(line[13], "4000");
            EXPECT_EQ(line[14], "0.15");
            EXPECT_EQ(line[15], "4");
            EXPECT_EQ(line[16], "1");
            EXPECT_EQ(line[17], "1");
            EXPECT_EQ(line[18], "100");
            EXPECT_EQ(line[19], "10");
            EXPECT_EQ(line[20], "0");
            EXPECT_EQ(line[21], "1");

            ASSERT_THROW(line[22], miocsv::NoRecord);
        }

        if (row_num == 1024)
        {
            EXPECT_EQ(line[0], "");
            EXPECT_EQ(line[1], "2950");
            EXPECT_EQ(line[2], "933");
            EXPECT_EQ(line[3], "534");
            EXPECT_EQ(line[4], "Highway");
            EXPECT_EQ(line[5], "1");
            EXPECT_EQ(line[6], "6.10762");
            EXPECT_EQ(line[7], "1");
            EXPECT_EQ(line[8], "3500");
            EXPECT_EQ(line[9], "60");
            EXPECT_EQ(line[10], "1");
            EXPECT_EQ(line[11], "0");
            EXPECT_EQ(line[12], "6.10762");
            EXPECT_EQ(line[13], "3500");
            EXPECT_EQ(line[14], "0.15");
            EXPECT_EQ(line[15], "4");
            EXPECT_EQ(line[16], "1");
            EXPECT_EQ(line[17], "1");
            EXPECT_EQ(line[18], "100");
            EXPECT_EQ(line[19], "10");
            EXPECT_EQ(line[20], "0");
            EXPECT_EQ(line[21], "1");

            ASSERT_THROW(line[22], miocsv::NoRecord);
        }
    }
}

TEST(MIOCSV, DictReader)
{
    const auto headers = "name,link_id,from_node_id,to_node_id,facility_type,dir_flag,length,lanes,capacity,free_speed,link_type,cost,VDF_fftt1,VDF_cap1,VDF_alpha1,VDF_beta1,VDF_theta1,VDF_gamma1,VDF_mu1,RUC_rho1,RUC_resource1,RUC_type";
    auto reader = miocsv::DictReader {INPUT_FILE};
    ASSERT_EQ(reader.get_fieldnames().str(), headers);
    
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        ASSERT_LE(row_num, 2951);
        
        if (row_num == 1)
        {
            EXPECT_EQ(line[0], "");
            EXPECT_EQ(line[1], "1");
            EXPECT_EQ(line[2], "1");
            EXPECT_EQ(line[3], "547");
            EXPECT_EQ(line[4], "Highway");
            EXPECT_EQ(line[5], "1");
            EXPECT_EQ(line[6], "0.86267");
            EXPECT_EQ(line[7], "1");
            EXPECT_EQ(line[8], "49500");
            EXPECT_EQ(line[9], "60");
            EXPECT_EQ(line[10], "1");
            EXPECT_EQ(line[11], "0");
            EXPECT_EQ(line[12], "0.86267");
            EXPECT_EQ(line[13], "49500");
            EXPECT_EQ(line[14], "0.15");
            EXPECT_EQ(line[15], "4");
            EXPECT_EQ(line[16], "1");
            EXPECT_EQ(line[17], "1");
            EXPECT_EQ(line[18], "100");
            EXPECT_EQ(line[19], "10");
            EXPECT_EQ(line[20], "0");
            EXPECT_EQ(line[21], "1");

            ASSERT_THROW(line[22], miocsv::NoRecord);
            ASSERT_THROW(line["no_such_header"], miocsv::NoRecord);
        }

        if (row_num == 1024)
        {
            EXPECT_EQ(line[0], "");
            EXPECT_EQ(line[1], "1024");
            EXPECT_EQ(line[2], "554");
            EXPECT_EQ(line[3], "437");
            EXPECT_EQ(line[4], "Highway");
            EXPECT_EQ(line[5], "1");
            EXPECT_EQ(line[6], "1.037");
            EXPECT_EQ(line[7], "1");
            EXPECT_EQ(line[8], "4000");
            EXPECT_EQ(line[9], "60");
            EXPECT_EQ(line[10], "1");
            EXPECT_EQ(line[11], "0");
            EXPECT_EQ(line[12], "1.037");
            EXPECT_EQ(line[13], "4000");
            EXPECT_EQ(line[14], "0.15");
            EXPECT_EQ(line[15], "4");
            EXPECT_EQ(line[16], "1");
            EXPECT_EQ(line[17], "1");
            EXPECT_EQ(line[18], "100");
            EXPECT_EQ(line[19], "10");
            EXPECT_EQ(line[20], "0");
            EXPECT_EQ(line[21], "1");

            ASSERT_THROW(line[22], miocsv::NoRecord);
        }

        if (row_num == 1024)
        {
            EXPECT_EQ(line[0], "");
            EXPECT_EQ(line[1], "2950");
            EXPECT_EQ(line[2], "933");
            EXPECT_EQ(line[3], "534");
            EXPECT_EQ(line[4], "Highway");
            EXPECT_EQ(line[5], "1");
            EXPECT_EQ(line[6], "6.10762");
            EXPECT_EQ(line[7], "1");
            EXPECT_EQ(line[8], "3500");
            EXPECT_EQ(line[9], "60");
            EXPECT_EQ(line[10], "1");
            EXPECT_EQ(line[11], "0");
            EXPECT_EQ(line[12], "6.10762");
            EXPECT_EQ(line[13], "3500");
            EXPECT_EQ(line[14], "0.15");
            EXPECT_EQ(line[15], "4");
            EXPECT_EQ(line[16], "1");
            EXPECT_EQ(line[17], "1");
            EXPECT_EQ(line[18], "100");
            EXPECT_EQ(line[19], "10");
            EXPECT_EQ(line[20], "0");
            EXPECT_EQ(line[21], "1");

            ASSERT_THROW(line[22], miocsv::NoRecord);
        }

        EXPECT_EQ(line[0], line["name"]);
        EXPECT_EQ(line[1], line["link_id"]);
        EXPECT_EQ(line[2], line["from_node_id"]);
        EXPECT_EQ(line[3], line["to_node_id"]);
        EXPECT_EQ(line[4], line["facility_type"]);
        EXPECT_EQ(line[5], line["dir_flag"]);
        EXPECT_EQ(line[6], line["length"]);
        EXPECT_EQ(line[7], line["lanes"]);
        EXPECT_EQ(line[8], line["capacity"]);
        EXPECT_EQ(line[9], line["free_speed"]);
        EXPECT_EQ(line[10], line["link_type"]);
        EXPECT_EQ(line[11], line["cost"]);
        EXPECT_EQ(line[12], line["VDF_fftt1"]);
        EXPECT_EQ(line[13], line["VDF_cap1"]);
        EXPECT_EQ(line[14], line["VDF_alpha1"]);
        EXPECT_EQ(line[15], line["VDF_beta1"]);
        EXPECT_EQ(line[16], line["VDF_theta1"]);
        EXPECT_EQ(line[17], line["VDF_gamma1"]);
        EXPECT_EQ(line[18], line["VDF_mu1"]);
        EXPECT_EQ(line[19], line["RUC_rho1"]);
        EXPECT_EQ(line[20], line["RUC_resource1"]);
        EXPECT_EQ(line[21], line["RUC_type"]);
    }
}

// test irregular csv file
// all the readers can run without interruptions

// test regular.csv
// 1. verify headers
// 2. verify the first row
// 3. verify one row from the middle
// 4. verify the last row

// test support for CRLF (Windows) and LF (Linux and macOS)

// test Row::operator[] Out of Range

// test writer?