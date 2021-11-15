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
      };
    }
  }
}

#endif
