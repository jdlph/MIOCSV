#include <stdcsv.h>
#include <miocsv.h>

#include <iostream>

enum DemoCases {std_reader, std_dictreader, mio_reader, mio_dictreader, writer};

void demo_Reader();
void demo_DictReader();
void demo_MIOReader();
void demo_MIODictReader();
void demo_Writer();
void demo(DemoCases& tc);

int main()
{
    DemoCases tc {std_reader};
    demo(tc);

    std::cout << "demo done!\n";

    return 0;
}

void demo_Reader()
{
    auto reader = miocsv::Reader {"/Users/jdlph/Dev/MIOCSV/test/irregular.csv"};

    // use range-for loop to print out the first 10 lines
    for (const auto& line: reader)
    {
        auto row_num = reader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // retrieve a record using index
        // std::cout << "1st record: " << line[0] << "; "
        //           << "2nd record: " << line[1] << '\n';

        // if (row_num > 10)
        //     break;
    }
}

void demo_DictReader()
{
    auto reader = miocsv::DictReader {"regular.csv"};

    // the order of headers is preserved as the input file
    std::cout << "headers are: " << reader.get_fieldnames() << '\n';

    // use range-for loop to print out the first 10 lines
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

        if (row_num > 10)
            break;
    }
}

void demo_MIOReader()
{
    auto mioreader = miocsv::MIOReader {"regular.csv"};

    for (const auto& line: mioreader)
    {
        auto row_num = mioreader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // similar to Reader, you can retrieve a record using index
        std::cout << "1st record: " << line[0] << "; "
                  << "2nd record: " << line[1] << '\n';
    }
}

void demo_MIODictReader()
{
    auto mioreader = miocsv::MIODictReader {"regular.csv"};

    std::cout << "headers are: " << mioreader.get_fieldnames() << '\n';

    for (const auto& line: mioreader)
    {
        auto row_num = mioreader.get_row_num();
        std::cout << "line " << row_num  << ": " << line << '\n';

        // similar to DictReader, you can retrieve a record using either index or header
        // via index
        std::cout << "2nd record: " << line[1] << "; "
                  << "3rd record: " << line[2] << '\n';

        // via header
        std::cout << "link_id: " << line["link_id"] << "; "
                  << "facility_type: " << line["facility_type"] << '\n';
    }
}

void demo_Writer()
{
    auto writer = miocsv::Writer {"output.csv"};

    // there are two ways to construct a line
    // 1st way: construct a dedicate line using Row
    miocsv::Row r = {"1st way to write a record include string, int, and double",
                     "sting", 1, 1.1};
    writer.write_row(r);

    // 2nd way: simply place a line into write_row()
    writer.write_row({"2nd way to write a record", "string", 2, 2.0});
}

void demo(DemoCases& tc)
{
    switch (tc)
    {
    case std_reader:
        demo_Reader();
        break;

    case std_dictreader:
        demo_DictReader();
        break;

    case mio_reader:
        demo_MIOReader();
        break;

    case mio_dictreader:
        demo_MIODictReader();
        break;

    case writer:
        demo_Writer();
        break;

    default:
        break;
    }
}