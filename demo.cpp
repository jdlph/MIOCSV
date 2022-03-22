#include "miocsv.h"

#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::ifstream ist {"link.csv"};

    if (ist)
    {
        // auto reader = miocsv::Reader(is);
        auto reader = miocsv::DictReader(ist);

        // the key order is not preserved as we use tree-based map
        std::cout << "headers are: " << reader.get_fieldnames();

        // use range-for loop to print out the first 10 lines
        for (auto& line: reader)
        {
            auto row_num = reader.get_row_num();
            std::cout << "line " << row_num  << ": " << line << '\n';
            
            if (row_num > 10)
                break;
        }
    }

    return 0;
}