/* Copyright 2020-2022 Wuping Xin
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _MIO_STRING_READER_H_
#define _MIO_STRING_READER_H_

#include <mio/mio.hpp>

#ifdef __GNUC__
#define semi_branch_expect(x, y) __builtin_expect(x, y)
#else
#define semi_branch_expect(x, y) x
#endif

namespace mio {

/**
   A fast line reader based on memory mapped file. It is ~ 15x faster than
   std::getline.

   Example:

     std::filesystem::path file_path = std::filesystem::current_path()/"test.txt";
     assert(std::filesystem::exists(file_path));
     mio::StringReader reader(file_path.string());

     if (reader.is_mapped()) {
       auto line = reader.getline();
       while (!line.empty()) {
         counter++;
         line = reader.getline();
       }
     }
 */
class StringReader
{
public:

  /**
     Constructs a reader to read from a disk file line by line. If the
     specified file does not exist, std::system_error will be thrown with
     error code describing the nature of the error.

     \param   a_file  The file to read. Must exists.
   */
  [[maybe_unused]] explicit StringReader(const std::string &a_file) : m_mmap{a_file}
  {
    m_begin = m_mmap.begin();
  }

  explicit StringReader(const std::string &&a_file) : m_mmap{a_file}
  {
    m_begin = m_mmap.begin();
  }

  StringReader() = delete;
  StringReader(const StringReader &) = delete;
  StringReader(StringReader &&) = delete;
  StringReader &operator=(StringReader &) = delete;
  StringReader &operator=(StringReader &&) = delete;

  ~StringReader()
  {
    m_mmap.unmap();
  }

  /**
     Checks whether the reader has reached end of file.

     \returns True if end of line, false otherwise.
   */
  [[maybe_unused]] [[nodiscard]] bool eof() const noexcept
  {
    return (m_begin == nullptr);
  }

  /**
   Checks whether the reader has successfully mapped the underlying file.
   Only on mapped file can getline be called.

   \returns True if mapped, false otherwise.
 */
  [[nodiscard]] inline bool is_mapped() const noexcept
  {
    return m_mmap.is_mapped();
  }

  /**
     Reads a new line into a string view.
     Precondition - StringReader::is_mapped() must be true.

     \returns A std::string_view, nullptr if the reader has reached end of file.
   */
  std::string_view getline() noexcept
  {
    const char *l_begin = m_begin;
    const char *l_find = std::find(l_begin, m_mmap.end(), '\n');

    // Typically, l_find == m_mmap.end() happens only once
    // at the end of file. The majority of the processing will be
    // for l_find != m_mmap.end(). So we give this hint to the compiler
    // for better branch prediction.
    if (semi_branch_expect((l_find != m_mmap.end()), true))
      m_begin = std::next(l_find);
    else
      m_begin = nullptr;

    return {l_begin, static_cast<size_t>(l_find - l_begin)};
  }

private:
  mmap_source m_mmap;
  const char *m_begin;
};

}

#endif