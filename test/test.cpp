#include <stdcsv.h>
#include <miocsv.h>

#include <iostream>

enum DemoCases {std_reader, std_dictreader, mio_reader, mio_dictreader};

void test_Reader();
void test_DictReader();
void test_MIOReader();
void test_MIODictReader();
void test(DemoCases& tc);

int main()
{
    DemoCases tc {mio_reader};

    test(tc);

    std::cout << "benchmark done!\n";

    return 0;
}

void test_Reader()
{
    auto ts = std::chrono::high_resolution_clock::now();
    auto reader = miocsv::Reader {"csvreader.csv"};

    for (const auto& line: reader)
    {
        // do nothing
    }

    auto te = std::chrono::high_resolution_clock::now();

    std::cout << "miocsv::Reader parses " << reader.get_row_num() << " lines in " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count()
              << " milliseconds\n";
}

void test_DictReader()
{
    auto ts = std::chrono::high_resolution_clock::now();
    auto reader = miocsv::DictReader {"csvreader.csv"};

    for (const auto& line: reader)
    {
        // do nothing
    }

    auto te = std::chrono::high_resolution_clock::now();

    std::cout << "miocsv::DictReader parses " << reader.get_row_num() << " lines in " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count()
              << " milliseconds\n";
}

void test_MIOReader()
{
    auto ts = std::chrono::high_resolution_clock::now();
    auto mioreader = miocsv::MIOReader {"csvreader.csv"};

    for (const auto& line: mioreader)
    {
        // do nothing
    }

    auto te = std::chrono::high_resolution_clock::now();

    std::cout << "miocsv::MIOReader parses " << mioreader.get_row_num() << " lines in " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count()
              << " milliseconds\n";
}

void test_MIODictReader()
{
    auto ts = std::chrono::high_resolution_clock::now();
    auto mioreader = miocsv::MIODictReader {"csvreader.csv"};

    for (const auto& line: mioreader)
    {
        // do nothing
    }

    auto te = std::chrono::high_resolution_clock::now();

    std::cout << "miocsv::MIODictReader parses " << mioreader.get_row_num() << " lines in " 
              << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count()
              << " milliseconds\n";
}

void test(DemoCases& tc)
{
    switch (tc)
    {
    case std_reader:
        test_Reader();
        break;

    case std_dictreader:
        test_DictReader();
        break;

    case mio_reader:
        test_MIOReader();
        break;

    case mio_dictreader:
        test_MIODictReader();
        break;

    default:
        break;
    }
}