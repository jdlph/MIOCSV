#ifndef GUARD_MIOCSV_H
#define GUARD_MIOCSV_H

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

class Row {
    friend void attach_fieldnames(Row& r, const FieldNames* fieldnames_)
    {
        r.fieldnames = fieldnames_;
        if (r.fieldnames->size() != r.size())
        {
            std::cout << "CAUTION: Data Inconsistency! " << r.fieldnames->size() 
                      << "fieldnames vs. " << r.size() << " fields\n";
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Row& r)
    {   
        for (size_type i = 0, sz = r.size(); i != sz - 1; ++i)
            os << r.records[i] << ',';
        
        os << r.records.back();

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

    Row(const Row&) = default;
    Row& operator=(const Row&) = delete;

    Row(Row&&) = default;
    Row& operator=(Row&&) = default;

    ~Row() = default;

    std::string& operator[](size_type i)
    {
        return records[i];
    }

    const std::string& operator[](size_type i) const
    {
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
            throw std::string {s + " is not existing!"};
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
            throw std::string {s + " is not existing!"};
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
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
        convert_to_string(t);
        convert_to_string(args...);
    }
};

class Reader {
public:
    class ReaderIterator;

    Reader() = delete;

    Reader(std::ifstream& is_, const char delim_ = ',')
        : is {is_}, delim {delim_}, quote {';'}, row_num {0}
    {
    }

    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;

    Reader(Reader&&) = default;
    Reader& operator=(Reader&&) = delete;

    ~Reader() = default;

    ReaderIterator begin();
    ReaderIterator end();

    size_type get_row_num() const
    {
        return row_num;
    }

protected:
    std::ifstream& is;
    const char delim;
    const char quote;
    size_type row_num;
    Row row;

    class IterationEnd {

    };

    virtual void iterate()
    {
        std::string s;
        if (!std::getline(is, s))
            throw IterationEnd();

        row = split(s);
        ++row_num;
    }

private:
    // support double quotes
    Row split(std::string& s) const
    {
        if (s.empty())
            return Row();

        Row r;
        std::string s1, s2;

        bool quoted = false;
        auto b = s.begin();
        for (auto i = s.begin(), e = s.end(); i != e; ++i)
        {
            if (*i == quote)
            {
                quoted = quoted ? false : true;
                if (!quoted)
                {
                    s1 += std::string(b, i);
                    // b = i + 1;
                    b = ++i;
                }
            }
            else if (*i == delim && !quoted)
            {
                if (i > b)
                {
                    s2 = std::string(b, i);
                    r.append(s2);
                }
                else
                {
                    r.append(s1);
                    s1.clear();
                }

                // b = i + 1;
                b = ++i;
            }
        }

        // last one
        if (!s1.empty())
            r.append(s1);
        else
        {
            s2 = std::string(b, s.end());
            r.append(s2);
        }

        // use move constructor to avoid copy
        return r;
    }
};

class Reader::ReaderIterator {
public:
    ReaderIterator() = delete;

    ReaderIterator(Reader* r_) : r {r_}
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
            return *this;
        }
        catch(IterationEnd)
        {
            r = nullptr;
            return *this;
        }
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
    Reader* r;
};

inline Reader::ReaderIterator Reader::begin()
{
    // just in case users retrieve it after iteration starts
    if (this->row_num > 1)
        return nullptr;

    return ReaderIterator(this);
}

inline Reader::ReaderIterator Reader::end()
{
    return nullptr;
}

class DictReader : public Reader {
public:
    DictReader(std::ifstream& is_, const Row& fieldnames_ = {}, const char delim_ = ',')
        : Reader{is_, delim_}
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
        Reader::iterate();
        // do not take blank lines in consistent with Python csv.DictReader
        while (row.empty())
            Reader::iterate();

        attach_fieldnames(row, &fieldnames);
    }
};

class Writer {
public:
    Writer() = delete;

    Writer(std::ofstream& os_) : os {os_}
    {
    }

    Writer(const Writer&) = delete;
    Writer& operator=(const Writer) = delete;

    Writer(Writer&&) = default;
    Writer& operator=(Writer&&) = delete;

    ~Writer() = default;

    void write_row(const Row& r)
    {
        os << r << '\n';
    }

    void write_row(Row&& r)
    {
        os << r << '\n';
    }

private:
    std::ofstream& os;
};

// some helper functions
std::fstream open_csv(const std::string& filename, const char mode = 'r')
{
    std::fstream fs;

    const char mode_ = std::tolower(mode);
    switch (mode_)
    {
        case 'r':
        {
            fs.open(filename, std::ios_base::in);
            if (fs)
                return fs;
        }
        case 'w':
        {
            fs.open(filename, std::ios_base::out);
            if (fs)
                return fs;
        }
        default:
            std::cout << "please provide the right mode: 'r' for read or 'w' for write\n";
            break;
    }
}

} // namespace miocsv


std::ostream& operator<<(std::ostream& os, const miocsv::FieldNames& fns)
{
    std::vector<std::pair<std::string, miocsv::size_type>> vec {fns.begin(), fns.end()};
    // sort vec to restore the insertion order
    std::sort(vec.begin(), vec.end(), [](auto& left, auto& right) { 
        return left.second < right.second;
    });

    for (miocsv::size_type i = 0, sz = vec.size() - 1; i != sz; ++i)
        os << vec[i].first << ',';

    os << vec.back().first;

    return os;
}

#endif