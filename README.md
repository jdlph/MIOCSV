# MIOCSV

A [MIO](https://github.com/wxinix/MIO)-bsased C++ library to read and write tabular data in CSV format. Our goal is to develop a fast and easy-to-use CSV reader and writer similar to the [csv module](https://docs.python.org/3/library/csv.html#module-csv) from the Python standard library. It serves as our first step to rebuild [DTALite](https://github.com/asu-trans-ai-lab/DTALite) using modern C++.

A fast CSV reader requires two efficient operations.
1. iterate over and and retrieve each line in the CSV file;
2. parse a line into a set of strings (fields).

We choose [mio::StringReader.getline()](https://github.com/wxinix/MIO/blob/master/include/mio/stringreader.hpp) to perform operation 1, which is way faster than std::getline(). Thanks to Dr. Wuping Xin for making this master piece! As it relies on C++17 and the [Guidelines Support Library (GSL)](https://github.com/microsoft/GSL), std::getline() is still embedded as an alternative implementation towards more flexibility.

The string split method is adopted from [CSVparser](https://github.com/rsylvian/CSVparser) with enhancement to support double quotes. Thanks to its original author for this elegant procedure!