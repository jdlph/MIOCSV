# MIOCSV

A [MIO](https://github.com/wxinix/wxlib/tree/main/mio)-bsased C++ library to read and write tabular data in CSV format. Our goal is to develop a fast and easy-to-use CSV reader and writer similar to the [csv module](https://docs.python.org/3/library/csv.html#module-csv) from the Python standard library. It serves as our first step to rebuild [DTALite](https://github.com/asu-trans-ai-lab/DTALite) using modern C++.

## Get Started
upcoming
## Data Inconsistency and Exception Handlings
1. Consistency over number of records in each row is not enforced (see [RFC4180](https://www.rfc-editor.org/rfc/rfc4180.txt) for details). No waring or exception will be triggered unless it is one of the followings.
2. Data inconsistency in terms that fieldnames (headers) are more or less than fields. A warning with detailed information will be printed out to help users inspect.
3. InvalidRow. Any value after quoted field is not allowed. A warning with detailed information will be printed out to help users inspect.
4. Empty row will be preserved in Reader.
5. Empty row will be skipped in DictReader.

### Reader
Users can retrieve a record in a line parsed by Reader via index (nonnegative integer).
* In case of non-existing index, std::out_of_range will be thrown.

### DictReader
For DictReader, users are able to retrieve a record in a line using either index (nonnegative integer) or fieldname (std::string).
1. Similar to Reader, std::out_of_range will be thrown if index is not existing.
2. std::out_of_range will be thrown as well if field does not exist.
3. NoRecord. It happens only when a valid fieldname is provided but there is no corresponding record (as a result of data inconsistency). NoRecord will be thrown.

## Under the Hood

A fast CSV reader generally requires two efficient operations.
1. iterate over and and retrieve each line in the CSV file.
2. parse a line into a set of strings (fields).

We choose [mio::StringReader.getline()](https://github.com/wxinix/wxlib/blob/main/mio/include/mio/stringreader.hpp) to perform operation 1, which is way faster than std::getline() (prior to C++20). As it relies on C++20, std::getline() is still embedded as an alternative implementation towards more flexibility.

The string split method is adopted from [CSVparser](https://github.com/rsylvian/CSVparser) with enhancement to support double quotes.
### Copy Matters
A common implementation of CSV parser usually involves the following copies, which are crucial to the overall performance.

1. copy from a stream of chars (input) into a string (as a line), e.g., std::getline().
2. split the above string and copy into a set of substrings separated by delimiter.
3. copy the substrings into a container to pass to its caller.
4. passing the data container itself may involve another copy which indicates copy of its elements.

Copy 4 can be easily avoided by passing the container as a pointer or a reference on the heap memory, e.g., [CSVparser](https://github.com/rsylvian/CSVparser), which imposes potential risk and slows down performance. C++11 introduced the moving semantics, which helps us to bypass both Copy 4 and Copy 3 without these two side effects.

This still leaves us with two set of copy operations. Can we do better to further improve the performance?

### Why Two Linear Searches?

Note that the string as a line in 1 and 2 has a limited lifecycle. It becomes useless after we get the substrings, and will be discarded while we are moving to the next line. If its role is only to serve an intermediate media from a line of chars to substrings, why construct a string object? Passing its range as a pair of begin and end iterators is equivalent but much more efficient (almost zero overhead). We can either build such a string range type or simply adopt string_view (C++17), which helps us remove another copy.

On the other hand, operation 1 is a linear search over a stream of chars (looking for next line) while operation 2 repeats the same search over the same set of chars (looking for delimiter and quote). Why not combine them into one?

## Benchmarks
upcoming

## Acknowledgement
1. [mio::StringReader.getline()](https://github.com/wxinix/wxlib/blob/main/mio/include/mio/stringreader.hpp) is created by Dr. Wuping Xin. Thanks to him for making this master piece!
2. The string split method is from [CSVparser](https://github.com/rsylvian/CSVparser). Thanks to its original author for this elegant procedure!