/****************************************************************************
 *   Copyright (C) 2016, 2021 Łukasz Szpakowski.                            *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _LESFL_FRONTEND_STRING_HPP
#define _LESFL_FRONTEND_STRING_HPP

#include <string>

namespace lesfl
{
  namespace frontend
  {
    class Stringable
    {
    protected:
      Stringable() {}
    public:
      virtual ~Stringable();

      virtual std::string to_string() const = 0;
    };
  }
}

#endif
