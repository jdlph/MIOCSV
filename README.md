# MIOCSV

A [MIO](https://github.com/wxinix/wxlib/tree/main/mio)-bsased C++ library to read and write tabular data in CSV format. Our goal is to develop a suite of fast and easy-to-use CSV readers and writer similar to the [csv module](https://docs.python.org/3/library/csv.html#module-csv) from the Python standard library. It serves as our first step to rebuild [DTALite](https://github.com/asu-trans-ai-lab/DTALite) using modern C++.

## A Quick Tour
Four readers and one writer are provided along with two supporting data structures.

Facility | Functionality | Core | Dependency
---------| --------------| -----| ----------
Reader | parse csv file line by line | std::getline() | C++11
DictReader | parse csv file with headers line by line | std::getline() | C++11
MIOReader | parse csv file line by line | memory mapping | mio.hpp and C++20
MIODictReader | parse csv file with headers line by line | memory mapping | mio.hpp and C++20
Writer | write user's data to a local file | std::ofstream operator<< | C++11
Row | store delimited strings or convert user’s data into strings | variadic template | C++11
StringRange | define a string range by [head, tail] to facilitate string concatenation | template | C++11

### Getting Started
***Use Reader***
```C++
#include "stdcsv.h"

#include <iostream>

int main()
{
    auto reader = miocsv::Reader {"regular.csv"};

    // use range-for loop to print out each line
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // retrieve a record using index
        std::cout << "1st record: " << line[0] << "; "
                  << "2nd record: " << line[1] << '\n';
    }

    return 0;
}
```

***Use DictReader***
```C++
#include "stdcsv.h"

#include <iostream>

int main()
{
    auto reader = miocsv::DictReader {"regular.csv"};

    // print out headers
    std::cout << "headers are: " << reader.get_fieldnames() << '\n';

    // use range-for loop to print out each line
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
    }

    return 0;
}
```

***Use MIOReader***
```C++
// you would have to place mio/mio.hpp in your inclusion folder besides miocsv.h
#include "miocsv.h"

#include <iostream>

int main()
{
    auto reader = miocsv::MIOReader {"regular.csv"};

    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // similar to Reader, you can retrieve a record using index
        std::cout << "1st record: " << line[0] << "; "
                  << "2nd record: " << line[1] << '\n';
    }

    return 0;
}
```

***Use MIODictReader***
```C++
#include "miocsv.h"

#include <iostream>

int main()
{
    auto reader = miocsv::MIODictReader {"regular.csv"};

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

    return 0;
}
```

***Use Writer***
```C++
#include "stdcsv.h"

#include <iostream>

int main()
{
    auto writer = miocsv::Writer {"output.csv"};

    // there are two ways to construct a line
    // 1st way: construct a dedicate line using Row
    miocsv::Row r = {"1st way to write a record include string, int, and double",
                     "sting", 1, 1.1};
    writer.write_row(r);

    // 2nd way: simply place a line into write_row()
    writer.write_row({"2nd way to write a record", "string", 2, 2.0});

    return 0;
}
```

### Exception Handlings

Consistency over number of records in each row (i.e., line) is not enforced (see [RFC4180](https://www.rfc-editor.org/rfc/rfc4180.txt) for details). No waring or exception will be triggered unless it is one of the followings.

Facility \ Exception | Inconsistent Number of Records | InvalidRow[^1] | Empty Row | Out of Range
---------------------| -------------------------------| ---------------| ----------|--------------
Reader | N/A | Warning | Preserve | Throw std::out_of_range
DictReader | Warning if headers are more or less than records | Warning | Discard | Throw std::out_of_range or NoRecord[^2]
MIOReader | same as Reader
MIODictReader | same as DictReader

[^1]: Any value after quoted field is not allowed, which only applies to input with double quotes. A warning with detailed information will be printed out to help users inspect.
[^2]: It happens only when a valid header is provided but there is no corresponding record (as a result of data inconsistency).

## Performance
### Time Bound at a Glance
The designed miocsv::MIOReader and miocsv::MIODictReader feature **Single Linear Search** and **One Copy-Process** in parsing each line of a CSV file, which are the minimum requirement on any CSV parser implementation. Their time complexities are both _**O(2N)**_ in comparison with _**O(7N)**_ (or _**O(6N)**_) from a regular implementation discussed below, where _N_ is the number of chars in the file (including special chars, such as white space, delimiter, and line terminator).  They are among the fastest CSV Parsers.

miocsv::Reader and miocsv::DictReader add one additional linear search and two more copy processes than their mio-based counterparts. Their running times are both bounded by _**O(5N)**_, which are still fast for majority of common use cases.

Facility | Time Complexity
---------| ---------------
Reader | _O(5N)_
DictReader |_O(5N)_
MIOReader | _O(2N)_
MIODictReader |_O(2N)_

The reason we go with _N_ rather than _n_ in time bound expressions is to better differentiate with line terminator '\n'.

### Benchmarks
upcoming

### Under the Hood
Parsing a CSV file or a file of any other delimited formats is essentially a linear search over the source file (as a stream of chars) and extract strings separated by the delimiter(s).

How fast it can iterate over the source file char by char largely determines its overall performance. As it is an I/O constrained operation, there are two general ways to speed it up.

1. multithreading
2. memory mapping

As the underlying linear search in parsing indicates a sequential operation, **multithreading** might be useless and even cause contention problems unless multiple files are being processed at the same time.

**Memory mapping** is another and natural way to increase I/O performance, especially for large files, by reducing I/O data movement (i.e., copy) and enabling way faster access operations. It allows a process (e.g., our CSV parser) to access a file and directly incorporate file data into the process address space without copying the data itself into a data buffer (e.g., std::ifstream in C++).

#### Why Two Linear Searches?
A common and easy way to implement a CSV parser is by repeating the following two serial operations.

1. retrieve a line from the file
2. parse a line into a set of strings

Operation 1 iterates every each char and search for the line terminator (i.e., '\n') while operation 2 repeats the same process but rather looking for the delimiter over the same set of chars returned from 1.

For a file with _N_ chars, this implementation involves two almost identical linear searches and implies a number of _O(2N)_ constant operations (in terms of comparison to these special chars). Why not combine them into one and reduce the operations into _O(N)_ times?  Even _O(2N)~O(N)_ in complexity analysis, their difference cannot be ignored in this context.
#### Copy Matters

There are several data copy operations going around with this implementation as illustrated by the following figure.

![Regular CSV Parser](pic/regular1.png)

The last one can be avoided by passing the container as a pointer or a reference on the heap memory, e.g., [CSVparser](https://github.com/rsylvian/CSVparser). However, it might impose additional risk of memory leak.

C++11 introduced moving semantics, which can helps us bypass it as well as Copy 4 without the side effect.

![Our Regular CSV Parser](pic/regular2.png)

Note that the string involved in copy 2 and copy 3 does nothing but only serves an intermediate media from buffered chars and the parsed substrings. Once its substrings are parsed, it becomes useless, and will be discarded while we are moving to the next line.

Therefore, why construct such a string object from the first beginning which only incurs unnecessary copy operation and additional cost on memory allocation? Why not pass its range as a pair of begin and end iterators which is equivalent but much more efficient (almost zero overhead)? To remove this copy operation, we can either build a customer string range type or simply adopt std::string_view (C++17).

With memory mapping presented before, the first copy operation is dropped as well. At this point, it leaves us with one and only one copy directly from chars in the file to the parsed substrings in conjunction with the single linear search.

![Our MIO-Based CSV Parser](pic/mio.png)

## Acknowledgement
This project is inspired by two existing works from the community.
* [mio::StringReader.getline()](https://github.com/wxinix/wxlib/blob/main/mio/include/mio/stringreader.hpp). Thanks to Dr. Wuping Xin for making this master piece!
* The parsing algorithm from [CSVparser](https://github.com/rsylvian/CSVparser). Thanks to its original author for this elegant procedure!