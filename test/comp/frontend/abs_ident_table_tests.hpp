/****************************************************************************
 *   Copyright (C) 2021 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _FRONTEND_ABS_IDENT_TABLE_TESTS_HPP
#define _FRONTEND_ABS_IDENT_TABLE_TESTS_HPP

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestAssert.h>
#include <lesfl/frontend/ident.hpp>

namespace lesfl
{
  namespace frontend
  {
    namespace test
    {
      class AbsoluteIdentifierTableTests : public CppUnit::TestFixture
      {
        CPPUNIT_TEST_SUITE(AbsoluteIdentifierTableTests);
        CPPUNIT_TEST(test_absolute_identifier_table_add_ident_method_adds_identifiers);
        CPPUNIT_TEST(test_absolute_identifier_table_add_ident_method_does_not_add_identifier);
        CPPUNIT_TEST(test_absolute_idnetifier_table_ident_method_returns_null_pointer_for_key_identifier);
        CPPUNIT_TEST(test_absolute_idnetifier_table_ident_method_returns_identifier_for_identifier_without_key_identifier);
        CPPUNIT_TEST(test_absolute_idnetifier_table_ident_method_returns_identifier_for_identifier_with_key_identifier);
        CPPUNIT_TEST(test_absolute_idnetifier_table_ident_method_returns_null_pointer_for_identifier_without_key_identifier);
        CPPUNIT_TEST(test_absolute_idnetifier_table_ident_method_returns_null_pointer_for_identifier_with_key_identifier);
        CPPUNIT_TEST(test_absolute_identifier_table_add_ident_or_get_key_ident_method_adds_identifiers);
        CPPUNIT_TEST(test_absolute_identifier_table_add_ident_or_get_key_ident_method_gets_key_identifier);
        CPPUNIT_TEST_SUITE_END();

        AbsoluteIdentifierTable *_M_abs_ident_table;
      public:
        void setUp();

        void tearDown();
        
        void test_absolute_identifier_table_add_ident_method_adds_identifiers();
        void test_absolute_identifier_table_add_ident_method_does_not_add_identifier();
        void test_absolute_idnetifier_table_ident_method_returns_null_pointer_for_key_identifier();
        void test_absolute_idnetifier_table_ident_method_returns_identifier_for_identifier_without_key_identifier();
        void test_absolute_idnetifier_table_ident_method_returns_identifier_for_identifier_with_key_identifier();
        void test_absolute_idnetifier_table_ident_method_returns_null_pointer_for_identifier_without_key_identifier();
        void test_absolute_idnetifier_table_ident_method_returns_null_pointer_for_identifier_with_key_identifier();
        void test_absolute_identifier_table_add_ident_or_get_key_ident_method_adds_identifiers();
        void test_absolute_identifier_table_add_ident_or_get_key_ident_method_gets_key_identifier();        
      };
    }
  }
}

#endif
