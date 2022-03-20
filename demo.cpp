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
        auto reader = miocsv::Reader(is);
        // use traditional for loop
        int i = 0;
        for (auto iter = reader.begin(); iter != reader.end(); ++iter, ++i)
        {
            std::cout << i << '\n';
        }

        // use range-for loop
        // for (auto& r: reader)
        // {
        //     std::cout << "running\n";
        // }
    }

    return 0;
}