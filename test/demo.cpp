#include "stdcsv.h"
#include "miocsv.h"

enum TestCases {std_reader, std_dictreader, mio_reader, mio_dictreader, writer};

void test_Reader();
void test_DictReader();
void test_MioReader();
void test_MioDictReader();
void test_Writer();
void test(TestCases& tc);

int main()
{
    TestCases tc {std_reader};
    test(tc);

    std::cout << "test done!\n";

    return 0;
}

void test(TestCases& tc)
{
    switch (tc)
    {
    case std_reader:
        test_Reader();

    case std_dictreader:
        test_DictReader();

    case mio_reader:
        test_MioReader();

    case mio_dictreader:
        test_MioDictReader();

    case writer:
        test_Writer();

    default:
        break;
    }
}

void test_Reader()
{
    auto inputfile {"irregular.csv"};
    auto reader = miocsv::Reader(inputfile);

    // use range-for loop to print out the first 10 lines
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        if (row_num > 10)
            break;
    }
}

void test_DictReader()
{
    auto inputfile {"irregular.csv"};
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

void test_MioReader()
{
    auto inputfile {"irregular.csv"};
    auto mioreader = miocsv::MIOReader(inputfile);

    for (const auto& line: mioreader)
    {
        auto row_num = mioreader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';
    }
}

void test_MioDictReader()
{
    auto inputfile {"irregular.csv"};
    auto mioreader = miocsv::MIODictReader(inputfile);

    std::cout << "headers are: " << mioreader.get_fieldnames() << '\n';

    for (const auto& line: mioreader)
    {
        auto row_num = mioreader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';
    }
}

void test_Writer()
{
    auto outputfile {"output.csv"};
    auto writer = miocsv::Writer(outputfile);

    // first way: construct a dedicate record to be written using Row
    miocsv::Row r = {"first way to write a record include string, int, and double",
                     "sting", 1, 1.1};
    writer.write_row(r);

    // second way: simply place a record to be written directly into write_row()
    writer.write_row({"second way to write a record", "string", 2, 2.0});
}