#ifndef GUARD_MIOCSV_H
#define GUARD_MIOCSV_H

#include "stdcsv.h"
#include "mio/stringreader.hpp"

// if (sr.eof())
//     throw IterationEnd();

// std::string_view s = sr.getline();
// row = sr.parse(row_num);

#ifdef USE_MIO
std::vector<std::string> mio::StringReader::parse(miocsv::size_type row_num, const char quote, const char delim)
{
    std::vector<std::string> vec;
    if (*m_begin == '\n')
    {
        ++m_begin;
        return vec;
    }

    auto quoted = false;
    auto i = m_begin;
    auto b = m_begin;
    miocsv::StringRange<std::string_view> sr{b};

    while (*i != '\n' && semi_branch_expect((i != m_mmap.end()), true))
    {
        if (*i == quote)
        {
            quoted ^= true;
            if (!quoted)
            {
                b = i + 1;
                sr.extend(b);
                if (*b != quote && *b != delim && *b != '\n')
                {
                    i = std::find(i, m_mmap.end(), '\n');
                    m_begin = i + 1;
                    throw miocsv::Reader::InvalidRow(row_num, sr.to_string());
                }
            }
        }
        else if (*i == delim && !quoted)
        {
            if (!sr.empty())
                vec.emplace_back(sr.to_string());
            else if (i > b)
                vec.emplace_back(std::string(b, i));

            b = i + 1;
            sr.reset(b);
        }
        ++i;
    }

    // last one
    if (!sr.empty())
        vec.emplace_back(sr.to_string());
    else
        vec.emplace_back(std::string(b, i));

    if (semi_branch_expect((i != m_mmap.end()), true))
        m_begin = i + 1;
    else
        m_begin = nullptr;

    return vec;
}
#endif

#endif