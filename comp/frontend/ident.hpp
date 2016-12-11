/****************************************************************************
 *   Copyright (C) 2016 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _FRONTEND_IDENT_HPP
#define _FRONTEND_IDENT_HPP

#include <string>

namespace lesfl
{
  namespace frontend
  {
    namespace priv
    {
      std::string ident_to_string(const std::string &ident);
    }
  }
}

#endif
