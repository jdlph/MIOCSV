#ifndef GUARD_MIOCSV_H
#define GUARD_MIOCSV_H

#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace miocsv
{

class Row {
public:
    using size_type = unsigned long;
    
    Row()=default;
    // Row()=delete;

    // pure string input
    Row(std::initializer_list<std::string> args)
    {
        for (auto& s: args)
            record.emplace_back(s);
    }
    
    template<typename T, typename... Args>
    Row(const T& t, const Args&... args)
    {
        convert_to_string(t, args);
    }

    Row(const Row&) = delete;
    Row& operator=(const Row&) = delete;
    
    Row(Row&&) = default;
    Row& operator=(Row&&) = delete;

    ~Row()=default;

    std::string& operator[](size_type i)
    {
        return record[i];
    }
    
    const std::string& operator[](size_type i) const 
    {
        return record[i];
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
        
        // is it proper to use move() here?
        // std::string str = std::move(os.str());
        // record.emplace_back(str);
        record.push_back(os.str());
    }

    template<typename T, typename... Args>
    void covert_to_string(const T& t, const Args&... args)
    {
        convert_to_string(t);
        convert_to_string(args...);
    }
    
    std::vector<std::string> record;
};

class Reader {
public:
    class iterator;
    using const_iterator = const iterator;

    using size_type = unsigned long;
    
    Reader() = delete;

    Reader(const std::string& fname_, const char delim_ = ',') 
        : fname {fname_}, delim {delim_}, f {fname}
    {
    }

    Reader(const std::string&& fname_, const char delim_ = ',') 
        : fname {fname_}, delim {delim_}, f {fname}
    {
    }

    ~Reader() = default;

    iterator begin();
    const_iterator begin() const;
    
    iterator end();
    const_iterator end() const;

    Row split_n(std::string&) const;

private:
    void read_headers();

    // support double quotes
    void split(std::string& s)
    {
        auto* r = new Row();
        std::string str1, str2;
        
        bool quoted = false;
        auto b = s.begin();
        for (auto i = s.begin(), e = s.end(); i != e; ++i)
        {
            if (*i = quote)
            {
                quoted = quoted ? false : true;
                if (!quoted)
                {
                    str1 += std::string(b, i);
                    // use b = ++i?
                    b = i + 1;
                }
            }
            else if (*i == delim && !quoted)
            {
                if (i > b)
                {
                    str2 = std::string(b, i);
                    r->append(str2);
                }
                else
                {
                    r->append(str1);
                    str1.clear();
                }

                b = i + 1;
            }
        }

        // last one
        if (!str1.empty())
            r->append(str1);
        else
        {
            str2 = std::string(b, s.end());
            r->append(str2);
        }
    }
    
    const char quote = ';';
    const char delim;
    std::string fname;
    std::ifstream f;
    std::map<std::string, size_type> fieldnames;
};

class Reader::iterator {
public:
    // iterator() : r {nullptr}
    // {
    // }

    iterator(std::ifstream& is_) : r {nullptr}, is {is_}
    {
    }
    
    iterator& operator++()
    {
        std::string s;
        std::getline(is, s);
        *r = split_n(s);

        return *this;
    }

    bool operator==(const iterator& it) const
    {
        return r == it.r;
    }

    bool operator!=(const iterator& it) const
    {
        return r != it.r;
    }

private:
    Row *r;
    std::ifstream& is;
};

Row Reader::split_n(std::string& s) const
{
    Row r;
    std::string s1, s2;
    
    bool quoted = false;
    auto b = s.begin();
    for (auto i = s.begin(), e = s.end(); i != e; ++i)
    {
        if (*i = quote)
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

class Writer {

};


} // namespace miocsv

#endif