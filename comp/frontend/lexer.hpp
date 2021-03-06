/****************************************************************************
 *   Copyright (C) 2016 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _FRONTEND_LEXER_HPP
#define _FRONTEND_LEXER_HPP

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer     LesflFrontendPrivFlexLexer
#include <FlexLexer.h>
#endif
#include "frontend/bison_parser.hpp"

namespace lesfl
{
  namespace frontend
  {
    namespace priv
    {
      class Lexer : public LesflFrontendPrivFlexLexer
      {
        typedef BisonParser::syntax_error syntax_error;
        typedef BisonParser::token token;

        BisonParser::semantic_type *value;
        BisonParser::location_type *loc;
        std::string buffer;
      public:
        Lexer(std::istream *is) : LesflFrontendPrivFlexLexer(is) {}

        virtual ~Lexer();

        int lex(BisonParser::semantic_type *value, BisonParser::location_type *loc)
        {
          this->value = value;
          this->loc = loc;
          return yylex();
        }

        void set_state_for_name_fields();
      private:
        int yylex();
      };
    }
  }
}

#endif
