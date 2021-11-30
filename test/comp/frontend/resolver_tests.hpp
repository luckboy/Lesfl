/****************************************************************************
 *   Copyright (C) 2021 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _FRONTEND_RESOLVER_TESTS_HPP
#define _FRONTEND_RESOLVER_TESTS_HPP

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
      class ResolverTests : public CppUnit::TestFixture
      {
        CPPUNIT_TEST_SUITE(ResolverTests);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_simple_definitions);
        CPPUNIT_TEST(test_resolver_adds_modules);
        CPPUNIT_TEST(test_resolver_adds_user_defined_variable);
        CPPUNIT_TEST(test_resolver_adds_user_defined_variable_with_access_modifier_and_type);
        CPPUNIT_TEST(test_resolver_adds_external_variable);
        CPPUNIT_TEST(test_resolver_adds_external_variable_with_access_modifier);
        CPPUNIT_TEST(test_resolver_adds_alias_variable);
        CPPUNIT_TEST(test_resolver_adds_alias_variable_with_access_modifier_and_type);
        CPPUNIT_TEST(test_resolver_adds_user_defined_variable_template);
        CPPUNIT_TEST(test_resolver_adds_user_defined_variable_template_with_access_modifier_and_type_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_user_defined_variable_template_without_value);
        CPPUNIT_TEST(test_resolver_adds_user_defined_variable_template_without_value_with_access_modifier_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_alias_variable_template);
        CPPUNIT_TEST(test_resolver_adds_alias_variable_template_with_access_modifier_and_type_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_user_defined_function);
        CPPUNIT_TEST(test_resolver_adds_user_defined_function_with_access_modifier_and_types);
        CPPUNIT_TEST(test_resolver_adds_external_function);
        CPPUNIT_TEST(test_resolver_adds_external_function_with_access_modifier);
        CPPUNIT_TEST(test_resolver_adds_native_function);
        CPPUNIT_TEST(test_resolver_adds_native_function_with_access_modifier);
        CPPUNIT_TEST(test_resolver_adds_user_defined_function_template);
        CPPUNIT_TEST(test_resolver_adds_user_defined_function_template_with_access_modifier_and_types_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_user_defined_function_template_without_body);
        CPPUNIT_TEST(test_resolver_adds_user_defined_function_template_without_body_with_access_modifier_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_type_synonym_variable);
        CPPUNIT_TEST(test_resolver_adds_type_synonym_variable_with_access_modifier);
        CPPUNIT_TEST(test_resolver_adds_datatype_variable);
        CPPUNIT_TEST(test_resolver_adds_datatype_variable_with_access_modifier);
        CPPUNIT_TEST(test_resolver_adds_unique_datatype_variable);
        CPPUNIT_TEST(test_resolver_adds_unique_datatype_variable_with_access_modifier);
        CPPUNIT_TEST(test_resolver_adds_datatype_variable_without_constructors);
        CPPUNIT_TEST(test_resolver_adds_datatype_variable_without_constructors_with_access_modifier);
        CPPUNIT_TEST(test_resolver_adds_unique_datatype_variable_without_constructors);
        CPPUNIT_TEST(test_resolver_adds_unique_datatype_variable_without_constructors_with_access_modifier);
        CPPUNIT_TEST(test_resolver_adds_type_synonym_function);
        CPPUNIT_TEST(test_resolver_adds_type_synonym_function_with_access_modifier_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_type_synonym_function_without_body);
        CPPUNIT_TEST(test_resolver_adds_type_synonym_function_without_body_with_access_modifier_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_datatype_function);
        CPPUNIT_TEST(test_resolver_adds_datatype_function_with_access_modifier_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_unique_datatype_function);
        CPPUNIT_TEST(test_resolver_adds_unique_datatype_function_with_access_modifier_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_datatype_function_without_constructors);
        CPPUNIT_TEST(test_resolver_adds_datatype_function_without_constructors_with_access_modifier_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_unique_datatype_function_without_constructors);
        CPPUNIT_TEST(test_resolver_adds_unique_datatype_function_without_constrcutors_with_access_modifier_and_instance_type_parameter);
        CPPUNIT_TEST(test_resolver_adds_constructors);
        CPPUNIT_TEST(test_resolver_adds_function_constructors_for_unique_datatype);
        CPPUNIT_TEST(test_resolver_adds_public_constructor_for_public_datatype);
        CPPUNIT_TEST(test_resolver_adds_private_constructor_for_public_datatype);
        CPPUNIT_TEST(test_resolver_adds_public_constructor_for_private_datatype);
        CPPUNIT_TEST(test_resolver_adds_private_constructor_for_private_datatype);
        CPPUNIT_TEST(test_resolver_adds_variable_constructor);
        CPPUNIT_TEST(test_resolver_adds_function_constructor);
        CPPUNIT_TEST(test_resolver_adds_named_field_constructor);
        CPPUNIT_TEST(test_resolver_adds_constructor_in_type_function_instance_where_access_modifier_is_not_updated);
        CPPUNIT_TEST(test_resolver_adds_public_constructor_in_type_function_instance_for_public_datatype);
        CPPUNIT_TEST(test_resolver_adds_private_constructor_in_type_function_instance_for_public_datatype);
        CPPUNIT_TEST(test_resolver_adds_public_constructor_in_type_function_instance_for_private_datatype);
        CPPUNIT_TEST(test_resolver_adds_private_constructor_in_type_function_instance_for_private_datatype);
        CPPUNIT_TEST(test_resolver_resolves_variable_absolute_identifiers);
        CPPUNIT_TEST(test_resolver_resolves_variable_relative_identifiers_for_current_module);
        CPPUNIT_TEST(test_resolver_resolves_variable_relative_identifiers_for_imported_modules);
        CPPUNIT_TEST(test_resolver_resolves_variable_relative_identifiers_for_predef_module);
        CPPUNIT_TEST(test_resolver_resolves_variable_absolute_identifier_for_private_variable);
        CPPUNIT_TEST(test_resolver_resolves_variable_relative_identifier_for_private_variable);
        CPPUNIT_TEST(test_resolver_resolves_type_variable_absolute_identifiers);
        CPPUNIT_TEST(test_resolver_resolves_type_variable_relative_identifiers_for_current_module);
        CPPUNIT_TEST(test_resolver_resolves_type_variable_relative_identifiers_for_imported_modules);
        CPPUNIT_TEST(test_resolver_resolves_type_variable_relative_identifiers_for_predef_module);
        CPPUNIT_TEST(test_resolver_resolves_type_variable_absolute_identifier_for_private_type_variable);
        CPPUNIT_TEST(test_resolver_resolves_type_variable_relative_identifier_for_private_type_variable);
        CPPUNIT_TEST(test_resolver_resolves_type_function_absolute_identifiers);
        CPPUNIT_TEST(test_resolver_resolves_type_function_relative_identifiers_for_current_module);
        CPPUNIT_TEST(test_resolver_resolves_type_function_relative_identifiers_for_imported_modules);
        CPPUNIT_TEST(test_resolver_resolves_type_function_relative_identifiers_for_predef_module);
        CPPUNIT_TEST(test_resolver_resolves_type_function_absolute_identifier_for_private_type_function);
        CPPUNIT_TEST(test_resolver_resolves_type_function_relative_identifier_for_private_type_function);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_arguments);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_arguments_and_covered_global_variable);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_local_variables_and_let_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_local_variables_and_let_expression_and_tuple_binding);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_covered_local_variables_and_let_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_covered_local_variables_and_nested_let_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_used_local_variables_in_other_variables_and_let_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_local_variable_and_let_expression_and_recursive_lambda);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_local_variables_and_match_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_covered_local_variables_and_match_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_covered_local_variables_and_nested_match_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_local_variables_and_lambda_value);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_covered_local_variables_and_lambda_value);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_covered_local_variables_and_nested_lambda_value);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_covered_local_variables_and_primitive_lambda_value);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_type_arguments);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_type_parameters_and_variable_template);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_for_type_parameters_and_function_template);
        CPPUNIT_TEST(test_resolver_checks_eager_annotation);
        CPPUNIT_TEST(test_resolver_checks_lazy_annotation);
        CPPUNIT_TEST(test_resolver_checks_memoized_annotation);
        CPPUNIT_TEST(test_resolver_checks_unmemoized_annotation);
        CPPUNIT_TEST(test_resolver_checks_eager_annotation_and_memoized_annotation);
        CPPUNIT_TEST(test_resolver_checks_eager_annotation_and_unmemoized_annotation);
        CPPUNIT_TEST(test_resolver_checks_lazy_annotation_and_memoized_annotation);
        CPPUNIT_TEST(test_resolver_checks_lazy_annotation_and_unmemoized_annotation);
        CPPUNIT_TEST(test_resolver_checks_onlyeager_annotation);
        CPPUNIT_TEST(test_resolver_checks_onlylazy_annotation);
        CPPUNIT_TEST(test_resolver_checks_onlymemoized_annotation);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_let_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_match_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_throw_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_typed_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_field_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_unique_field_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_set_unique_field_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_named_field_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_unique_named_field_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_set_unique_named_field_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_application);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_primitive_application);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_unique_application);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_builtin_application);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_literal);
        CPPUNIT_TEST(test_resolver_resolves_identifier_from_variable_expression);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_list);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_array);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_unique_array);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_tuple);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_unique_tuple);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_named_field_constructor_application);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_typed_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifier_from_variable_constructor_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_function_constructor_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_named_field_constructor_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_list_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_array_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_unique_array_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_tuple_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_unique_tuple_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_literal_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifier_from_variable_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_as_pattern);
        CPPUNIT_TEST(test_resolver_resolves_identifiers_from_wildcard_pattern);
        CPPUNIT_TEST_SUITE_END();

        BuiltinTypeAdder *_M_builtin_type_adder;
        Parser *_M_parser;
        Resolver *_M_resolver;
      public:
        void setUp();

        void tearDown();
        
        void test_resolver_resolves_identifiers_from_simple_definitions();
        void test_resolver_adds_modules();
        void test_resolver_adds_user_defined_variable();
        void test_resolver_adds_user_defined_variable_with_access_modifier_and_type();
        void test_resolver_adds_external_variable();
        void test_resolver_adds_external_variable_with_access_modifier();
        void test_resolver_adds_alias_variable();
        void test_resolver_adds_alias_variable_with_access_modifier_and_type();
        void test_resolver_adds_user_defined_variable_template();
        void test_resolver_adds_user_defined_variable_template_with_access_modifier_and_type_and_instance_type_parameter();
        void test_resolver_adds_user_defined_variable_template_without_value();
        void test_resolver_adds_user_defined_variable_template_without_value_with_access_modifier_and_instance_type_parameter();
        void test_resolver_adds_alias_variable_template();
        void test_resolver_adds_alias_variable_template_with_access_modifier_and_type_and_instance_type_parameter();
        void test_resolver_adds_user_defined_function();
        void test_resolver_adds_user_defined_function_with_access_modifier_and_types();
        void test_resolver_adds_external_function();
        void test_resolver_adds_external_function_with_access_modifier();
        void test_resolver_adds_native_function();
        void test_resolver_adds_native_function_with_access_modifier();
        void test_resolver_adds_user_defined_function_template();
        void test_resolver_adds_user_defined_function_template_with_access_modifier_and_types_and_instance_type_parameter();
        void test_resolver_adds_user_defined_function_template_without_body();
        void test_resolver_adds_user_defined_function_template_without_body_with_access_modifier_and_instance_type_parameter();
        void test_resolver_adds_type_synonym_variable();
        void test_resolver_adds_type_synonym_variable_with_access_modifier();
        void test_resolver_adds_datatype_variable();
        void test_resolver_adds_datatype_variable_with_access_modifier();
        void test_resolver_adds_unique_datatype_variable();
        void test_resolver_adds_unique_datatype_variable_with_access_modifier();
        void test_resolver_adds_datatype_variable_without_constructors();
        void test_resolver_adds_datatype_variable_without_constructors_with_access_modifier();
        void test_resolver_adds_unique_datatype_variable_without_constructors();
        void test_resolver_adds_unique_datatype_variable_without_constructors_with_access_modifier();
        void test_resolver_adds_type_synonym_function();
        void test_resolver_adds_type_synonym_function_with_access_modifier_and_instance_type_parameter();
        void test_resolver_adds_type_synonym_function_without_body();
        void test_resolver_adds_type_synonym_function_without_body_with_access_modifier_and_instance_type_parameter();
        void test_resolver_adds_datatype_function();
        void test_resolver_adds_datatype_function_with_access_modifier_and_instance_type_parameter();
        void test_resolver_adds_unique_datatype_function();
        void test_resolver_adds_unique_datatype_function_with_access_modifier_and_instance_type_parameter();
        void test_resolver_adds_datatype_function_without_constructors();
        void test_resolver_adds_datatype_function_without_constructors_with_access_modifier_and_instance_type_parameter();
        void test_resolver_adds_unique_datatype_function_without_constructors();
        void test_resolver_adds_unique_datatype_function_without_constrcutors_with_access_modifier_and_instance_type_parameter();
        void test_resolver_adds_constructors();
        void test_resolver_adds_function_constructors_for_unique_datatype();
        void test_resolver_adds_public_constructor_for_public_datatype();
        void test_resolver_adds_private_constructor_for_public_datatype();
        void test_resolver_adds_public_constructor_for_private_datatype();
        void test_resolver_adds_private_constructor_for_private_datatype();
        void test_resolver_adds_variable_constructor();
        void test_resolver_adds_function_constructor();
        void test_resolver_adds_named_field_constructor();
        void test_resolver_adds_constructor_in_type_function_instance_where_access_modifier_is_not_updated();
        void test_resolver_adds_public_constructor_in_type_function_instance_for_public_datatype();
        void test_resolver_adds_private_constructor_in_type_function_instance_for_public_datatype();
        void test_resolver_adds_public_constructor_in_type_function_instance_for_private_datatype();
        void test_resolver_adds_private_constructor_in_type_function_instance_for_private_datatype();
        void test_resolver_resolves_variable_absolute_identifiers();
        void test_resolver_resolves_variable_relative_identifiers_for_current_module();
        void test_resolver_resolves_variable_relative_identifiers_for_imported_modules();
        void test_resolver_resolves_variable_relative_identifiers_for_predef_module();
        void test_resolver_resolves_variable_absolute_identifier_for_private_variable();
        void test_resolver_resolves_variable_relative_identifier_for_private_variable();
        void test_resolver_resolves_type_variable_absolute_identifiers();
        void test_resolver_resolves_type_variable_relative_identifiers_for_current_module();
        void test_resolver_resolves_type_variable_relative_identifiers_for_imported_modules();
        void test_resolver_resolves_type_variable_relative_identifiers_for_predef_module();
        void test_resolver_resolves_type_variable_absolute_identifier_for_private_type_variable();
        void test_resolver_resolves_type_variable_relative_identifier_for_private_type_variable();
        void test_resolver_resolves_type_function_absolute_identifiers();
        void test_resolver_resolves_type_function_relative_identifiers_for_current_module();
        void test_resolver_resolves_type_function_relative_identifiers_for_imported_modules();
        void test_resolver_resolves_type_function_relative_identifiers_for_predef_module();
        void test_resolver_resolves_type_function_absolute_identifier_for_private_type_function();
        void test_resolver_resolves_type_function_relative_identifier_for_private_type_function();
        void test_resolver_resolves_identifiers_for_arguments();
        void test_resolver_resolves_identifiers_for_arguments_and_covered_global_variable();
        void test_resolver_resolves_identifiers_for_local_variables_and_let_expression();
        void test_resolver_resolves_identifiers_for_local_variables_and_let_expression_and_tuple_binding();
        void test_resolver_resolves_identifiers_for_covered_local_variables_and_let_expression();
        void test_resolver_resolves_identifiers_for_covered_local_variables_and_nested_let_expression();
        void test_resolver_resolves_identifiers_for_used_local_variables_in_other_variables_and_let_expression();
        void test_resolver_resolves_identifiers_for_local_variable_and_let_expression_and_recursive_lambda();
        void test_resolver_resolves_identifiers_for_local_variables_and_match_expression();
        void test_resolver_resolves_identifiers_for_covered_local_variables_and_match_expression();
        void test_resolver_resolves_identifiers_for_covered_local_variables_and_nested_match_expression();
        void test_resolver_resolves_identifiers_for_local_variables_and_lambda_value();
        void test_resolver_resolves_identifiers_for_covered_local_variables_and_lambda_value();
        void test_resolver_resolves_identifiers_for_covered_local_variables_and_nested_lambda_value();
        void test_resolver_resolves_identifiers_for_covered_local_variables_and_primitive_lambda_value();
        void test_resolver_resolves_identifiers_for_type_arguments();
        void test_resolver_resolves_identifiers_for_type_parameters_and_variable_template();
        void test_resolver_resolves_identifiers_for_type_parameters_and_function_template();
        void test_resolver_checks_eager_annotation();
        void test_resolver_checks_lazy_annotation();
        void test_resolver_checks_memoized_annotation();
        void test_resolver_checks_unmemoized_annotation();
        void test_resolver_checks_eager_annotation_and_memoized_annotation();
        void test_resolver_checks_eager_annotation_and_unmemoized_annotation();
        void test_resolver_checks_lazy_annotation_and_memoized_annotation();
        void test_resolver_checks_lazy_annotation_and_unmemoized_annotation();
        void test_resolver_checks_onlyeager_annotation();
        void test_resolver_checks_onlylazy_annotation();
        void test_resolver_checks_onlymemoized_annotation();
        void test_resolver_resolves_identifiers_from_let_expression();
        void test_resolver_resolves_identifiers_from_match_expression();
        void test_resolver_resolves_identifiers_from_throw_expression();
        void test_resolver_resolves_identifiers_from_typed_expression();
        void test_resolver_resolves_identifiers_from_field_expression();
        void test_resolver_resolves_identifiers_from_unique_field_expression();
        void test_resolver_resolves_identifiers_from_set_unique_field_expression();
        void test_resolver_resolves_identifiers_from_named_field_expression();
        void test_resolver_resolves_identifiers_from_unique_named_field_expression();
        void test_resolver_resolves_identifiers_from_set_unique_named_field_expression();
        void test_resolver_resolves_identifiers_from_application();
        void test_resolver_resolves_identifiers_from_primitive_application();
        void test_resolver_resolves_identifiers_from_unique_application();
        void test_resolver_resolves_identifiers_from_builtin_application();
        void test_resolver_resolves_identifiers_from_literal();
        void test_resolver_resolves_identifier_from_variable_expression();
        void test_resolver_resolves_identifiers_from_list();
        void test_resolver_resolves_identifiers_from_array();
        void test_resolver_resolves_identifiers_from_unique_array();
        void test_resolver_resolves_identifiers_from_tuple();
        void test_resolver_resolves_identifiers_from_unique_tuple();
        void test_resolver_resolves_identifiers_from_named_field_constructor_application();
        void test_resolver_resolves_identifiers_from_typed_pattern();
        void test_resolver_resolves_identifier_from_variable_constructor_pattern();
        void test_resolver_resolves_identifiers_from_function_constructor_pattern();
        void test_resolver_resolves_identifiers_from_named_field_constructor_pattern();
        void test_resolver_resolves_identifiers_from_list_pattern();
        void test_resolver_resolves_identifiers_from_array_pattern();
        void test_resolver_resolves_identifiers_from_unique_array_pattern();
        void test_resolver_resolves_identifiers_from_tuple_pattern();
        void test_resolver_resolves_identifiers_from_unique_tuple_pattern();
        void test_resolver_resolves_identifiers_from_literal_pattern();
        void test_resolver_resolves_identifier_from_variable_pattern();
        void test_resolver_resolves_identifiers_from_as_pattern();
        void test_resolver_resolves_identifiers_from_wildcard_pattern();
      };
    }
  }
}

#endif
