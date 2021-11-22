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
      };
    }
  }
}

#endif
