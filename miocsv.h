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
    using size_type = size_t;
    
    Row()=default;
    // Row()=delete;

    // pure string input
    Row(std::initializer_list<std::string> args)
    {
        for (const auto& s: args)
            record.emplace_back(s);
    }
    
    template<typename T, typename... Args>
    Row(const T& t, const Args&... args)
    {
        convert_to_string(t, args);
    }

    ~Row()=default;

    const std::string operator [] (size_type i) const 
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
        std::string str = std::move(os.str());
        record.emplace_back(str);
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
    using size_type = size_t;
    Reader()=delete;

    Reader(const std::string& fname_, const char delim_ = ',') 
        : fname {fname_}, delim {delim_}, f {fname}
    {
    }

    Reader(const std::string&& fname_, const char delim_ = ',') 
        : fname {fname_}, delim {delim_}, f {fname}
    {
    }

    ~Reader()=default;

private:
    void read_headers();

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
                else if (*i == delim && !quoted)
                {
                    if (i > b)
                    {
                        str2 = std::string(b, i);
                        r->append(str2);
                    }
                    else
                        r->append(str1);

                    b = i + 1;
                }
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

class Writer {

};


} // namespace miocsv

#endif