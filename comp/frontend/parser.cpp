/****************************************************************************
 *   Copyright (C) 2016, 2021 Łukasz Szpakowski.                            *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#include <lesfl/comp.hpp>
#include <lesfl/frontend.hpp>
#include "frontend/driver.hpp"
#include "frontend/lexer.hpp"
#include "frontend/bison_parser.hpp"

using namespace std;
using namespace lesfl::frontend::priv;

namespace lesfl
{
  namespace frontend
  {
    Parser::~Parser() {}

    bool Parser::parse(const vector<Source> &sources, Tree &tree, list<Error> &errors)
    {
      bool is_success = true;
      for(auto &source : sources) {
        SourceStream ss = source.open();
        if(ss.istream().good()) {
          Driver driver(source, tree, errors);
          Lexer lexer(&(ss.istream()));
          BisonParser parser(driver, lexer);
          try {
            is_success &= (parser.parse() == 0);
          } catch(BisonParser::syntax_error &e) {
            driver.add_error(Error(Position(driver.source(), e.location.begin.line, e.location.begin.column), e.what()));
            is_success = false;
          }
        } else {
          errors.push_back(Error(Position(source, 1, 1), "can't open file"));
          is_success = false;
        }
      }
      return is_success;
    }
  }
}
