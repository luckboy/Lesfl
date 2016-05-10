/****************************************************************************
 *   Copyright (C) 2016 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _FRONTEND_DRIVER_HPP
#define _FRONTEND_DRIVER_HPP

#include <lesfl/frontend/tree.hpp>
#include <lesfl/comp.hpp>

namespace lesfl
{
  namespace frontend
  {
    namespace priv
    {
      class Driver
      {
        const Source &_M_source;
        Tree &_M_tree;
        std::list<Error> &_M_errors;
      public:
        Driver(const Source &source, Tree &tree, std::list<Error> &errors) :
          _M_source(source), _M_tree(tree), _M_errors(errors) {}

        const Source &source() const { return _M_source; }

        void add_defs(const std::list<std::unique_ptr<Definition>> *defs)
        { _M_tree.add_defs(defs); }

        void add_error(const Error &error) { _M_errors.push_back(error); }
      };
    }
  }
}

#endif
