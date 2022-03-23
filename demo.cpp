#include "miocsv.h"

#include <fstream>
#include <iostream>
#include <string>

int main()
{
    std::ifstream ist {"link.csv"};

    if (ist)
    {
        // auto reader = miocsv::Reader(ist);
        auto reader = miocsv::DictReader(ist);

        // the order of headers is not preserved as we use tree-based map
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

    return 0;
}