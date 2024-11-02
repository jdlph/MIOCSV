/**
 * @file test.cpp, part of the project MIOCSV under Apache License 2.0
 * @author jdlph (jdlph@hotmail.com)
 * @brief Benchmark the readers and writer from this project
 *
 * @copyright Copyright (c) 2022 - 2024 Peiheng Li, Ph.D.
 *
 */

#include <stdcsv.h>
#include <miocsv.h>

#include <fstream>
#include <iostream>
#include <string>

enum DemoCases {std_reader, std_dictreader, mio_reader, mio_dictreader, std_getline};

void test_Reader();
void test_DictReader();
void test_MIOReader();
void test_MIODictReader();
void test_getline();
void test(DemoCases& tc);

int main()
{
    DemoCases tc {std_dictreader};

    test(tc);

    std::cout << "benchmark done!\n";

    return 0;
}

void test_Reader()
{
    auto ts = std::chrono::high_resolution_clock::now();
    auto reader = miocsv::Reader {"data/csvreader.csv"};

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
    auto reader = miocsv::DictReader {"data/csvreader.csv"};

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
    auto mioreader = miocsv::MIOReader {"data/csvreader.csv"};

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
    auto mioreader = miocsv::MIODictReader {"data/csvreader.csv"};

    for (const auto& line: mioreader)
    {
        // do nothing
    }

    auto te = std::chrono::high_resolution_clock::now();

    std::cout << "miocsv::MIODictReader parses " << mioreader.get_row_num() << " lines in "
              << std::chrono::duration_cast<std::chrono::milliseconds>(te - ts).count()
              << " milliseconds\n";
}

void test_getline()
{
    auto ts = std::chrono::high_resolution_clock::now();
    std::ifstream ist {"data/csvreader.csv"};

    auto line_num = 0;
    for (std::string s; std::getline(ist, s); ++line_num)
    {
        // do nothing
    }

    auto te = std::chrono::high_resolution_clock::now();

    std::cout << "std::getline() reads " << line_num << " lines in "
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

    case std_getline:
        test_getline();
        break;

    default:
        break;
    }
}