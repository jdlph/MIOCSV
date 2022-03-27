#include "miocsv.h"

#include <fstream>
#include <iostream>
#include <string>

using miocsv::Row;

int main()
{
    std::ifstream ist {"irregular.csv"};
    if (ist)
    {
        auto reader = miocsv::Reader(ist);
        // auto reader = miocsv::DictReader(ist);

        // the order of headers is preserved as the input file
        // std::cout << "headers are: " << reader.get_fieldnames() << '\n';

        // use range-for loop to print out the first 10 lines
        for (const auto& line: reader)
        {
            auto row_num = reader.get_row_num();
            std::cout << "line " << row_num  << ": " << line << '\n';

            if (row_num > 10)
                break;
        }
    }

    std::ofstream ost {"output.csv"};
    if (ost)
    {
        auto writer = miocsv::Writer(ost);
        // construct a dedicate record to be written using Row
        Row r = {"first way to write a record include string, int, and double",
                 "sting", 1, 1.1};
        writer.write_row(r);
        // or simply place a record to be written directly into write_row()
        writer.write_row({"second way to write a record", "string", 2, 2.0});
    }

    return 0;
}