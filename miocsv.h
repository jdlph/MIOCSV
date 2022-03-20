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
    std::string& operator[](std::string& s)
    {
        try
        {
            size_type i = fieldnames->at(s);
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

    const std::string& operator[](std::string& s) const
    {
        try
        {
            size_type i = fieldnames->at(s);
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

    // provide an overloaded form to take rvalue reference?
    void append(std::string& s)
    {
        records.emplace_back(s);
    }

private:
    Records records;
    // reserved for DictReader
    const FieldNames* fieldnames = nullptr;

    template<typename T>
    void convert_to_string(const T& t)
    {
        std::ostringstream os;
        os << t;
        // os.str() will be moved into record as os.str() is a rvalue reference
        records.push_back(os.str());
    }

    template<typename T, typename... Args>
    void covert_to_string(const T& t, const Args&... args)
    {
        convert_to_string(t);
        convert_to_string(args...);
    }

    friend void attach_fieldnames(Row& r, const FieldNames* fieldnames_)
    {
        r.fieldnames = fieldnames_;
    }
};

class Reader {
public:
    class iterator;

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

    iterator begin();
    iterator end();

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
    // end_iter cannot point to anything else
    // iterator end_iter = nullptr;

    // support double quotes
    Row split(std::string& s) const
    {
        if (s.empty())
            return nullptr;

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
                    // use b = ++i?
                    b = i + 1;
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

                b = i + 1;
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

class Reader::iterator {
public:
    iterator(Reader* r_) : r {r_} 
    {
        if (r == nullptr)
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

    iterator(const iterator&) = default;
    iterator& operator=(const iterator&) = delete;

    iterator(iterator&&) = default;
    iterator& operator=(iterator&&) = delete;

    ~iterator() = default;
    
    iterator operator++()
    {
        try
        {
            r->iterate();
            return *this;
        }
        catch(IterationEnd)
        {
            r = nullptr;
            return nullptr;
        }
    }

    bool operator==(const iterator& it) const
    {
        return r == it.r;
    }

    bool operator!=(const iterator& it) const
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

Reader::iterator Reader::begin()
{
    // just in case users retrieve it after iteration starts
    if (this->row_num > 1)
        return nullptr;
    
    return iterator(this);
}

inline Reader::iterator Reader::end()
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

private:
    FieldNames fieldnames;

    void iterate() override
    {
        Reader::iterate();
        attach_fieldnames(row, &fieldnames);
    }
};

class Writer {

};


} // namespace miocsv

#endif