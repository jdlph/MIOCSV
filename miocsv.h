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
    using Record = std::vector<std::string>;
    using iterator = Record::iterator;
    using const_iterator = Record::const_iterator;

    Row() = default;

    // pure string input
    Row(std::initializer_list<std::string> args)
    {
        for (auto& s: args)
            record.emplace_back(s);
    }

    template<typename T, typename... Args>
    Row(const T& t, const Args&... args)
    {
        convert_to_string(t, args...);
    }

    Row(const Row&) = delete;
    Row& operator=(const Row&) = delete;

    Row(Row&&) = default;
    Row& operator=(Row&&) = default;

    ~Row() = default;

    std::string& operator[](size_type i)
    {
        return record[i];
    }

    const std::string& operator[](size_type i) const
    {
        return record[i];
    }

    // use std::runtime_error?
    std::string& operator[](std::string& s)
    {
        try
        {
            size_type i = fieldnames->at(s);
            return record[i];
        }
        catch(const std::out_of_range)
        {
            throw std::string {s + " is not existing!"};
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    const std::string& operator[](std::string& s) const
    {
        try
        {
            size_type i = fieldnames->at(s);
            return record[i];
        }
        catch(const std::out_of_range)
        {
            throw std::string {s + " is not existing!"};
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    iterator begin()
    {
        return record.begin();
    }

    const_iterator begin() const
    {
        return record.begin();
    }

    iterator end()
    {
        return record.end();
    }

    const_iterator end() const
    {
        return record.end();
    }

    size_type size() const
    {
        return record.size();
    }

    bool empty() const
    {
        return record.size() == 0;
    }

    // provide an overloaded form to take rvalue reference?
    void append(std::string& s)
    {
        record.emplace_back(s);
    }

private:
    Record record;
    // reserved for DictReader
    const FieldNames* fieldnames = nullptr;
    
    template<typename T>
    void convert_to_string(const T& t)
    {
        std::ostringstream os;
        os << t;
        // os.str() will be moved into record as os.str() is a rvalue reference
        record.push_back(os.str());
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
    using const_iterator = const Reader;

    Reader() = delete;

    Reader(std::ifstream& is_, const char delim_ = ',')
        : is {is_}, delim {delim_}, quote {';'}, row_num {0}
    {
        iterate();
    }

    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;

    Reader(Reader&&) = delete;
    Reader& operator=(Reader&&) = delete;

    ~Reader() = default;

protected:
    std::ifstream& is;
    const char delim;
    const char quote;
    size_type row_num;
    Row row;
    
    const_iterator& operator++()
    {
        if (iterate())
            return *this;
        else 
            return *end_iter; 
    }

    Row& operator*()
    {
        return row;
    }

    bool operator==(const_iterator& it) const
    {
        return *this == it;
    }

    bool operator!=(const_iterator& it) const
    {
        return *this != it;
    }
    
    const_iterator& begin() const
    {
        return *this;
    }

    const_iterator& end() const
    {
        return *end_iter;
    }

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

private:
    // end_iter cannot point to anything else
    Reader* const end_iter = nullptr;

    bool iterate()
    {
        std::string s;
        if (!std::getline(is, s))
            return false;

        row = split(s);
        ++row_num;
        return true;
    }
};

class DictReader : public Reader {
public:
    DictReader(std::ifstream& is_, const Row& fieldnames_ = {}, const char delim_ = ',')
        : Reader{is_, delim_}
    {
        setup_headers(fieldnames_);
    }

private:
    FieldNames fieldnames;

    bool iterate()
    {
        std::string s;
        if (!std::getline(is, s))
            return false;

        row = split(s);
        attach_fieldnames(row, &fieldnames);
        ++row_num;
        return true;
    }

    // use std::initializer_list<std::string> as headers can only be strings?
    void setup_headers(const Row& r)
    {
        if (fieldnames.empty())
        {
            iterate();
            for (size_type i = 0, sz = r.size(); i != sz; ++i)
            {
                const auto& s = r[i];
                fieldnames[s] = i;
            }
        }
    }
};

class Writer {

};


} // namespace miocsv

#endif