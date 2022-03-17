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
    using iterator = std::vector<std::string>::iterator;
    using const_iterator = std::vector<std::string>::const_iterator;
    
    Row() = delete;

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
    
    std::vector<std::string> record;
};

class Reader {
public:
    // class iterator;
    using iterator = Row*;
    using const_iterator = const Row*;
    using size_type = unsigned long;
    
    Reader() = delete;

    Reader(std::ifstream& is_, bool hasheaders_ = true, const char delim_ = ',') 
        : is {is_}, hasheaders {hasheaders_}, delim {delim_}, 
          quote {';'}, row_num {0}, iter {nullptr}
    {
        if (hasheaders)
            setup_headers();
        else
            next();
    }

    ~Reader() = default;

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
    
    iterator& operator++()
    {
        next();
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

    void setup_headers(std::initializer_list<std::string> args)
    {
        if (args.size() == 0)
            throw std::string {"emtpy headers!"};

        // non-empty
        if (!fieldnames.empty())
            fieldnames.clear();

        for (size_type i = 0, sz = args.size(); i != sz; ++i)
        {
            std::string s = (*iter)[i];
            fieldnames[s] = i;
        }
    }

private:
    void setup_headers()
    {
        if (fieldnames.empty() && row_num == 0)
        {
            next();

            for (size_type i = 0, sz = iter->size(); i != iter->size(); ++i)
            {
                std::string s = (*iter)[i];
                fieldnames[s] = i;
            }
        }
    }

    void next()
    {
        std::string s;
        std::getline(is, s);
        *iter = split(s);
        ++row_num;
    }

    // support double quotes
    Row split(std::string& s) const
    {
        Row r = nullptr;
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

    std::ifstream& is;
    bool hasheaders;
    const char delim;
    const char quote;
    size_type row_num;
    Row* iter;
    std::map<std::string, size_type> fieldnames;
};

class Writer {

};


} // namespace miocsv

#endif