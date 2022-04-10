# MIOCSV

A [MIO](https://github.com/wxinix/wxlib/tree/main/mio)-bsased C++ library to read and write tabular data in CSV format. Our goal is to develop a suite of fast and easy-to-use CSV readers and writer similar to the [csv module](https://docs.python.org/3/library/csv.html#module-csv) from the Python standard library. It serves as our first step to rebuild [DTALite](https://github.com/asu-trans-ai-lab/DTALite) using modern C++.

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

## Time Bound at a Glance
The designed miocsv::MIOReader and miocsv::MIODictReader feature Single Linear Search and One Copy-Process, which are the minimum requirement on any CSV parser implementation. Their time complexity are both O(2N) in comparison with O(7N) (or O(6N)) from a regular implementation discussed below, where N is the number of chars in the file (including special chars, such as white space, delimiter, and line terminator).  They are among the fastest CSV Parsers.

miocsv::Reader and miocsv::DictReader add one additional linear search and two more copy processes than their mio-based counterparts.  Their running times are both bounded by O(5N), which are still fast for majority of common use cases.

The reason we go with N rather than n in time bound expressions is to better differentiate with line terminator '\n'.

## Benchmarks
upcoming

## Under the Hood
Parsing a CSV file or a file of any other delimited formats is essentially a linear search over the source file (as a stream of chars) and subtract/slice/parse/extract strings separated by the delimiter(s).

How fast it can iterate over the source file char by char largely determines its overall performance. As it is an I/O constrained operation, there are two general ways to speed it up.

1. multithreading
2. memory mapping

As the underlying linear search in parsing indicates a sequential operation, multithreading might be useless and even cause contention problems unless multiple files are being processed at the same time.

Memory mapping is another and natural way to increase I/O performance, especially for large files, by reducing I/O data movement (i.e., copy). It allows a process (e.g., our CSV parser) to access a file and directly incorporate file data into the process address space without copying the data itself into a data buffer (e.g., std::ifstream in C++) besides a way faster change to the local memory of this process over the corresponding system call.

### Why Two Linear Searches?
A common and easy way to implement a CSV parser is by repeating the following two serial operations.

1. retrieve a line from the file
2. parse a line into a set of strings

Operation 1 iterates every each char and search for the line terminator (i.e., '\n') while operation 2 repeats the same process but rather looking for the delimiter over the same set of chars returned from 1. 

For a file with N chars, this implementation involves two almost identical linear searches and implies a number of O(2N) constant operations (in terms of comparison to these special chars). Why not combine them into one and reduce the operations into O(N) times.  Even O(2N)~O(N) in complexity analysis, their difference cannot be ignored in this context.
### Copy Matters

There are several data copy operations going around with this implementation as illustrated by the following figure.

![Regular CSV Parser](pic/regular1.png)

The last one can be avoided by passing the container as a pointer or a reference on the heap memory, e.g., [CSVparser](https://github.com/rsylvian/CSVparser). However, it might impose additional risk of memory leak and slow down the performance.

C++11 introduced moving semantics, which can helps us bypass it as well as Copy 4 without these two side effects.

![Our Regular CSV Parser](pic/regular2.png)

Note that the string involved in copy 2 and copy 3 does nothing but only serves an intermediate media from buffered chars and the parsed substrings. Once its substrings are parsed, it becomes useless, and will be discarded while we are moving to the next line.

Why construct such a string object from the first beginning which only incurs unnecessary copy operation and additional cost on memory allocation? Why not pass its range as a pair of begin and end iterators which is equivalent but much more efficient (almost zero overhead)? 

We can either build a customer string range type or simply adopt std::string_view (C++17). Either way will remove this copy operation.

With memory mapping presented before, the first copy operation is dropped as well. At this point, it leaves us with one and only one copy directly from chars in the file to the parsed substrings in conjunction with the single linear search.

![Our MIO-Based CSV Parser](pic/mio.png)

## Acknowledgement
1. [mio::StringReader.getline()](https://github.com/wxinix/wxlib/blob/main/mio/include/mio/stringreader.hpp) is created by Dr. Wuping Xin. Thanks to him for making this master piece!
2. The string split method is from [CSVparser](https://github.com/rsylvian/CSVparser). Thanks to its original author for this elegant procedure!