/**
 * @file demo.cpp, part of the project MIOCSV under Apache License 2.0
 * @author jdlph (jdlph@hotmail.com)
 * @brief Use cases of the readers and writer from this project
 *
 * @copyright Copyright (c) 2022 - 2023 Peiheng Li, Ph.D.
 *
 */

#include <stdcsv.h>
#include <miocsv.h>

#include <algorithm>
#include <iostream>

enum DemoCase {Reader, DictReader, MIOReader, MIODictReader, Writer};

DemoCase get_demo_case(const char* arg)
{
    using namespace std::string_literals;

    if (!std::strlen(arg))
        return Reader;

    std::string s {arg};
    std::transform(s.cbegin(), s.cend(), s.begin(), [](unsigned char c) { return std::tolower(c); });

    if (s == "reader"s)
        return Reader;
    else if (s == "dictreader"s)
        return DictReader;
    else if (s == "mioreader"s)
        return MIOReader;
    else if (s == "miodictreader"s)
        return MIODictReader;
    else if (s == "writer"s)
        return Writer;
    else
        return Reader;
}

void demo_Reader();
void demo_DictReader();
void demo_MIOReader();
void demo_MIODictReader();
void demo_Writer();
void demo(DemoCase& tc);

int main(int argc, char* argv[])
{
    if (argc == 0)
    {
        DemoCase tc {Reader};
        demo(tc);
    }
    else
    {
        DemoCase tc = get_demo_case(argv[0]);
        demo(tc);
    }

    std::cout << "demo done!\n";

    return 0;
}

void demo_Reader()
{
    auto reader = miocsv::Reader {INPUT_FILE};

    // use range-for loop to print out the first 10 lines
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // retrieve a record using index
        // std::cout << "1st record: " << line[0] << "; "
        //           << "2nd record: " << line[1] << '\n';

        // if (row_num > 10)
        //     break;
    }
}

void demo_DictReader()
{
    auto reader = miocsv::DictReader {INPUT_FILE};

    // the order of headers is preserved as the input file
    std::cout << "headers are: " << reader.get_fieldnames() << '\n';

    // use range-for loop to print out the first 10 lines
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // for DictReader, we offer two ways to retrieve a record
        // 1st way, via index
        std::cout << "2nd record: " << line[1] << "; "
                  << "3rd record: " << line[2] << '\n';

        // 2nd way, via header
        std::cout << "link_id: " << line["link_id"] << "; "
                  << "facility_type: " << line["facility_type"] << '\n';

        if (row_num > 10)
            break;
    }
}

void demo_MIOReader()
{
    auto reader = miocsv::MIOReader {INPUT_FILE};

    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // similar to Reader, you can retrieve a record using index
        std::cout << "1st record: " << line[0] << "; "
                  << "2nd record: " << line[1] << '\n';
    }
}

void demo_MIODictReader()
{
    auto reader = miocsv::MIODictReader {INPUT_FILE};

    std::cout << "headers are: " << reader.get_fieldnames() << '\n';

    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // similar to DictReader, you can retrieve a record using either index or header
        // via index
        std::cout << "2nd record: " << line[1] << "; "
                  << "3rd record: " << line[2] << '\n';

        // via header
        std::cout << "link_id: " << line["link_id"] << "; "
                  << "facility_type: " << line["facility_type"] << '\n';
    }
}

void demo_Writer()
{
    auto writer = miocsv::Writer {"output.csv"};

    // there are two ways to construct a line
    // 1st way: construct a dedicate line using Row
    miocsv::Row r = {"1st way to write a record include string, int, and double",
                     "string", 1, 1.1};
    writer.write_row(r);

    // 2nd way: simply place a line into write_row()
    writer.write_row({"2nd way to write a record", "string", 2, 2.0});
}

void demo(DemoCase& tc)
{
    switch (tc)
    {
    case Reader:
        demo_Reader();
        break;

    case DictReader:
        demo_DictReader();
        break;

    case MIOReader:
        demo_MIOReader();
        break;

    case MIODictReader:
        demo_MIODictReader();
        break;

    case Writer:
        demo_Writer();
        break;

    default:
        break;
    }
}