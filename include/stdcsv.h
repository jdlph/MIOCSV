#ifndef GUARD_STDCSV_H
#define GUARD_STDCSV_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace miocsv
{
using size_type = unsigned long;
using FieldNames = std::map<std::string, size_type>;

/**
 * @brief a helper class to define a string range by [head, tail]
 *
 * it is to mimic std::string_view but with capability in changing head and tail.
 * we use it to avoid potential dynamic memory allocation (and deallocation) and
 * copy in string concatenation.
 */
template <typename C>
class StringRange {
public:
    using iterator = typename C::const_iterator;

    StringRange() = delete;

    explicit StringRange(iterator it) : head {it}, tail {it}
    {
    }

    StringRange(const StringRange&) = delete;
    StringRange& operator=(const StringRange&) = delete;

    StringRange(StringRange&&) = delete;
    StringRange& operator=(StringRange&&) = delete;

    ~StringRange() = default;

    bool empty() const
    {
        return tail - head == 0;
    }

    void extend(iterator it)
    {
        // check it > tail?
        tail = it;
    }

    void reset(iterator it)
    {
        head = tail = it;
    }

    std::string to_string() const
    {
        return std::string{head, tail};
    }

private:
    iterator head;
    iterator tail;
};

class Row {
    friend void attach_fieldnames(Row& r, const FieldNames* fns, size_type row_num)
    {
        r.fieldnames = fns;
        if (r.fieldnames->size() != r.size())
        {
            std::cout << "CAUTION: Data Inconsistency at line " << row_num
                      << ": " << r.fieldnames->size() << " fieldnames vs. "
                      << r.size() << " fields\n";
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Row& r)
    {
        if (r.empty())
            return os;

        for (size_type i = 0, sz = r.size(); i != sz - 1; ++i)
            os << r.records[i] << ',';
        // last one
        os << r.back();

        return os;
    }

public:
    using Records = std::vector<std::string>;
    using iterator = Records::iterator;
    using const_iterator = Records::const_iterator;

    Row() = default;

    // pure string input
    Row(std::initializer_list<std::string> args)
    {
        for (auto& s: args)
            records.emplace_back(s);
    }

    template<typename T, typename... Args>
    Row(const T& t, const Args&... args)
    {
        convert_to_string(t, args...);
    }

    Row(Records&& r)
    {
        records = r;
    }

    Row(const Row&) = default;
    Row& operator=(const Row&) = delete;

    Row(Row&&) = default;
    Row& operator=(Row&&) = default;

    ~Row() = default;

    std::string& operator[](size_type i)
    {
        if (i >= records.size() || 0 > i)
            throw std::out_of_range{std::to_string(i)};

        return records[i];
    }

    const std::string& operator[](size_type i) const
    {
        if (i >= records.size() || 0 > i)
            throw std::out_of_range{std::to_string(i)};

        return records[i];
    }

    // use std::runtime_error?
    std::string& operator[](const std::string& s)
    {
        try
        {
            size_type i = fieldnames->at(s);
            // more fieldnames than fields
            if (i >= records.size())
                throw NoRecord();

            return records[i];
        }
        catch (const std::out_of_range)
        {
            std::cerr << "std::out_of_range " << s << " is not existing!\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    /**
     * @brief retrieve a field (record) using fieldname (header)
     *
     * in case of data inconsistency, the following two cases are considered.
     *
     * 1. if there are more fields than fieldnames, users can only retrieve those
     * extra fields (with no corresponding fieldnames) using indices via overloaded
     * [].
     * 2. if there are more fieldnames than fields, an exception NoRecord will be
     * thrown at run time.
     *
     * Note that our way handling these two exceptions are different with Python
     * csv.DictReader, where, extra fields will be concatenated as a list
     * (of strings) with None assigned as a new fieldname for case 1; None will
     * be output for case 2.
     *
     * @param s
     * @return const std::string&
     */
    const std::string& operator[](const std::string& s) const
    {
        try
        {
            size_type i = fieldnames->at(s);
            // more fieldnames than fields
            if (i >= records.size())
                throw NoRecord();

            return records[i];
        }
        catch (const std::out_of_range)
        {
            std::cerr << "std::out_of_range " << s << " is not existing!\n";
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    std::string& back()
    {
        return records.back();
    }

    const std::string& back() const
    {
        return records.back();
    }

    iterator begin()
    {
        return records.begin();
    }

    const_iterator begin() const
    {
        return records.begin();
    }

    iterator end()
    {
        return records.end();
    }

    const_iterator end() const
    {
        return records.end();
    }

    size_type size() const
    {
        return records.size();
    }

    bool empty() const
    {
        return records.size() == 0;
    }

    // move non-const string
    void append(std::string& s)
    {
        records.emplace_back(s);
    }

    // move rvalue string
    void append(std::string&& s)
    {
        records.push_back(s);
    }

    // copy const string with no move
    void append(const std::string& s)
    {
        records.push_back(s);
    }

    void append(const std::string_view sv)
    {
        records.push_back(std::string{sv});
    }

private:
    Records records;
    // reserved for DictReader
    const FieldNames* fieldnames = nullptr;

    class NoRecord {

    };

    template<typename T>
    void convert_to_string(const T& t)
    {
        std::ostringstream os;
        os << t;
        // os.str() will be moved into records as os.str() is a rvalue reference
        records.push_back(os.str());
    }

    template<typename T, typename... Args>
    void convert_to_string(const T& t, const Args&... args)
    {
#if __cplusplus >= 201703L
        std::ostringstream os;
        os << t;
        records.push_back(os.str());
        // it requires C++17
        if constexpr(sizeof...(args) > 0)
            convert_to_string(args...);
#else
        convert_to_string(t);
        convert_to_string(args...);
#endif
    }
};

// not a pure abstract class
class BaseReader {
public:
    BaseReader() : quote {'"'}, row_num {0}
    {
    }

    BaseReader(const BaseReader&) = delete;
    BaseReader& operator=(const BaseReader&) = delete;

    BaseReader(BaseReader&&) = default;
    BaseReader& operator=(BaseReader&&) = delete;

    virtual ~BaseReader()
    {
    }

    // forward declaration
    class ReaderIterator;

    ReaderIterator begin();
    ReaderIterator end();

    size_type get_row_num() const
    {
        return row_num;
    }

    struct InvalidRow : public std::runtime_error {
        InvalidRow() = delete;

        InvalidRow(size_type row_num, const std::string& str)
            : std::runtime_error{"CAUTION: Invalid Row at line "
                                 + std::to_string(row_num + 1)
                                 + "! Any value after quoted field is not allowed:"
                                 + " invalid field found after " + str}
        {
        }
    };

protected:
    const char quote;
    size_type row_num;
    Row row;

    class IterationEnd {

    };

    virtual void iterate() = 0;
};

class BaseReader::ReaderIterator {
public:
    ReaderIterator() = delete;

    ReaderIterator(BaseReader* r_) : r {r_}
    {
        if (!r)
            return;

        try
        {
            r->iterate();
        }
        catch (IterationEnd)
        {
            r = nullptr;
        }
    }

    ReaderIterator(const ReaderIterator&) = default;
    ReaderIterator& operator=(const ReaderIterator&) = delete;

    ReaderIterator(ReaderIterator&&) = default;
    ReaderIterator& operator=(ReaderIterator&&) = delete;

    ~ReaderIterator() = default;

    ReaderIterator operator++()
    {
        try
        {
            r->iterate();
        }
        catch (IterationEnd)
        {
            r = nullptr;
        }

        return *this;
    }

    bool operator==(const ReaderIterator& it) const
    {
        return r == it.r;
    }

    bool operator!=(const ReaderIterator& it) const
    {
        return r != it.r;
    }

    const Row& operator*() const
    {
        return r->row;
    }

private:
    BaseReader* r;
};

// dreaded diamond
class BaseDictReader : public virtual BaseReader {
public:
    BaseDictReader() = default;

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

protected:
    FieldNames fieldnames;
};

// dreaded diamond
class Reader : public virtual BaseReader {
public:
    Reader() = delete;

    Reader(const std::string& ist_, const char delim_ = ',')
        : BaseReader{}, ist {ist_}, delim {delim_}
    {
        if (!ist)
            std::cerr << "invalid input!\n";
    }

    Reader(std::string&& ist_, const char delim_ = ',')
        : BaseReader{}, ist {ist_}, delim {delim_}
    {
        if (!ist)
            std::cerr << "invalid input!\n";
    }

protected:
    std::ifstream ist;
    const char delim;

    void iterate() override
    {
        std::string s;
        if (!std::getline(ist, s))
            throw IterationEnd();

        try
        {
            row = split2(s);
        }
        catch (const InvalidRow& e)
        {
            std::cout << e.what() << '\n';
            // this is necessary. otherwise, DictReader::iterate() will be called
            // if DictReader is being used.
            Reader::iterate();
        }

        ++row_num;
    }

private:
    // support double quotes
    Row split(const std::string& s) const;

    template<typename C>
    Row split2(const C& c) const;
};

class DictReader : public Reader, public BaseDictReader {
public:
    DictReader() = delete;

    DictReader(const std::string& ist_, const Row& fieldnames_ = {}, const char delim_ = ',')
        : Reader{ist_}, BaseDictReader{}
    {
        setup_headers(fieldnames_);
    }

    DictReader(std::string&& ist_, const Row& fieldnames_ = {}, const char delim_ = ',')
        : Reader{ist_}, BaseDictReader{}
    {
        setup_headers(fieldnames_);
    }

private:
    void iterate() override
    {
        Reader::iterate();
        // do not take blank lines in consistent with Python csv.DictReader
        while (row.empty())
            Reader::iterate();

        if (row_num > 1)
            attach_fieldnames(row, &fieldnames, row_num);
    }
};

class Writer {
public:
    Writer() = delete;

    Writer(const std::string& ost_, const char delim_ = ',')
        : ost {ost_}, delim {delim_}
    {
        if (!ost)
            std::cerr << "invalid input!\n";
    }

    Writer(std::string&& ost_, const char delim_ = ',')
        : ost {ost_}, delim {delim_}
    {
        if (!ost)
            std::cerr << "invalid input!\n";
    }

    Writer(const Writer&) = delete;
    Writer& operator=(const Writer) = delete;

    Writer(Writer&&) = default;
    Writer& operator=(Writer&&) = delete;

    ~Writer() = default;

    /**
     * @brief write a row of records into the file
     *
     * if no records contain the delimiter, then a simple implemention via the
     * overloaded operator<< for Row would work fine, i.e., os << r << '\n'.
     *
     * @param r a const reference of miocsv::Row
     */
    void write_row(const Row& r)
    {
        for (auto it = r.begin(), it_end = r.end() - 1; it != it_end; ++it)
        {
            // check if the current record contains the delimiter or not
            if (std::find(it->begin(), it->end(), delim) != it->end())
                ost << '"' << *it << '"' << delim;
            else
                ost << *it << delim;
        }

        // the last record
        if (std::find(r.back().begin(), r.back().end(), delim) != r.back().end())
            ost << '"' << r.back() << '"';
        else
            ost << r.back();

        ost << '\n';
    }

    void write_row(Row&& r)
    {
        for (auto it = r.begin(), it_end = r.end() - 1; it != it_end; ++it)
        {
            // check if the current record contains the delimiter or not
            if (std::find(it->begin(), it->end(), delim) != it->end())
                ost << '"' << *it << '"' << delim;
            else
                ost << *it << delim;
        }

        // the last record
        if (std::find(r.back().begin(), r.back().end(), delim) != r.back().end())
            ost << '"' << r.back() << '"';
        else
            ost << r.back();

        ost << '\n';
    }

private:
    std::ofstream ost;
    const char delim;
};

// implementations
inline BaseReader::ReaderIterator BaseReader::begin()
{
    // just in case users retrieve it after iteration starts
    if (this->row_num > 1)
        return nullptr;

    return ReaderIterator(this);
}

inline BaseReader::ReaderIterator BaseReader::end()
{
    return nullptr;
}

Row Reader::split(const std::string& s) const
{
    if (s.empty())
        return Row();

    Row r;
    std::string s1;

    auto quoted = false;
    auto b = s.begin();
    for (auto i = s.begin(), e = s.end(); i != e; ++i)
    {
        if (*i == quote)
        {
            quoted ^= true;
            if (!quoted)
            {
                s1 += std::string(b, i + 1);
                b = i + 1;
                if (*b != quote && *b != delim && b != e)
                    throw InvalidRow(row_num, s1);
            }
        }
        else if (*i == delim && !quoted)
        {
            if (!s1.empty())
            {
                r.append(s1);
                s1.clear();
            }
            else if (i > b)
                r.append(std::string(b, i));

            b = i + 1;
        }
    }

    // last one
    if (!s1.empty())
        r.append(s1);
    else
        r.append(std::string(b, s.end()));

    // use move constructor to avoid copy
    return r;
}

template<typename C>
Row Reader::split2(const C& c) const
{
    if (c.empty())
        return Row();

    auto b = c.begin();
    StringRange<C> sr{b};

    Row r;
    auto quoted = false;

    for (auto i = c.begin(), e = c.end(); i != e; ++i)
    {
        if (*i == quote)
        {
            quoted ^= true;
            if (!quoted)
            {
                b = i + 1;
                sr.extend(b);
                if (*b != quote && *b != delim && b != e)
                    throw InvalidRow(row_num, sr.to_string());
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
    }

    // last one
    if (!sr.empty())
        r.append(sr.to_string());
    else
        r.append(std::string(b, c.end()));

    // use move constructor to avoid copy
    return r;
}

} // namespace miocsv

std::ostream& operator<<(std::ostream& os, const miocsv::FieldNames& fns)
{
    if (fns.empty())
        return os;

    using name_pos = std::pair<std::string, miocsv::size_type>;

    std::vector<name_pos> vec {fns.begin(), fns.end()};
    // sort vec to restore the insertion order
    std::sort(vec.begin(), vec.end(), [](const name_pos& left, const name_pos& right) {
        return left.second < right.second;
    });

    for (miocsv::size_type i = 0, sz = vec.size() - 1; i != sz; ++i)
        os << vec[i].first << ',';
    // last one
    os << vec.back().first;

    return os;
}

#endif