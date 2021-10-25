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
        CPPUNIT_TEST(test_parser_parses_imports);
        CPPUNIT_TEST(test_parser_parses_module_definitions);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_definition);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_definition_with_modifier_and_type);
        CPPUNIT_TEST(test_parser_parses_external_variable_definition);
        CPPUNIT_TEST(test_parser_parses_external_variable_definition_with_modifier);
        CPPUNIT_TEST(test_parser_parses_alias_variable_definition);
        CPPUNIT_TEST(test_parser_parses_alias_variable_definition_with_modifier_and_type);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_template_definition);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_template_definition_with_modifier_and_type_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_template_definition_without_value);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_template_definition_without_value_with_modifier_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_alias_variable_template_definition);
        CPPUNIT_TEST(test_parser_parses_alias_variable_template_definition_with_modifier_and_type_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_instance_definition);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_instance_definition_with_type);
        CPPUNIT_TEST(test_parser_parses_external_variable_instance_definition);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_instance_template_definition);
        CPPUNIT_TEST(test_parser_parses_user_defined_variable_instance_template_definition_with_type);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_definition);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_definition_with_annotations_and_modifiers_and_types);
        CPPUNIT_TEST(test_parser_parses_external_function_definition);
        CPPUNIT_TEST(test_parser_parses_external_function_definition_with_modifiers);
        CPPUNIT_TEST(test_parser_parses_native_function_definition);
        CPPUNIT_TEST(test_parser_parses_native_function_definition_with_annotations_and_modifiers);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_template_definition);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_template_definition_with_annotations_and_modifiers_and_types_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_template_definition_without_body);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_template_definition_without_body_with_modifiers_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_instance_definition);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_instance_definition_with_annotations_and_modifiers_and_types);
        CPPUNIT_TEST(test_parser_parses_external_function_instance_definition);
        CPPUNIT_TEST(test_parser_parses_external_function_instance_definition_with_modifiers);
        CPPUNIT_TEST(test_parser_parses_native_function_instance_definition);
        CPPUNIT_TEST(test_parser_parses_native_function_instance_definition_with_annotations_and_modifiers);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_instance_template_definition);
        CPPUNIT_TEST(test_parser_parses_user_defined_function_instance_template_definition_with_annotations_and_modifiers_and_types);
        CPPUNIT_TEST(test_parser_parses_type_synonym_variable_definition);
        CPPUNIT_TEST(test_parser_parses_type_synonym_variable_definition_with_modifier);
        CPPUNIT_TEST(test_parser_parses_datatype_variable_definition);
        CPPUNIT_TEST(test_parser_parses_datatype_variable_definition_with_modifier);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_variable_definition);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_variable_definition_with_modifier);
        CPPUNIT_TEST(test_parser_parses_datatype_variable_definition_without_constructors);
        CPPUNIT_TEST(test_parser_parses_datatype_variable_definition_without_constructors_with_modifier);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_variable_definition_without_constructors);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_variable_definition_without_constructors_with_modifier);
        CPPUNIT_TEST(test_parser_parses_type_synonym_function_definition);
        CPPUNIT_TEST(test_parser_parses_type_synonym_function_definition_with_modifier_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_type_synonym_function_definition_without_body);
        CPPUNIT_TEST(test_parser_parses_type_synonym_function_definition_without_body_with_modifier_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_datatype_function_definition);
        CPPUNIT_TEST(test_parser_parses_datatype_function_definition_with_modifier_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_function_definition);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_function_definition_with_modifier_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_datatype_function_definition_without_constructors);
        CPPUNIT_TEST(test_parser_parses_datatype_function_definition_without_constructors_with_modifier_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_function_definition_without_constructors);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_function_definition_without_constructors_with_modifier_and_instance_type_parameters);
        CPPUNIT_TEST(test_parser_parses_type_synonym_instance_definition);
        CPPUNIT_TEST(test_parser_parses_datatype_instance_definition);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_instance_definition);
        CPPUNIT_TEST(test_parser_parses_datatype_instance_definition_without_constructors);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_instance_definition_without_constructors);
        CPPUNIT_TEST(test_parser_parses_type_synonym_instance_template_definition);
        CPPUNIT_TEST(test_parser_parses_datatype_instance_template_definition);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_instance_template_definition);
        CPPUNIT_TEST(test_parser_parses_datatype_instance_template_definition_without_constructors);
        CPPUNIT_TEST(test_parser_parses_unique_datatype_instance_template_definition_without_constructors);
        CPPUNIT_TEST(test_parser_parses_arguments_without_types);
        CPPUNIT_TEST(test_parser_parses_arguments_with_types);
        CPPUNIT_TEST(test_parser_parses_typed_arguments);
        CPPUNIT_TEST(test_parser_parses_arguments_without_types_for_binary_operator);
        CPPUNIT_TEST(test_parser_parses_arguments_with_types_for_binary_operator);
        CPPUNIT_TEST(test_parser_parses_typed_arguments_for_binary_operator);
        CPPUNIT_TEST(test_parser_parses_argument_without_type_for_unary_operator);
        CPPUNIT_TEST(test_parser_parses_argument_with_type_for_unary_operator);
        CPPUNIT_TEST(test_parser_parses_typed_argument_for_unary_operator);
        CPPUNIT_TEST(test_parser_parses_annotations);
        CPPUNIT_TEST(test_parser_parses_if_expression);
        CPPUNIT_TEST(test_parser_parses_if_expression_with_nested_if_expressions);
        CPPUNIT_TEST(test_parser_parses_let_expression);
        CPPUNIT_TEST(test_parser_parses_let_expression_with_nested_let_expressions);
        CPPUNIT_TEST(test_parser_parses_match_expression);
        CPPUNIT_TEST(test_parser_parses_match_expression_with_nested_match_expression);
        CPPUNIT_TEST(test_parser_parses_throw_expression);
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
        void test_parser_parses_imports();
        void test_parser_parses_module_definitions();
        void test_parser_parses_user_defined_variable_definition();
        void test_parser_parses_user_defined_variable_definition_with_modifier_and_type();
        void test_parser_parses_external_variable_definition();
        void test_parser_parses_external_variable_definition_with_modifier();
        void test_parser_parses_alias_variable_definition();
        void test_parser_parses_alias_variable_definition_with_modifier_and_type();
        void test_parser_parses_user_defined_variable_template_definition();
        void test_parser_parses_user_defined_variable_template_definition_with_modifier_and_type_and_instance_type_parameters();
        void test_parser_parses_user_defined_variable_template_definition_without_value();
        void test_parser_parses_user_defined_variable_template_definition_without_value_with_modifier_and_instance_type_parameters();
        void test_parser_parses_alias_variable_template_definition();
        void test_parser_parses_alias_variable_template_definition_with_modifier_and_type_and_instance_type_parameters();
        void test_parser_parses_user_defined_variable_instance_definition();
        void test_parser_parses_user_defined_variable_instance_definition_with_type();
        void test_parser_parses_external_variable_instance_definition();
        void test_parser_parses_user_defined_variable_instance_template_definition();
        void test_parser_parses_user_defined_variable_instance_template_definition_with_type();
        void test_parser_parses_user_defined_function_definition();
        void test_parser_parses_user_defined_function_definition_with_annotations_and_modifiers_and_types();
        void test_parser_parses_external_function_definition();
        void test_parser_parses_external_function_definition_with_modifiers();
        void test_parser_parses_native_function_definition();
        void test_parser_parses_native_function_definition_with_annotations_and_modifiers();
        void test_parser_parses_user_defined_function_template_definition();
        void test_parser_parses_user_defined_function_template_definition_with_annotations_and_modifiers_and_types_and_instance_type_parameters();
        void test_parser_parses_user_defined_function_template_definition_without_body();
        void test_parser_parses_user_defined_function_template_definition_without_body_with_modifiers_and_instance_type_parameters();
        void test_parser_parses_user_defined_function_instance_definition();
        void test_parser_parses_user_defined_function_instance_definition_with_annotations_and_modifiers_and_types();
        void test_parser_parses_external_function_instance_definition();
        void test_parser_parses_external_function_instance_definition_with_modifiers();
        void test_parser_parses_native_function_instance_definition();
        void test_parser_parses_native_function_instance_definition_with_annotations_and_modifiers();
        void test_parser_parses_user_defined_function_instance_template_definition();
        void test_parser_parses_user_defined_function_instance_template_definition_with_annotations_and_modifiers_and_types();
        void test_parser_parses_type_synonym_variable_definition();
        void test_parser_parses_type_synonym_variable_definition_with_modifier();
        void test_parser_parses_datatype_variable_definition();
        void test_parser_parses_datatype_variable_definition_with_modifier();
        void test_parser_parses_unique_datatype_variable_definition();
        void test_parser_parses_unique_datatype_variable_definition_with_modifier();
        void test_parser_parses_datatype_variable_definition_without_constructors();
        void test_parser_parses_datatype_variable_definition_without_constructors_with_modifier();
        void test_parser_parses_unique_datatype_variable_definition_without_constructors();
        void test_parser_parses_unique_datatype_variable_definition_without_constructors_with_modifier();
        void test_parser_parses_type_synonym_function_definition();
        void test_parser_parses_type_synonym_function_definition_with_modifier_and_instance_type_parameters();
        void test_parser_parses_type_synonym_function_definition_without_body();
        void test_parser_parses_type_synonym_function_definition_without_body_with_modifier_and_instance_type_parameters();
        void test_parser_parses_datatype_function_definition();
        void test_parser_parses_datatype_function_definition_with_modifier_and_instance_type_parameters();
        void test_parser_parses_unique_datatype_function_definition();
        void test_parser_parses_unique_datatype_function_definition_with_modifier_and_instance_type_parameters();
        void test_parser_parses_datatype_function_definition_without_constructors();
        void test_parser_parses_datatype_function_definition_without_constructors_with_modifier_and_instance_type_parameters();
        void test_parser_parses_unique_datatype_function_definition_without_constructors();
        void test_parser_parses_unique_datatype_function_definition_without_constructors_with_modifier_and_instance_type_parameters();
        void test_parser_parses_type_synonym_instance_definition();
        void test_parser_parses_datatype_instance_definition();
        void test_parser_parses_unique_datatype_instance_definition();
        void test_parser_parses_datatype_instance_definition_without_constructors();
        void test_parser_parses_unique_datatype_instance_definition_without_constructors();
        void test_parser_parses_type_synonym_instance_template_definition();
        void test_parser_parses_datatype_instance_template_definition();
        void test_parser_parses_unique_datatype_instance_template_definition();
        void test_parser_parses_datatype_instance_template_definition_without_constructors();
        void test_parser_parses_unique_datatype_instance_template_definition_without_constructors();
        void test_parser_parses_arguments_without_types();
        void test_parser_parses_arguments_with_types();
        void test_parser_parses_typed_arguments();
        void test_parser_parses_arguments_without_types_for_binary_operator();
        void test_parser_parses_arguments_with_types_for_binary_operator();
        void test_parser_parses_typed_arguments_for_binary_operator();
        void test_parser_parses_argument_without_type_for_unary_operator();
        void test_parser_parses_argument_with_type_for_unary_operator();
        void test_parser_parses_typed_argument_for_unary_operator();
        void test_parser_parses_annotations();
        void test_parser_parses_if_expression();
        void test_parser_parses_if_expression_with_nested_if_expressions();
        void test_parser_parses_let_expression();
        void test_parser_parses_let_expression_with_nested_let_expressions();
        void test_parser_parses_match_expression();
        void test_parser_parses_match_expression_with_nested_match_expression();
        void test_parser_parses_throw_expression();
      };
    }
  }
}

#endif
