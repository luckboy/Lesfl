/****************************************************************************
 *   Copyright (C) 2021 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#include <algorithm>
#include <memory>
#include "frontend/abs_ident_table_tests.hpp"

using namespace std;

namespace lesfl
{
  namespace frontend
  {
    namespace test
    {
      CPPUNIT_TEST_SUITE_REGISTRATION(AbsoluteIdentifierTableTests);

      void AbsoluteIdentifierTableTests::setUp()
      { _M_abs_ident_table = new AbsoluteIdentifierTable(); }

      void AbsoluteIdentifierTableTests::tearDown()
      { delete _M_abs_ident_table; }
      
      void AbsoluteIdentifierTableTests::test_absolute_identifier_table_add_ident_method_adds_identifiers()
      {
        unique_ptr<AbsoluteIdentifier> abs_ident;
        KeyIdentifier key_ident;
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module1", "fun1" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(0) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module2", "fun2" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(1) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "fun3" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(2) == key_ident);
        const AbsoluteIdentifier *tmp_abs_ident;
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(0));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(0) == tmp_abs_ident->key_ident());
        list<string> expected_idents1 { "module1", "fun1" };
        CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(1));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents2 { "module2", "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(2));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == tmp_abs_ident->key_ident());
        list<string> expected_idents3 { "fun3" };
        CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), tmp_abs_ident->idents().begin()));
      }
      
      void AbsoluteIdentifierTableTests::test_absolute_identifier_table_add_ident_method_does_not_add_identifier()
      {
        unique_ptr<AbsoluteIdentifier> abs_ident;
        KeyIdentifier key_ident;
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "fun1" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(0) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module2", "fun2" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(1) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "fun1" }));
        bool is_added = _M_abs_ident_table->add_ident(abs_ident.get(), key_ident);
        if(is_added) abs_ident.release();
        CPPUNIT_ASSERT_EQUAL(false, is_added);
        const AbsoluteIdentifier *tmp_abs_ident;
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(0));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(0) == tmp_abs_ident->key_ident());
        list<string> expected_idents1 { "fun1" };
        CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(1));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents2 { "module2", "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), tmp_abs_ident->idents().begin()));
      }
      
      void AbsoluteIdentifierTableTests::test_absolute_idnetifier_table_ident_method_returns_null_pointer_for_key_identifier()
      {
        unique_ptr<AbsoluteIdentifier> abs_ident;
        KeyIdentifier key_ident;
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module1", "fun1" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(0) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module2", "fun2" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(1) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module3", "fun3" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(2) == key_ident);
        const AbsoluteIdentifier *tmp_abs_ident;
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(0));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(0) == tmp_abs_ident->key_ident());
        list<string> expected_idents1 { "module1", "fun1" };
        CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(1));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents2 { "module2", "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(2));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == tmp_abs_ident->key_ident());
        list<string> expected_idents3 { "module3", "fun3" };
        CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(3));
        CPPUNIT_ASSERT(nullptr == tmp_abs_ident);
      }
      
      void AbsoluteIdentifierTableTests::test_absolute_idnetifier_table_ident_method_returns_identifier_for_identifier_without_key_identifier()
      {
        unique_ptr<AbsoluteIdentifier> abs_ident;
        KeyIdentifier key_ident;
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module1", "fun1" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(0) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "fun2" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(1) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module3", "fun3" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(2) == key_ident);
        const AbsoluteIdentifier *tmp_abs_ident;
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(0));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(0) == tmp_abs_ident->key_ident());
        list<string> expected_idents1 { "module1", "fun1" };
        CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(1));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents2 { "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(2));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == tmp_abs_ident->key_ident());
        list<string> expected_idents3 { "module3", "fun3" };
        CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), tmp_abs_ident->idents().begin()));
        unique_ptr<AbsoluteIdentifier> orig_abs_ident(new AbsoluteIdentifier(list<string> { "fun2" }));
        tmp_abs_ident = _M_abs_ident_table->ident(orig_abs_ident.get());
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents4 { "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), tmp_abs_ident->idents().begin()));
      }
      
      void AbsoluteIdentifierTableTests::test_absolute_idnetifier_table_ident_method_returns_identifier_for_identifier_with_key_identifier()
      {
        unique_ptr<AbsoluteIdentifier> abs_ident;
        KeyIdentifier key_ident;
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module1", "fun1" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(0) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module2", "fun2" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(1) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module3", "fun3" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(2) == key_ident);
        const AbsoluteIdentifier *tmp_abs_ident;
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(0));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(0) == tmp_abs_ident->key_ident());
        list<string> expected_idents1 { "module1", "fun1" };
        CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(1));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents2 { "module2", "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(2));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == tmp_abs_ident->key_ident());
        list<string> expected_idents3 { "module3", "fun3" };
        CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), tmp_abs_ident->idents().begin()));
        unique_ptr<AbsoluteIdentifier> orig_abs_ident(new AbsoluteIdentifier(list<string> { "module3", "fun3" }));
        orig_abs_ident->set_key_ident(KeyIdentifier(2));
        tmp_abs_ident = _M_abs_ident_table->ident(orig_abs_ident.get());
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == tmp_abs_ident->key_ident());
        list<string> expected_idents4 { "module3", "fun3" };
        CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), tmp_abs_ident->idents().begin()));
      }

      void AbsoluteIdentifierTableTests::test_absolute_idnetifier_table_ident_method_returns_null_pointer_for_identifier_without_key_identifier()
      {
        unique_ptr<AbsoluteIdentifier> abs_ident;
        KeyIdentifier key_ident;
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module1", "fun1" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(0) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "fun2" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(1) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module3", "fun3" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(2) == key_ident);
        const AbsoluteIdentifier *tmp_abs_ident;
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(0));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(0) == tmp_abs_ident->key_ident());
        list<string> expected_idents1 { "module1", "fun1" };
        CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(1));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents2 { "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(2));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == tmp_abs_ident->key_ident());
        list<string> expected_idents3 { "module3", "fun3" };
        CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), tmp_abs_ident->idents().begin()));
        unique_ptr<AbsoluteIdentifier> orig_abs_ident(new AbsoluteIdentifier(list<string> { "fun4" }));
        tmp_abs_ident = _M_abs_ident_table->ident(orig_abs_ident.get());
        CPPUNIT_ASSERT(nullptr == tmp_abs_ident);
      }
      
      void AbsoluteIdentifierTableTests::test_absolute_idnetifier_table_ident_method_returns_null_pointer_for_identifier_with_key_identifier()
      {
        unique_ptr<AbsoluteIdentifier> abs_ident;
        KeyIdentifier key_ident;
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module1", "fun1" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(0) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module2", "fun2" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(1) == key_ident);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module3", "fun3" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident(abs_ident.get(), key_ident));
        abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(2) == key_ident);
        const AbsoluteIdentifier *tmp_abs_ident;
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(0));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(0) == tmp_abs_ident->key_ident());
        list<string> expected_idents1 { "module1", "fun1" };
        CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(1));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents2 { "module2", "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(2));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == tmp_abs_ident->key_ident());
        list<string> expected_idents3 { "module3", "fun3" };
        CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), tmp_abs_ident->idents().begin()));
        unique_ptr<AbsoluteIdentifier> orig_abs_ident(new AbsoluteIdentifier(list<string> { "fun4" }));
        orig_abs_ident->set_key_ident(KeyIdentifier(3));
        tmp_abs_ident = _M_abs_ident_table->ident(orig_abs_ident.get());
        CPPUNIT_ASSERT(nullptr == tmp_abs_ident);
      }
      
      void AbsoluteIdentifierTableTests::test_absolute_identifier_table_add_ident_or_get_key_ident_method_adds_identifiers()
      {
        unique_ptr<AbsoluteIdentifier> abs_ident;
        KeyIdentifier key_ident;
        bool is_added;
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module1", "fun1" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added));
        if(is_added) abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(0) == key_ident);
        CPPUNIT_ASSERT_EQUAL(true, is_added);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module2", "fun2" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added));
        if(is_added) abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(1) == key_ident);
        CPPUNIT_ASSERT_EQUAL(true, is_added);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "fun3" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added));
        if(is_added) abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(2) == key_ident);
        CPPUNIT_ASSERT_EQUAL(true, is_added);
        const AbsoluteIdentifier *tmp_abs_ident;
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(0));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(0) == tmp_abs_ident->key_ident());
        list<string> expected_idents1 { "module1", "fun1" };
        CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(1));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents2 { "module2", "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(2));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == tmp_abs_ident->key_ident());
        list<string> expected_idents3 { "fun3" };
        CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), tmp_abs_ident->idents().begin()));
      }
      
      void AbsoluteIdentifierTableTests::test_absolute_identifier_table_add_ident_or_get_key_ident_method_gets_key_identifier()
      {
        unique_ptr<AbsoluteIdentifier> abs_ident;
        KeyIdentifier key_ident;
        bool is_added;
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module1", "fun1" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added));
        if(is_added) abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(0) == key_ident);
        CPPUNIT_ASSERT_EQUAL(true, is_added);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module2", "fun2" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added));
        if(is_added) abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(1) == key_ident);
        CPPUNIT_ASSERT_EQUAL(true, is_added);
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module3", "fun3" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added));
        if(is_added) abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(2) == key_ident);
        CPPUNIT_ASSERT_EQUAL(true, is_added);
        const AbsoluteIdentifier *tmp_abs_ident;
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(0));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(0) == tmp_abs_ident->key_ident());
        list<string> expected_idents1 { "module1", "fun1" };
        CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(1));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(1) == tmp_abs_ident->key_ident());
        list<string> expected_idents2 { "module2", "fun2" };
        CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), tmp_abs_ident->idents().begin()));
        tmp_abs_ident = _M_abs_ident_table->ident(KeyIdentifier(2));
        CPPUNIT_ASSERT(nullptr != tmp_abs_ident);
        CPPUNIT_ASSERT_EQUAL(true, tmp_abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == tmp_abs_ident->key_ident());
        list<string> expected_idents3 { "module3", "fun3" };
        CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), tmp_abs_ident->idents().size());
        CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), tmp_abs_ident->idents().begin()));
        abs_ident = unique_ptr<AbsoluteIdentifier>(new AbsoluteIdentifier(list<string> { "module3", "fun3" }));
        CPPUNIT_ASSERT_EQUAL(true, _M_abs_ident_table->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added));
        CPPUNIT_ASSERT_EQUAL(true, abs_ident->has_key_ident());
        CPPUNIT_ASSERT(KeyIdentifier(2) == abs_ident->key_ident());
        if(is_added) abs_ident.release();
        CPPUNIT_ASSERT(KeyIdentifier(2) == key_ident);
        CPPUNIT_ASSERT_EQUAL(false, is_added);
      }
    }
  }
}
