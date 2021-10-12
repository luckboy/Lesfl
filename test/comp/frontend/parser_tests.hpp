/****************************************************************************
 *   Copyright (C) 2021 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _PARSER_TESTS_HPP
#define _PARSER_TESTS_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <lesfl/frontend.hpp>

namespace lesfl
{
  namespace frontend
  {
    namespace test
    {
      class ParserTests : public CppUnit::TestFixture
      {
        CPPUNIT_TEST_SUITE(ParserTests);
        CPPUNIT_TEST(test_parser_parses_simple_definitions);
        CPPUNIT_TEST(test_parser_parses_definitions_with_comments);
        CPPUNIT_TEST(test_parser_parses_definition_with_nested_comment);
        CPPUNIT_TEST(test_parser_parses_definitions_which_are_separated_semicolon);
        CPPUNIT_TEST(test_parser_parses_expression_without_space_separations);
        CPPUNIT_TEST(test_parser_parses_definition_with_newline_after_keyword);
        CPPUNIT_TEST(test_parser_parses_expression_with_newline_after_symbol);
        CPPUNIT_TEST(test_parser_parses_expression_with_newlines_in_parenthesis);
        CPPUNIT_TEST(test_parser_parses_qualified_identifiers);
        CPPUNIT_TEST(test_parser_parses_characters);
        CPPUNIT_TEST(test_parser_parses_wide_characters);
        CPPUNIT_TEST(test_parser_parses_integer_numbers);
        CPPUNIT_TEST(test_parser_parses_negative_integer_numbers_in_values);
        CPPUNIT_TEST(test_parser_parses_floating_point_numbers);
        CPPUNIT_TEST(test_parser_parses_infinities);
        CPPUNIT_TEST(test_parser_parses_nans);
        CPPUNIT_TEST(test_parser_parses_negative_floating_point_numbers_in_values);
        CPPUNIT_TEST(test_parser_parses_negative_infinities_in_values);
        CPPUNIT_TEST(test_parser_parses_strings);
        CPPUNIT_TEST(test_parser_parses_wide_strings);
        CPPUNIT_TEST_SUITE_END();

        Parser *_M_parser;
      public:
        void setUp();

        void tearDown();
        
        void test_parser_parses_simple_definitions();
        void test_parser_parses_definitions_with_comments();
        void test_parser_parses_definition_with_nested_comment();
        void test_parser_parses_definitions_which_are_separated_semicolon();
        void test_parser_parses_expression_without_space_separations();
        void test_parser_parses_definition_with_newline_after_keyword();
        void test_parser_parses_expression_with_newline_after_symbol();
        void test_parser_parses_expression_with_newlines_in_parenthesis();
        void test_parser_parses_qualified_identifiers();
        void test_parser_parses_characters();
        void test_parser_parses_wide_characters();
        void test_parser_parses_integer_numbers();
        void test_parser_parses_negative_integer_numbers_in_values();
        void test_parser_parses_floating_point_numbers();
        void test_parser_parses_infinities();
        void test_parser_parses_nans();
        void test_parser_parses_negative_floating_point_numbers_in_values();
        void test_parser_parses_negative_infinities_in_values();
        void test_parser_parses_strings();
        void test_parser_parses_wide_strings();
      };
    }
  }
}

#endif
