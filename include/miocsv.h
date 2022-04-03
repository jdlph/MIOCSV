#ifndef GUARD_MIOCSV_H
#define GUARD_MIOCSV_H

#include "stdcsv.h"
#include "mio/mio.hpp"
#include "mio/stringreader.hpp"

#ifdef __GNUC__
#define semi_branch_expect(x, y) __builtin_expect(x, y)
#else
#define semi_branch_expect(x, y) x
#endif

namespace miocsv
{

class MIOReader : public virtual BaseReader {
public:
    MIOReader()= delete;
    
    MIOReader(const std::string& ms_, const char delim_ = ',')
        : BaseReader{delim_}, ms {ms_}
    {
        if (!ms.is_mapped())
            std::cerr << "invalid input!\n";

        delim = delim_;
        it = ms.begin();
    }

    MIOReader(std::string&& ms_, const char delim_ = ',')
        : BaseReader{delim_}, ms {ms_}
    {
        if (!ms.is_mapped())
            std::cerr << "invalid input!\n";

        delim = delim_;
        it = ms.begin();
    }

protected:
    mio::mmap_source ms;
    const char* it;

    void iterate() override
    {
        if (it == nullptr)
            throw IterationEnd{};

        try
        {
            row = parse();
        }
        catch(const InvalidRow& e)
        {
            std::cerr << e.what() << '\n';
            MIOReader::iterate();
        }

        ++row_num;
    }

private:
    Row parse();

    template<typename C>
    Row parse(const C& c) const;
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
            attach_fieldnames(row, &fieldnames, row_num);
    }
};

Row MIOReader::parse()
{
    static constexpr char lineter = '\n';

    if (*it == lineter)
    {
        ++it;
        return Row{};
    }

    auto i = it;
    auto b = it;
    miocsv::StringRange<std::string_view> sr{b};

    Row r;
    auto quoted = false;

    while (*i != lineter && semi_branch_expect((i != ms.end()), true))
    {
        if (*i == quote)
        {
            quoted ^= true;
            if (!quoted)
            {
                b = i + 1;
                sr.extend(b);
                if (*b != quote && *b != delim && *b != lineter)
                {
                    i = std::find(i, ms.end(), lineter);
                    it = i + 1;
                    throw miocsv::Reader::InvalidRow(row_num, sr.to_string());
                }
            }
        }
        else if (*i == delim && !quoted)
        {
            if (!sr.empty())
                r.append(sr.to_string());
            else if (i > b)
                r.append(std::string(b, i));

            b = i + 1;
            sr.reset(b);
        }
        ++i;
    }

    // last one
    if (!sr.empty())
        r.append(sr.to_string());
    else
        r.append(std::string(b, i));

    if (semi_branch_expect((i != ms.end()), true))
        it = i + 1;
    else
        it = nullptr;

    return r;
}

} // namespace miocsv

#endif