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

class MIOReader : public BaseReader {
public:
    MIOReader(const std::string& ms_, const char delim_ = ',')
        : BaseReader{delim_}, ms {ms_}
    {
        if (!ms.is_mapped())
            std::cerr << "invalid input!\n";
    }
    
    MIOReader(std::string&& ms_, const char delim_ = ',')
        : BaseReader{delim_}, ms {ms_}
    {
        if (!ms.is_mapped())
            std::cerr << "invalid input!\n";
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

class MIODictReader : public MIOReader {
public:
    MIODictReader(const std::string& ist_, const Row& fieldnames_ = {}, const char delim_ = ',')
        : MIOReader{ist_, delim_}
    {
        setup_headers(fieldnames_);
    }
    
    MIODictReader(std::string&& ist_, const Row& fieldnames_ = {}, const char delim_ = ',')
        : MIOReader{ist_, delim_}
    {
        setup_headers(fieldnames_);
    }

    void setup_headers(const Row& r)
    {
        for (size_type i = 0, sz = r.size(); i != sz; ++i)
        {
            const auto& s = r[i];
            fieldnames[s] = i;
        }

        if (fieldnames.empty() && row_num == 0)
        {
            try
            {
                iterate();
                setup_headers(row);
            }
            catch (IterationEnd)
            {
                return;
            }
        }
    }

    const FieldNames& get_fieldnames() const
    {
        return fieldnames;
    }

private:
    FieldNames fieldnames;

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