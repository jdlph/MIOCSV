#include "miocsv.h"

#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::string s = "link.csv";
    std::ifstream is {s};

    if (is)
    {
        // auto reader = miocsv::Reader(is);
        auto reader = miocsv::DictReader(is);
        
        auto i = 0;
        // use range-for loop
        for (auto& r: reader)
        {
            std::cout << "line " << i  << ": "<< r << '\n';
            
            ++i;
            if (i > 10)
                break;
        }
    }

    return 0;
}