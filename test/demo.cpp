#include "stdcsv.h"
#include "miocsv.h"

#include <fstream>
#include <iostream>
#include <string>

using miocsv::Row;

int main()
{
    auto test_reader = false;
    auto test_mioreader = true;
    auto test_writer = false;
    
    auto inputfile = "/Users/jdlph/Dev/MIOCSV/test/irregular.csv";
    
    if (test_reader)
    {
        // auto reader = miocsv::Reader(inputfile);
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

    if (test_mioreader)
    {
        auto mioreader = miocsv::MIOReader(inputfile);
        // std::cout << "headers are: " << mioreader.get_fieldnames() << '\n';

        for (const auto& line: mioreader)
        {
            auto row_num = mioreader.get_row_num();
            std::cout << "line " << row_num  << ": " << line << '\n';
        }
    }

    if (test_writer)
    {
        auto outputfile {"output.csv"};
        auto writer = miocsv::Writer(outputfile);
        // construct a dedicate record to be written using Row
        Row r = {"first way to write a record include string, int, and double",
                    "sting", 1, 1.1};
        writer.write_row(r);
        // or simply place a record to be written directly into write_row()
        writer.write_row({"second way to write a record", "string", 2, 2.0});
    }

    std::cout << "test done!\n";

    return 0;
}