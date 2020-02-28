//          Copyright Joakim Karlsson & Kim Gräsman 2010-2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef SNOWHOUSE_ASSERTIONEXCEPTION_H
#define SNOWHOUSE_ASSERTIONEXCEPTION_H

#include <stdexcept>
#include <string>

#include "macros.h"

namespace snowhouse
{
  struct AssertionException : public std::runtime_error
  {
    explicit AssertionException(const std::string& message, const std::string& file, unsigned int line)
        : std::runtime_error(message), m_file(file), m_line(line)
    {
    }

    explicit AssertionException(const std::string& message)
        : AssertionException(message, "", 0)
    {
    }

    AssertionException(const AssertionException&) = default;

    std::string file() const
    {
      return m_file;
    }

    unsigned int line() const
    {
      return m_line;
    }

  private:
    std::string m_file;
    unsigned int m_line;
  };
}

#endif
