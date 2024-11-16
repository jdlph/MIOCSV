/**
 * @file demo.cpp, part of the project MIOCSV under Apache License 2.0
 * @author jdlph (jdlph@hotmail.com)
 * @brief Demonstrate use cases of the readers and writer from this project
 *
 * @copyright Copyright (c) 2022 - 2024 Peiheng Li, Ph.D.
 *
 */

#include <stdcsv.h>
#include <miocsv.h>

#include <algorithm>
#include <cstring>
#include <iostream>

enum DemoCase {Reader, DictReader, MIOReader, MIODictReader, Writer};

DemoCase get_demo_case(int argc, char* arg)
{
    if (argc == 1 || !std::strlen(arg))
        return Reader;

    // to lower case
    std::transform(arg, arg + std::strlen(arg), arg, [](unsigned char c) { return std::tolower(c); });

    if (!std::strcmp(arg, "dictreader"))
        return DictReader;
    else if (!std::strcmp(arg, "mioreader"))
        return MIOReader;
    else if (!std::strcmp(arg, "miodictreader"))
        return MIODictReader;
    else if (!std::strcmp(arg, "writer"))
        return Writer;
    else
        return Reader;
}

void demo_Reader()
{
    std::cout << "Demo miocsv::Reader\n\n";

    auto reader = miocsv::Reader {INPUT_FILE};
    // use range-for loop to print out the first 10 lines
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // retrieve a record using index
        // std::cout << "1st record: " << line[0] << "; "
        //           << "2nd record: " << line[1] << '\n';

        if (row_num >= 10)
            break;
    }
}

void demo_DictReader()
{
    std::cout << "Demo miocsv::DictReader\n\n";

    auto reader = miocsv::DictReader {INPUT_FILE};

    // the order of headers is preserved as the input file
    std::cout << "headers are: " << reader.get_fieldnames() << "\n\n";

    // use range-for loop to print out the first 10 lines
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // for DictReader, we offer two ways to retrieve a record
        // 1st way, via index
        std::cout << "\t2nd record: " << line[1] << "; "
                  << "3rd record: " << line[2] << '\n';

        // 2nd way, via header
        std::cout << "\tlink_id: " << line["link_id"] << "; "
                  << "facility_type: " << line["facility_type"] << '\n';

        if (row_num >= 10)
            break;
    }
}

void demo_MIOReader()
{
    std::cout << "Demo miocsv::MIOReader\n\n";

    auto reader = miocsv::MIOReader {INPUT_FILE};

    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // similar to Reader, you can retrieve a record using index
        std::cout << "\t1st record: " << line[0] << "; "
                  << "2nd record: " << line[1] << '\n';

        if (row_num >= 10)
            break;
    }
}

void demo_MIODictReader()
{
    std::cout << "Demo miocsv::MIODictReader\n\n";

    auto reader = miocsv::MIODictReader {INPUT_FILE};

    std::cout << "headers are: " << reader.get_fieldnames() << "\n\n";

    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // similar to DictReader, you can retrieve a record using either index or header
        // via index
        std::cout << "\t2nd record: " << line[1] << "; "
                  << "3rd record: " << line[2] << '\n';

        // via header
        std::cout << "\tlink_id: " << line["link_id"] << "; "
                  << "facility_type: " << line["facility_type"] << '\n';

        if (row_num >= 10)
            break;
    }
}

void demo_Writer()
{
    std::cout << "Demo miocsv::Writer\n";

    auto writer = miocsv::Writer {"output.csv"};

    // there are two ways to construct a line
    // 1st way: construct a dedicate line using Row
    miocsv::Row r = {"1st way to write a record include string, int, and double",
                     "string", 1, 1.1};
    writer.write_row(r);

    // 2nd way: simply place a line into write_row()
    writer.write_row({"2nd way to write a record", "string", 2, 2.0});
}

void demo(const DemoCase& tc)
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

int main(int argc, char* argv[])
{
    DemoCase tc = get_demo_case(argc, argv[1]);
    demo(tc);

    std::cout << "\ndemo done!\n";

    return 0;
}
