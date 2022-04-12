#include "stdcsv.h"
#include "miocsv.h"

#include <iostream>

enum DemoCases {std_reader, std_dictreader, mio_reader, mio_dictreader, writer};

void demo_Reader();
void demo_DictReader();
void demo_MIOReader();
void demo_MIODictReader();
void demo_Writer();
void demo(DemoCases& tc);

int main()
{
    DemoCases tc {std_reader};
    demo(tc);

    std::cout << "demo done!\n";

    return 0;
}

void demo_Reader()
{
    auto inputfile {"regular.csv"};
    auto reader = miocsv::Reader(inputfile);

    // use range-for loop to print out the first 10 lines
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        if (row_num > 10)
            break;
    }
}

void demo_DictReader()
{
    auto inputfile {"regular.csv"};
    auto reader = miocsv::DictReader(inputfile);

    // the order of headers is preserved as the input file
    std::cout << "headers are: " << reader.get_fieldnames() << '\n';

    // use range-for loop to print out the first 10 lines
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        if (row_num > 10)
            break;
    }
}

void demo_MIOReader()
{
    auto inputfile {"regular.csv"};
    auto mioreader = miocsv::MIOReader(inputfile);

    for (const auto& line: mioreader)
    {
        auto row_num = mioreader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';
    }
}

void demo_MIODictReader()
{
    auto inputfile {"regular.csv"};
    auto mioreader = miocsv::MIODictReader(inputfile);

    std::cout << "headers are: " << mioreader.get_fieldnames() << '\n';

    for (const auto& line: mioreader)
    {
        auto row_num = mioreader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';
    }
}

void demo_Writer()
{
    auto outputfile {"output.csv"};
    auto writer = miocsv::Writer(outputfile);

    // first way: construct a dedicate record to be written using Row
    miocsv::Row r = {"first way to write a record include string, int, and double",
                     "sting", 1, 1.1};
    writer.write_row(r);

    // second way: simply place a record to be written directly into write_row()
    writer.write_row({"second way to write a record", "string", 2, 2.0});
}

void demo(DemoCases& tc)
{
    switch (tc)
    {
    case std_reader:
        demo_Reader();
        break;

    case std_dictreader:
        demo_DictReader();
        break;

    case mio_reader:
        demo_MIOReader();
        break;

    case mio_dictreader:
        demo_MIODictReader();
        break;

    case writer:
        demo_Writer();
        break;

    default:
        break;
    }
}