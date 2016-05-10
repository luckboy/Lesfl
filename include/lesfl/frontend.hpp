/****************************************************************************
 *   Copyright (C) 2016 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _LESFL_FRONTEND_HPP
#define _LESFL_FRONTEND_HPP

#include <lesfl/frontend/tree.hpp>
#include <lesfl/comp.hpp>

namespace lesfl
{
  namespace frontend
  {
    class Parser
    {
    public:
      Parser() {}

      virtual ~Parser();

      Tree *parse(const std::vector<Source> &sources, std::list<Error> &errors);

      Tree *parse(const Source &source, std::list<Error> &errors)
      { return parse(std::vector<Source> { source }, errors); }
    };
  }
}

#endif
