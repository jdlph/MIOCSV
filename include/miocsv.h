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
            std::cerr << "invalid input!\n";
            std::terminate();
        }

        it = ms.begin();
    }

    MIOReader(std::string&& ms_, const char delim_ = ',')
        : BaseReader{}, ms {ms_}, delim {delim_}
    {
        if (!ms.is_mapped())
        {
            std::cerr << "invalid input!\n";
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

        try
        {
            row = parse();
        }
        catch (const InvalidRow& e)
        {
            std::cerr << e.what() << '\n';
            MIOReader::iterate();
        }

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
    static constexpr char lineter = '\n';

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
            if (!quoted && *it != quote && *it != delim && *it != lineter)
            {
                ++it = std::find(it, ms.end(), lineter);
                throw Reader::InvalidRow{row_num, sr.to_string()};
            }
        }
        else if (*it == delim && !quoted)
        {
            r.append(sr.to_string());
            sr.reset(++it);
        }
        else if (*it == lineter)
        {
            // last one
            r.append(sr.to_string());
            ++it;
            break;
        }
        else if (semi_branch_expect((it == ms.end()), true))
            break;
        else
            sr.extend(++it);
    }

    return r;
}

} // namespace miocsv

#endif