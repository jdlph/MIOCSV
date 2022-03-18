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

class Row {
public:
    using size_type = unsigned long;
    using iterator = std::vector<std::string>::iterator;
    using const_iterator = std::vector<std::string>::const_iterator;

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
        return record.size() != 0;
    }

    void append(std::string& str)
    {
        record.emplace_back(str);
    }

private:
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

    friend void attach_fieldnames(Row& r, const std::map<std::string, size_type>* fieldnames_)
    {
        r.fieldnames = fieldnames_;
    }

    std::vector<std::string> record;
    const std::map<std::string, size_type>* fieldnames = nullptr;
};

class Reader {
public:
    // class iterator;
    using iterator = Row*;
    using const_iterator = const Row*;
    using size_type = unsigned long;

    Reader() = delete;

    Reader(std::ifstream& is_, const char delim_ = ',')
        : is {is_}, delim {delim_}, quote {';'}, row_num {0}, iter {nullptr}
    {
        iterate();
    }

    Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;

    Reader(Reader&&) = delete;
    Reader& operator=(Reader&&) = delete;

    ~Reader() = default;

    iterator& operator++()
    {
        iterate();
        return iter;
    }

    bool operator==(const iterator& it) const
    {
        return iter == it;
    }

    bool operator!=(const iterator& it) const
    {
        return iter != it;
    }

protected:
    iterator begin()
    {
        return iter;
    }

    const_iterator begin() const
    {
        return iter;
    }

    iterator end()
    {
        return nullptr;
    }

    const_iterator end() const
    {
        return nullptr;
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
    void iterate()
    {
        std::string s;
        std::getline(is, s);
        *iter = split(s);
        ++row_num;
    }

protected:
    std::ifstream& is;
    const char delim;
    const char quote;
    size_type row_num;
    iterator iter;
};

class DictReader : public Reader {
public:
    DictReader() = delete;
    
    DictReader(std::ifstream& is_, const Row& fieldnames_ = {}, const char delim_ = ',')
        : Reader{is_, delim_}
    {
        setup_headers(fieldnames_);
    }

private:
    std::map<std::string, size_type> fieldnames;

    // use std::initializer_list<std::string> as headers can only be strings?
    void setup_headers(const Row& r)
    {
        if (fieldnames.empty() && row_num == 0)
        {
            iterate();
            for (size_type i = 0, sz = r.size(); i != sz; ++i)
            {
                std::string s = r[i];
                fieldnames[s] = i;
            }
        }
    }

    void iterate()
    {
        std::string s;
        std::getline(is, s);
        *iter = split(s);
        attach_fieldnames(*iter, &fieldnames);
        ++row_num;
    }
};

class Writer {

};


} // namespace miocsv

#endif