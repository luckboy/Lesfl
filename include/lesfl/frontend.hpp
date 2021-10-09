/****************************************************************************
 *   Copyright (C) 2016, 2021 Łukasz Szpakowski.                            *
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

      bool parse(const std::vector<Source> &sources, Tree &tree, std::list<Error> &errors);

      bool parse(const Source &source, Tree &tree, std::list<Error> &errors)
      { return parse(std::vector<Source> { source }, tree, errors); }
    };

    class BuiltinTypeAdder
    {
    public:
      BuiltinTypeAdder() {}

      virtual ~BuiltinTypeAdder();
      
      bool add_builtin_types(Tree &tree);
    };
    
    class Resolver
    {
    public:
      Resolver() {}

      virtual ~Resolver();

      bool resolve(Tree &tree, std::list<Error> &errors);
    };
  }
}

#endif
