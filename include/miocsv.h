/**
 * @file miocsv.h, part of the project MIOCSV under Apache License 2.0
 * @author jdlph (jdlph@hotmail.com)
 * @brief A suite of lightening-fast CSV parsers built upon memory mapping (MIO)
 *
 * @copyright Copyright (c) 2022 - 2024 Peiheng Li, Ph.D.
 *
 */

#ifndef GUARD_MIOCSV_H
#define GUARD_MIOCSV_H

#include "mio/mio.hpp"
#include "stdcsv.h"

#ifdef __GNUC__
#define semi_branch_expect(x, y) __builtin_expect(x, y)
#else
#define semi_branch_expect(x, y) x
#endif

namespace miocsv
{
class MIOReader : public virtual BaseReader {
public:
    MIOReader() = delete;

    MIOReader(const std::string& ms_, const char delim_ = ',')
        : BaseReader{}, ms {ms_}, delim {delim_}
    {
        if (!ms.is_mapped())
        {
            std::cerr << ms_ << "is not successfully mapped!\n";
            std::terminate();
        }

        it = ms.begin();
    }

    MIOReader(std::string&& ms_, const char delim_ = ',')
        : BaseReader{}, ms {ms_}, delim {delim_}
    {
        if (!ms.is_mapped())
        {
            std::cerr << ms_ << "is not successfully mapped!\n";
            std::terminate();
        }

        it = ms.begin();
    }

    ~MIOReader()
    {
        ms.unmap();
    }

protected:
    mio::mmap_source ms;
    const char delim;
    const char* it;

    void iterate() override
    {
        // EOF is reached
        if (it == ms.end())
            throw IterationEnd{};

        row = parse();
        ++row_num;
    }

private:
    Row parse();
};

class MIODictReader : public MIOReader, public BaseDictReader {
public:
    MIODictReader() = delete;

    MIODictReader(const std::string& ist_, const Row& fieldnames_ = {}, const char delim_ = ',')
        : MIOReader{ist_, delim_}, BaseDictReader{}
    {
        setup_headers(fieldnames_);
    }

    MIODictReader(std::string&& ist_, const Row& fieldnames_ = {}, const char delim_ = ',')
        : MIOReader{ist_, delim_}, BaseDictReader{}
    {
        setup_headers(fieldnames_);
    }

private:
    void iterate() override
    {
        MIOReader::iterate();
        // do not take blank lines
        while (row.empty())
            MIOReader::iterate();

        if (row_num > 1)
            attach_fieldnames(row, &fns, row_num);
    }
};

Row MIOReader::parse()
{
    Row r;
    auto quoted = false;
    StringRange<const char*> sr{it};

    // caution: the last line might be null terminated rather than '\n'
    while (true)
    {
        if (*it == quote)
        {
            sr.extend(++it);
            quoted ^= true;
#ifdef FORMAT_CHECKER
            if (!quoted && *it != quote && *it != delim && *it != CR && *it != LF)
            {
                std::cerr << "CAUTION: Invalid Row at line " << row_num + 1
                          << "! Value is not allowed after quoted field: "
                          << sr.to_string() << '\n';

                it = std::find(it, ms.end(), LF);
                // "it" may have reached EOF
                if (it++ == ms.end())
                    return r;
            }
#endif
        }
        else if (*it == delim && !quoted)
        {
            r.append(sr.to_string());
            sr.reset(++it);
        }
        else if (*it == LF)
        {
            // last one
            if (sr.back() != CR)
                r.append(sr.to_string());
            else
                r.append(sr.to_string_cr());

            ++it;
            return r;
        }
        else if (semi_branch_expect((it == ms.end()), true))
            return r;
        else
            sr.extend(++it);
    }
}

} // namespace miocsv

#endif