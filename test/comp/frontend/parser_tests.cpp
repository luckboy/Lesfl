/****************************************************************************
 *   Copyright (C) 2021 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#include <algorithm>
#include <cmath>
#include <sstream>
#include "frontend/parser_tests.hpp"

using namespace std;

namespace lesfl
{
  namespace frontend
  {
    namespace test
    {
      CPPUNIT_TEST_SUITE_REGISTRATION(ParserTests);

      void ParserTests::setUp()
      { _M_parser = new Parser(); }

      void ParserTests::tearDown()
      { delete _M_parser; }
      
      void ParserTests::test_parser_parses_simple_definitions()
      {
        istringstream iss("\
v = 1\n\
\n\
f() = 2\n\
\n\
g(x) = f() + v + x\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app2);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app2->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          NonUniqueApplication *app3 = dynamic_cast<NonUniqueApplication *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != app3);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app3->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "f" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(true, app3->args().empty());
          arg_iter2++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "v" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin())); 
          arg_iter1++;
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), var_expr5->pos().column());
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          list<string> expected_idents5 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), rel_ident5->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_definitions_with_comments()
      {
        istringstream iss("\
// some comment\n\
// second some comment\n\
v =\n\
  /* third some comment\n\
   * some text\n\
   */1\n\
\n\
/* fourth some comment\n\
 * some text\n\
 */\n\
f() =\n\
  // fifth some comment\n\
  2\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value->i());
        }
      }

      void ParserTests::test_parser_parses_definition_with_nested_comment()
      {
        istringstream iss("\
/* some comment\n\
 * /* nested comment */\n\
 */\n\
v = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
      }

      void ParserTests::test_parser_parses_definitions_which_are_separated_semicolon()
      {
        istringstream iss("\
a = 1; b = 2;\n\
c = 3\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("a"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("b"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value->i());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("c"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(3), int_value->i());
        }
      }

      void ParserTests::test_parser_parses_expression_without_space_separations()
      {
        istringstream iss("\
f()=1>=-2+-3\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { ">=" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          Literal *literal2 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal2->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());
          arg_iter1++;
          NonUniqueApplication *app3 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app3);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app3->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app3->args().size());
          auto arg_iter3 = app3->args().begin();
          NonUniqueApplication *app4 = dynamic_cast<NonUniqueApplication *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != app4);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app4->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app4->pos().column());
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(app4->fun());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "unary_-" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app4->args().size());
          auto arg_iter4 = app4->args().begin();
          Literal *literal5 = dynamic_cast<Literal *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal5->pos().column());
          IntValue *int_value5 = dynamic_cast<IntValue *>(literal5->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value5->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value5->i());
          arg_iter3++;
          NonUniqueApplication *app6 = dynamic_cast<NonUniqueApplication *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != app6);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app6->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), app6->pos().column());
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(app6->fun());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr6->pos().column());
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          list<string> expected_idents6 { "unary_-" };
          CPPUNIT_ASSERT_EQUAL(expected_idents6.size(), rel_ident6->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents6.begin(), expected_idents6.end(), rel_ident6->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app6->args().size());
          auto arg_iter6 = app6->args().begin();
          Literal *literal7 = dynamic_cast<Literal *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != literal7);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal7->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal7->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), literal7->pos().column());
          IntValue *int_value7 = dynamic_cast<IntValue *>(literal7->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value7->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(3), int_value7->i());
        }
      }
      
      void ParserTests::test_parser_parses_definition_with_newline_after_keyword()
      {
        istringstream iss("\
inline\n\
f() = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::INLINE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
      }
      
      void ParserTests::test_parser_parses_expression_with_newline_after_symbol()
      {
        istringstream iss("\
f() =\n\
  1 +\n\
  2\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          Literal *literal2 = dynamic_cast<Literal *>(arg_iter1->get());          
          CPPUNIT_ASSERT(nullptr != literal2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), literal2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal2->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());
          arg_iter1++;
          Literal *literal3 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal3->pos().column());
          IntValue *int_value3 = dynamic_cast<IntValue *>(literal3->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value3->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value3->i());
        }
      }
      
      void ParserTests::test_parser_parses_expression_with_newlines_in_parenthesis()
      {
        istringstream iss("\
f() =\n\
  (1\n\
  +\n\
  2)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          Literal *literal2 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), literal2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), literal2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal2->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());
          arg_iter1++;
          Literal *literal3 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), literal3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal3->pos().column());
          IntValue *int_value3 = dynamic_cast<IntValue *>(literal3->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value3->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value3->i());
        }    
      }
      
      void ParserTests::test_parser_parses_qualified_identifiers()
      {
        istringstream iss("\
f() = stdlib.somemodule1.somefun1\n\
\n\
g() = .somelib1.somefun2\n\
\n\
h() = .somelib2.somemodule2.SomeConstr1\n\
\n\
i() = somelib3.SomeConstr2\n\
\n\
j() = somefun3\n\
\n\
k() = .SomeConstr3\n\
\n\
l() = ``::``\n\
\n\
m() = `+`\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "stdlib", "somemodule1", "somefun1" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          AbsoluteIdentifier *abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != abs_ident1);
          list<string> expected_idents1 { "somelib1", "somefun2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), abs_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), abs_ident1->idents().begin())); 
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          AbsoluteIdentifier *abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != abs_ident1);
          list<string> expected_idents1 { "somelib2", "somemodule2", "SomeConstr1" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), abs_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), abs_ident1->idents().begin())); 
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "somelib3", "SomeConstr2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
        }        
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("j"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "somefun3" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
        }        
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("k"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          AbsoluteIdentifier *abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != abs_ident1);
          list<string> expected_idents1 { "SomeConstr3" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), abs_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), abs_ident1->idents().begin())); 
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("l"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "::" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("m"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
        }
      }
      
      void ParserTests::test_parser_parses_characters()
      {
        istringstream iss("\
f() = 'a'\n\
\n\
g() = '\\n'\n\
\n\
h() = '\\5'\n\
\n\
i() = '\\41'\n\
\n\
j() = '\\177'\n\
\n\
k() = '\\xf'\n\
\n\
l() = '\\x7f'\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != char_value);
          CPPUNIT_ASSERT_EQUAL('a', char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != char_value);          
          CPPUNIT_ASSERT_EQUAL('\n', char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != char_value);
          CPPUNIT_ASSERT_EQUAL('\5', char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != char_value);
          CPPUNIT_ASSERT_EQUAL('\41', char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("j"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());          
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != char_value);          
          CPPUNIT_ASSERT_EQUAL('\177', char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("k"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != char_value);          
          CPPUNIT_ASSERT_EQUAL('\17', char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("l"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != char_value);
          CPPUNIT_ASSERT_EQUAL('\177', char_value->c());
        }
      }
      
      void ParserTests::test_parser_parses_wide_characters()
      {
        istringstream iss("\
f() = w'b'\n\
\n\
g() = w'\\r'\n\
\n\
h() = w'\\6'\n\
\n\
i() = w'\\45'\n\
\n\
j() = w'\\177'\n\
\n\
k() = w'\\xf'\n\
\n\
l() = w'\\x7f'\n\
\n\
m() = w'\\u1a2b'\n\
\n\
n() = w'\\U00ab1234'\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_char_value);
          CPPUNIT_ASSERT_EQUAL(L'b', wide_char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_char_value);
          CPPUNIT_ASSERT_EQUAL(L'\r', wide_char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_char_value);
          CPPUNIT_ASSERT_EQUAL(L'\6', wide_char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_char_value);          
          CPPUNIT_ASSERT_EQUAL(L'\45', wide_char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("j"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_char_value);          
          CPPUNIT_ASSERT_EQUAL(L'\177', wide_char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("k"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_char_value);          
          CPPUNIT_ASSERT_EQUAL(L'\17', wide_char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("l"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_char_value);          
          CPPUNIT_ASSERT_EQUAL(L'\177', wide_char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("m"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_char_value);
          CPPUNIT_ASSERT_EQUAL(L'\u1a2b', wide_char_value->c());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("n"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_char_value);
          CPPUNIT_ASSERT_EQUAL(static_cast<wchar_t>(0x00ab1234), wide_char_value->c());
        }
      }
      
      void ParserTests::test_parser_parses_integer_numbers()
      {
        istringstream iss("\
f() = 112i8\n\
\n\
g() = 32712i16\n\
\n\
h() = 2147483612i32\n\
\n\
i() = 9223372036854775801i64\n\
\n\
j() = 9223372036854775802\n\
\n\
k() = 0xfedcba43210\n\
\n\
l() = 01234567\n\
\n\
m() = 0\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT8, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(112), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT16, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(32712), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT32, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2147483612), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(9223372036854775801LL), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("j"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(9223372036854775802LL), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("k"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(0xfedcba43210LL), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("l"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(01234567LL), int_value->i());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("m"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(0), int_value->i());
        }
      }
      
      void ParserTests::test_parser_parses_negative_integer_numbers_in_values()
      {
        istringstream iss("\
a = -101i8\n\
\n\
b = -32701i16\n\
\n\
c = -2147483601i32\n\
\n\
d = -9223372036854775801i64\n\
\n\
e = -102i8 - 1i8\n\
\n\
f = -32702i16 - 1i16\n\
\n\
g = -2147483602i32 - 1i32\n\
\n\
h = -9223372036854775802i64 - 1i64\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("a"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT8, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(-101), int_value->i());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("b"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT16, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(-32701), int_value->i());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("c"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT32, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(-2147483601), int_value->i());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("d"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(-9223372036854775801LL), int_value->i());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("e"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT8, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(-103), int_value->i());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT16, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(-32703), int_value->i());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("g"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT32, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(-2147483603), int_value->i());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("h"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(-9223372036854775803LL), int_value->i());
        }    
      }
      
      void ParserTests::test_parser_parses_floating_point_numbers()
      {
        istringstream iss("\
f() = 1234.56f\n\
\n\
g() = 7890.12d\n\
\n\
h() = 12345.\n\
\n\
i() = 1234.56e10\n\
\n\
j() = 1234.56E+34\n\
\n\
k() = 1234.e-20\n\
\n\
l() = 1234E10\n\
\n\
m() = 1234e-23\n\
\n\
n() = 0.0\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::SINGLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(1234.56, float_value->f());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(7890.12, float_value->f());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(12345.0, float_value->f());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(1234.56e10, float_value->f());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("j"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(1234.56e34, float_value->f());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("k"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(1234.0e-20, float_value->f());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("l"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(1234.0e10, float_value->f());
        }        
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("m"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(1234.0e-23, float_value->f());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("n"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(0.0, float_value->f());
        }
      }

      void ParserTests::test_parser_parses_infinities()
      {
        istringstream iss("\
f() = inff\n\
\n\
g() = infd\n\
\n\
h() = inf\n\
\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::SINGLE, float_value->float_type());
          CPPUNIT_ASSERT(isinf(float_value->f()));
          CPPUNIT_ASSERT(0 < float_value->f());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT(isinf(float_value->f()));
          CPPUNIT_ASSERT(0 < float_value->f());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT(isinf(float_value->f()));
          CPPUNIT_ASSERT(0 < float_value->f());
        }
      }

      void ParserTests::test_parser_parses_nans()
      {
        istringstream iss("\
f() = nanf\n\
\n\
g() = nand\n\
\n\
h() = nan\n\
\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::SINGLE, float_value->float_type());
          CPPUNIT_ASSERT(isnan(float_value->f()));
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT(isnan(float_value->f()));
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT(isnan(float_value->f()));
        }
      }
      
      void ParserTests::test_parser_parses_negative_floating_point_numbers_in_values()
      {
        istringstream iss("\
a = -1234.56f\n\
\n\
b = -7890.12d\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("a"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::SINGLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(-1234.56, float_value->f());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("b"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::DOUBLE, float_value->float_type());
          CPPUNIT_ASSERT_EQUAL(-7890.12, float_value->f());
        }
      }
      
      void ParserTests::test_parser_parses_negative_infinities_in_values()
      {
        istringstream iss("\
a = -inff\n\
\n\
b = -infd\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("a"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT_EQUAL(FloatType::SINGLE, float_value->float_type());
          CPPUNIT_ASSERT(isinf(float_value->f()));
          CPPUNIT_ASSERT(0 > float_value->f());
        }
        def_iter++;
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);          
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("b"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != float_value);
          CPPUNIT_ASSERT(isinf(float_value->f()));
          CPPUNIT_ASSERT(0 > float_value->f());
        }
      }
      
      void ParserTests::test_parser_parses_strings()
      {
        istringstream iss("\
f() = \"ab\\rcd\\nef\"\n\
\n\
g() = \"\\6AB\\45C\\177\\xf\\x7f\"\n\
\n\
h() = \"abc\\n\\\n\
def\\n\\\n\
ghi\"\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          StringValue *string_value = dynamic_cast<StringValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != string_value);
          CPPUNIT_ASSERT_EQUAL(string("ab\rcd\nef"), string_value->string());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);          
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          StringValue *string_value = dynamic_cast<StringValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != string_value);
          CPPUNIT_ASSERT_EQUAL(string("\6AB\45C\177\17\177"), string_value->string());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          StringValue *string_value = dynamic_cast<StringValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != string_value);
          CPPUNIT_ASSERT_EQUAL(string("abc\ndef\nghi"), string_value->string());
        }
      }
      
      void ParserTests::test_parser_parses_wide_strings()
      {
        istringstream iss("\
f() = w\"fe\\rdc\\nba\"\n\
\n\
g() = w\"\\7CB\\46A\\177\\xf\\x7f\"\n\
\n\
h() = w\"ghi\\n\\\n\
def\\n\\\n\
abc\"\n\
\n\
i() = w\"\\uabcd\\U001234ab\"\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideStringValue *wide_string_value = dynamic_cast<WideStringValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_string_value);
          CPPUNIT_ASSERT(wstring(L"fe\rdc\nba") == wide_string_value->string());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideStringValue *wide_string_value = dynamic_cast<WideStringValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_string_value);
          CPPUNIT_ASSERT(wstring(L"\7CB\46A\177\17\177") == wide_string_value->string());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideStringValue *wide_string_value = dynamic_cast<WideStringValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_string_value);
          CPPUNIT_ASSERT(wstring(L"ghi\ndef\nabc") == wide_string_value->string());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideStringValue *wide_string_value = dynamic_cast<WideStringValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != wide_string_value);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), wide_string_value->string().length());
          CPPUNIT_ASSERT_EQUAL(L'\uabcd', wide_string_value->string()[0]);
          CPPUNIT_ASSERT_EQUAL(static_cast<wchar_t>(0x001234ab), wide_string_value->string()[1]);
        }
      }
      
      void ParserTests::test_parser_parses_imports()
      {
        istringstream iss("\
import stdlib.somemodule1\n\
import .somelib.somemodule2\n\
import .;\n\
\n\
f() = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          Import *import = dynamic_cast<Import *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != import);
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(import->module_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "stdlib", "somemodule1" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), import->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), import->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), import->pos().column());
        }
        def_iter++;
        {
          Import *import = dynamic_cast<Import *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != import);
          AbsoluteIdentifier *abs_ident = dynamic_cast<AbsoluteIdentifier *>(import->module_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident);
          list<string> expected_idents { "somelib", "somemodule2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), abs_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), abs_ident->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), import->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), import->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), import->pos().column());
        }
        def_iter++;
        {
          Import *import = dynamic_cast<Import *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != import);
          AbsoluteIdentifier *abs_ident = dynamic_cast<AbsoluteIdentifier *>(import->module_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident);
          CPPUNIT_ASSERT_EQUAL(true, abs_ident->idents().empty());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), import->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), import->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), import->pos().column());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
      }
      
      void ParserTests::test_parser_parses_module_definitions()
      {
        istringstream iss("\
module somelib {\n\
  f() = 1\n\
\n\
  module module1.module2 {\n\
    g() = 2\n\
  }\n\
\n\
  module .somelib2.module3 {\n\
    h() = 3\n\
  }\n\
\n\
  module . {\n\
    i() = 4\n\
  }\n\
}\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          ModuleDefinition *module_def = dynamic_cast<ModuleDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != module_def);
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(module_def->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "somelib" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), module_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), module_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), module_def->pos().column());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), module_def->defs().size());
          auto module_def_iter = module_def->defs().begin();
          {
            FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(module_def_iter->get());
            CPPUNIT_ASSERT(nullptr != fun_def);
            CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
            CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
            CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_def->pos().line());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().column());
            UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
            CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
            CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
            CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
            CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
            CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
            CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
            Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
            CPPUNIT_ASSERT(nullptr != literal);
            CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), literal->pos().line());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().column());
            IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
            CPPUNIT_ASSERT(nullptr != int_value);
            CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
            CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
          }
          module_def_iter++;
          {
            ModuleDefinition *module_def2 = dynamic_cast<ModuleDefinition *>(module_def_iter->get());
            CPPUNIT_ASSERT(nullptr != module_def2);
            RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(module_def2->ident());
            CPPUNIT_ASSERT(nullptr != rel_ident);
            list<string> expected_idents { "module1", "module2" };
            CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
            CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin())); 
            CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), module_def2->pos().source().file_name());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), module_def2->pos().line());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), module_def2->pos().column());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), module_def2->defs().size());
            auto module_def_iter2 = module_def2->defs().begin();
            {
              FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(module_def_iter2->get());
              CPPUNIT_ASSERT(nullptr != fun_def);
              CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
              CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
              CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().column());
              UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
              CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
              CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
              CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
              CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
              CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
              CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
              Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
              CPPUNIT_ASSERT(nullptr != literal);
              CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().column());
              IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
              CPPUNIT_ASSERT(nullptr != int_value);
              CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
              CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value->i());
            }
          }
          module_def_iter++;
          {
            ModuleDefinition *module_def2 = dynamic_cast<ModuleDefinition *>(module_def_iter->get());
            CPPUNIT_ASSERT(nullptr != module_def2);
            AbsoluteIdentifier *abs_ident = dynamic_cast<AbsoluteIdentifier *>(module_def2->ident());
            CPPUNIT_ASSERT(nullptr != abs_ident);
            list<string> expected_idents { "somelib2", "module3" };
            CPPUNIT_ASSERT_EQUAL(expected_idents.size(), abs_ident->idents().size());
            CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), abs_ident->idents().begin())); 
            CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), module_def2->pos().source().file_name());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), module_def2->pos().line());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), module_def2->pos().column());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), module_def2->defs().size());
            auto module_def_iter2 = module_def2->defs().begin();
            {
              FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(module_def_iter2->get());
              CPPUNIT_ASSERT(nullptr != fun_def);
              CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
              CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
              CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), fun_def->pos().line());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().column());
              UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
              CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
              CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
              CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
              CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
              CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
              CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
              Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
              CPPUNIT_ASSERT(nullptr != literal);
              CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().column());
              IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
              CPPUNIT_ASSERT(nullptr != int_value);
              CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
              CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(3), int_value->i());
            }
          }
          module_def_iter++;
          {
            ModuleDefinition *module_def2 = dynamic_cast<ModuleDefinition *>(module_def_iter->get());
            CPPUNIT_ASSERT(nullptr != module_def2);
            AbsoluteIdentifier *abs_ident = dynamic_cast<AbsoluteIdentifier *>(module_def2->ident());
            CPPUNIT_ASSERT(nullptr != abs_ident);
            CPPUNIT_ASSERT_EQUAL(true, abs_ident->idents().empty()); 
            CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), module_def2->pos().source().file_name());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), module_def2->pos().line());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), module_def2->pos().column());
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), module_def2->defs().size());
            auto module_def_iter2 = module_def2->defs().begin();
            {
              FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(module_def_iter2->get());
              CPPUNIT_ASSERT(nullptr != fun_def);
              CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
              CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
              CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), fun_def->pos().line());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().column());
              UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
              CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
              CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
              CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
              CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
              CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
              CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
              Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
              CPPUNIT_ASSERT(nullptr != literal);
              CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
              CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().column());
              IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
              CPPUNIT_ASSERT(nullptr != int_value);
              CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
              CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(4), int_value->i());
            }
          }
        }
      }

      void ParserTests::test_parser_parses_user_defined_variable_definition()
      {
        istringstream iss("\
v = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
      }

      void ParserTests::test_parser_parses_user_defined_variable_definition_with_modifier_and_type()
      {
        istringstream iss("\
private v: Int = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr != user_defined_var->type_expr());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_var_expr->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin())); 
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
      }
      
      void ParserTests::test_parser_parses_external_variable_definition()
      {
        istringstream iss("\
extern v: Int = somevar\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_def->pos().column());
          ExternalVariable *external_var = dynamic_cast<ExternalVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != external_var);
          CPPUNIT_ASSERT_EQUAL(false, external_var->is_template());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(external_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), type_var_expr->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somevar"), external_var->external_var_ident());
        }
      }
      
      void ParserTests::test_parser_parses_external_variable_definition_with_modifier()
      {
        istringstream iss("\
private extern v: Int = somevar\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_def->pos().column());
          ExternalVariable *external_var = dynamic_cast<ExternalVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != external_var);
          CPPUNIT_ASSERT_EQUAL(false, external_var->is_template());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(external_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), type_var_expr->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somevar"), external_var->external_var_ident());
        }
      }
      
      void ParserTests::test_parser_parses_alias_variable_definition()
      {
        istringstream iss("\
v = a\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          AliasVariable *alias_var = dynamic_cast<AliasVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != alias_var);
          CPPUNIT_ASSERT_EQUAL(false, alias_var->is_template());
          CPPUNIT_ASSERT(nullptr == alias_var->type_expr());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(alias_var->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "a" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_alias_variable_definition_with_modifier_and_type()
      {
        istringstream iss("\
private v: T = C\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_def->pos().column());
          AliasVariable *alias_var = dynamic_cast<AliasVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != alias_var);
          CPPUNIT_ASSERT_EQUAL(false, alias_var->is_template());
          CPPUNIT_ASSERT(nullptr != alias_var->type_expr());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(alias_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_var_expr->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin())); 
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(alias_var->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "C" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_user_defined_variable_template_definition()
      {
        istringstream iss("\
template\n\
v = (Nil)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->inst_type_params().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_var->value());
          ConstructorValue *constr_value = dynamic_cast<ConstructorValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != constr_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), constr_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), constr_value->pos().column());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(constr_value->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "Nil" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_user_defined_variable_template_definition_with_modifier_and_type_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private v: T(t, u) = (C)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_var->inst_type_params().size());
          auto inst_type_param_iter = user_defined_var->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());
          CPPUNIT_ASSERT(nullptr != user_defined_var->type_expr());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_app->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app->args().size());
          auto type_arg_iter = type_app->args().begin();
          TypeParameterExpression *type_param_expr1 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), type_param_expr1->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr1->ident());
          type_arg_iter++;
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr2->ident());
          CPPUNIT_ASSERT(nullptr != user_defined_var->value());
          ConstructorValue *constr_value = dynamic_cast<ConstructorValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != constr_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), constr_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), constr_value->pos().column());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(constr_value->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "C" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin()));
        }        
      }
      
      void ParserTests::test_parser_parses_user_defined_variable_template_definition_without_value()
      {
        istringstream iss("\
template\n\
v: List(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->inst_type_params().empty());
          CPPUNIT_ASSERT(nullptr != user_defined_var->type_expr());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), type_app->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "List" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app->args().size());
          auto type_arg_iter = type_app->args().begin();
          TypeParameterExpression *type_param_expr1 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), type_param_expr1->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr1->ident());
          CPPUNIT_ASSERT(nullptr == user_defined_var->value());
        }        
      }

      void ParserTests::test_parser_parses_user_defined_variable_template_definition_without_value_with_modifier_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private v: T(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_var->inst_type_params().size());
          auto inst_type_param_iter = user_defined_var->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());
          CPPUNIT_ASSERT(nullptr != user_defined_var->type_expr());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_app->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app->args().size());
          auto type_arg_iter = type_app->args().begin();
          TypeParameterExpression *type_param_expr1 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), type_param_expr1->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr1->ident());
          type_arg_iter++;
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr2->ident());
          CPPUNIT_ASSERT(nullptr == user_defined_var->value());
        }        
      }
      
      void ParserTests::test_parser_parses_alias_variable_template_definition()
      {
        istringstream iss("\
template\n\
v = a\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          AliasVariable *alias_var = dynamic_cast<AliasVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != alias_var);
          CPPUNIT_ASSERT_EQUAL(true, alias_var->is_template());
          CPPUNIT_ASSERT_EQUAL(true, alias_var->inst_type_params().empty());
          CPPUNIT_ASSERT(nullptr == alias_var->type_expr());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(alias_var->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "a" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_alias_variable_template_definition_with_modifier_and_type_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private v: T(t, u) = C\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableDefinition *var_def = dynamic_cast<VariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_def->pos().column());
          AliasVariable *alias_var = dynamic_cast<AliasVariable *>(var_def->var().get());
          CPPUNIT_ASSERT(nullptr != alias_var);
          CPPUNIT_ASSERT_EQUAL(true, alias_var->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), alias_var->inst_type_params().size());
          auto inst_type_param_iter = alias_var->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());
          CPPUNIT_ASSERT(nullptr != alias_var->type_expr());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(alias_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_app->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app->args().size());
          auto type_arg_iter = type_app->args().begin();
          TypeParameterExpression *type_param_expr1 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), type_param_expr1->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr1->ident());
          type_arg_iter++;
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr2->ident());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(alias_var->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "C" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin()));
        }        
      }

      void ParserTests::test_parser_parses_user_defined_variable_instance_definition()
      {
        istringstream iss("\
instance\n\
v = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableInstanceDefinition *var_inst_def = dynamic_cast<VariableInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("v"), var_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_inst_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_inst_def->var_inst()->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
      }

      void ParserTests::test_parser_parses_user_defined_variable_instance_definition_with_type()
      {
        istringstream iss("\
instance\n\
v: Int = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableInstanceDefinition *var_inst_def = dynamic_cast<VariableInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("v"), var_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_inst_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_inst_def->var_inst()->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(false, user_defined_var->is_template());
          CPPUNIT_ASSERT(nullptr != user_defined_var->type_expr());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), type_var_expr->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin())); 
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_literal_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_literal_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), var_literal_value->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(var_literal_value->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
      }
      
      void ParserTests::test_parser_parses_external_variable_instance_definition()
      {
        istringstream iss("\
instance\n\
extern v: Int = somevar\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableInstanceDefinition *var_inst_def = dynamic_cast<VariableInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("v"), var_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_inst_def->pos().column());
          ExternalVariable *external_var = dynamic_cast<ExternalVariable *>(var_inst_def->var_inst()->var().get());
          CPPUNIT_ASSERT(nullptr != external_var);
          CPPUNIT_ASSERT_EQUAL(false, external_var->is_template());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(external_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), type_var_expr->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somevar"), external_var->external_var_ident());
        }
      }
      
      void ParserTests::test_parser_parses_user_defined_variable_instance_template_definition()
      {
        istringstream iss("\
template instance\n\
v = Nil\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableInstanceDefinition *var_def = dynamic_cast<VariableInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_def);
          CPPUNIT_ASSERT_EQUAL(string("v"), var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_def->var_inst()->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->inst_type_params().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_var->value());
          ConstructorValue *constr_value = dynamic_cast<ConstructorValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != constr_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), constr_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), constr_value->pos().column());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(constr_value->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "Nil" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_user_defined_variable_instance_template_definition_with_type()
      {
        istringstream iss("\
template instance\n\
v: T(t, u) = C\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          VariableInstanceDefinition *var_inst_def = dynamic_cast<VariableInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != var_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("v"), var_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_inst_def->pos().column());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_inst_def->var_inst()->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->inst_type_params().empty());
          CPPUNIT_ASSERT(nullptr != user_defined_var->type_expr());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), type_app->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app->args().size());
          auto type_arg_iter = type_app->args().begin();
          TypeParameterExpression *type_param_expr1 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_param_expr1->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr1->ident());
          type_arg_iter++;
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr2->ident());
          CPPUNIT_ASSERT(nullptr != user_defined_var->value());
          ConstructorValue *constr_value = dynamic_cast<ConstructorValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != constr_value);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), constr_value->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr_value->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), constr_value->pos().column());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(constr_value->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          list<string> expected_idents { "C" };
          CPPUNIT_ASSERT_EQUAL(expected_idents.size(), rel_ident->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents.begin(), expected_idents.end(), rel_ident->idents().begin()));
        }        
      }
      
      void ParserTests::test_parser_parses_user_defined_function_definition()
      {
        istringstream iss("\
f(x, y) = g(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_user_defined_function_definition_with_annotations_and_modifiers_and_types()
      {
        istringstream iss("\
@lazy @memoized\n\
private primitive inline f(x, y: Int8): Int = g(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->annotations().size());
          auto annotation_iter = user_defined_fun->annotations().begin();
          CPPUNIT_ASSERT_EQUAL(string("lazy"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().column());
          annotation_iter++;
          CPPUNIT_ASSERT_EQUAL(string("memoized"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), (*annotation_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::INLINE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(34), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));          
          CPPUNIT_ASSERT(nullptr != user_defined_fun->result_type_expr());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(41), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));          
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(47), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(47), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(49), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(52), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_external_function_definition()
      {
        istringstream iss("\
extern f(x: WChar, y: Char): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), fun_def->pos().column());
          ExternalFunction *external_fun = dynamic_cast<ExternalFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, external_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, external_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, external_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), external_fun->args().size());
          auto arg_iter = external_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Char" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(external_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), external_fun->external_fun_ident());
        }
      }
      
      void ParserTests::test_parser_parses_external_function_definition_with_modifiers()
      {
        istringstream iss("\
private primitive extern f(x: WChar, y: Char): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), fun_def->pos().column());
          ExternalFunction *external_fun = dynamic_cast<ExternalFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, external_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, external_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, external_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), external_fun->args().size());
          auto arg_iter = external_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(38), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(41), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Char" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(external_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(48), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), external_fun->external_fun_ident());
        }
      }

      void ParserTests::test_parser_parses_native_function_definition()
      {
        istringstream iss("\
native f(x: Int8, y: Int16): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), fun_def->pos().column());
          NativeFunction *native_fun = dynamic_cast<NativeFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, native_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, native_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, native_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, native_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), native_fun->args().size());
          auto arg_iter = native_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int16" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(native_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), native_fun->native_fun_ident());
        }
      }
      
      void ParserTests::test_parser_parses_native_function_definition_with_annotations_and_modifiers()
      {
        istringstream iss("\
@memoized @lazy\n\
private inline primitive native f(x: Int8, y: Int16): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(33), fun_def->pos().column());
          NativeFunction *native_fun = dynamic_cast<NativeFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, native_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), native_fun->annotations().size());
          auto annotation_iter = native_fun->annotations().begin();
          CPPUNIT_ASSERT_EQUAL(string("memoized"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().column());
          annotation_iter++;
          CPPUNIT_ASSERT_EQUAL(string("lazy"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), (*annotation_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::INLINE, native_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, native_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), native_fun->args().size());
          auto arg_iter = native_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(35), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(38), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(44), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(47), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int16" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(native_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(55), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), native_fun->native_fun_ident());
        }
      }
      
      void ParserTests::test_parser_parses_user_defined_function_template_definition()
      {
        istringstream iss("\
template\n\
f(x, y) = g(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->body());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_user_defined_function_template_definition_with_annotations_and_modifiers_and_types_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
@lazy @memoized\n\
private inline primitive f(x: T(t, u), y): Int = g(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->inst_type_params().size());
          auto inst_type_param_iter = user_defined_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->annotations().size());
          auto annotation_iter = user_defined_fun->annotations().begin();
          CPPUNIT_ASSERT_EQUAL(string("lazy"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().column());
          annotation_iter++;
          CPPUNIT_ASSERT_EQUAL(string("memoized"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), (*annotation_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::INLINE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->args().size());
          auto type_arg_iter = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(33), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          type_arg_iter++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(36), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr3->ident());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(40), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->result_type_expr());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(44), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT(nullptr != user_defined_fun->body());          
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(50), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(50), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(52), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(55), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_user_defined_function_template_definition_without_body()
      {
        istringstream iss("\
template\n\
f(x: List(t), y: Int8): Int\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "List" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          CPPUNIT_ASSERT(nullptr != user_defined_fun->result_type_expr());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->body());
        }
      }

      void ParserTests::test_parser_parses_user_defined_function_template_definition_without_body_with_modifiers_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private primitive f(x: Int8, y: T(t, u)): Int\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->inst_type_params().size());
          auto inst_type_param_iter = user_defined_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());          
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(24), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeApplication *type_app2 = dynamic_cast<TypeApplication *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(33), type_app2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_app2->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->args().size());
          auto type_arg_iter = type_app2->args().begin();
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(35), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr3->ident());
          type_arg_iter++;
          TypeParameterExpression *type_param_expr4 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(38), type_param_expr4->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr4->ident());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->result_type_expr());
          TypeVariableExpression *type_var_expr5 = dynamic_cast<TypeVariableExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(43), type_var_expr5->pos().column());
          RelativeIdentifier *type_rel_ident5 = dynamic_cast<RelativeIdentifier *>(type_var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident5);
          list<string> expected_type_idents5 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents5.size(), type_rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents5.begin(), expected_type_idents5.end(), type_rel_ident5->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->body());          
        }
      }

      void ParserTests::test_parser_parses_user_defined_function_instance_definition()
      {
        istringstream iss("\
instance\n\
f(x, y) = g(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionInstanceDefinition *fun_inst_def = dynamic_cast<FunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_inst_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_inst_def->fun_inst()->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_user_defined_function_instance_definition_with_annotations_and_modifiers_and_types()
      {
        istringstream iss("\
instance\n\
@lazy @memoized\n\
primitive inline f(x, y: Int8): Int = g(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionInstanceDefinition *fun_inst_def = dynamic_cast<FunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), fun_inst_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_inst_def->fun_inst()->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->annotations().size());
          auto annotation_iter = user_defined_fun->annotations().begin();
          CPPUNIT_ASSERT_EQUAL(string("lazy"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().column());
          annotation_iter++;
          CPPUNIT_ASSERT_EQUAL(string("memoized"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), (*annotation_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::INLINE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));          
          CPPUNIT_ASSERT(nullptr != user_defined_fun->result_type_expr());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(33), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));          
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(39), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(39), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(41), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(44), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }        
      }
      
      void ParserTests::test_parser_parses_external_function_instance_definition()
      {
        istringstream iss("\
instance\n\
extern f(x: WChar, y: Char): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionInstanceDefinition *fun_inst_def = dynamic_cast<FunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), fun_inst_def->pos().column());
          ExternalFunction *external_fun = dynamic_cast<ExternalFunction *>(fun_inst_def->fun_inst()->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, external_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, external_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, external_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), external_fun->args().size());
          auto arg_iter = external_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Char" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(external_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), external_fun->external_fun_ident());
        }
      }
      
      void ParserTests::test_parser_parses_external_function_instance_definition_with_modifiers()
      {
        istringstream iss("\
instance\n\
primitive extern f(x: WChar, y: Char): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionInstanceDefinition *fun_inst_def = dynamic_cast<FunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), fun_inst_def->pos().column());
          ExternalFunction *external_fun = dynamic_cast<ExternalFunction *>(fun_inst_def->fun_inst()->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, external_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, external_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, external_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), external_fun->args().size());
          auto arg_iter = external_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(33), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Char" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(external_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(40), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), external_fun->external_fun_ident());
        }
      }

      void ParserTests::test_parser_parses_native_function_instance_definition()
      {
        istringstream iss("\
instance\n\
native f(x: Int8, y: Int16): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionInstanceDefinition *fun_inst_def = dynamic_cast<FunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), fun_inst_def->pos().column());
          NativeFunction *native_fun = dynamic_cast<NativeFunction *>(fun_inst_def->fun_inst()->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, native_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, native_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, native_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, native_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), native_fun->args().size());
          auto arg_iter = native_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int16" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(native_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), native_fun->native_fun_ident());
        }
      }

      void ParserTests::test_parser_parses_native_function_instance_definition_with_annotations_and_modifiers()
      {
        istringstream iss("\
instance\n\
@memoized @lazy\n\
inline primitive native f(x: Int8, y: Int16): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionInstanceDefinition *fun_inst_def = dynamic_cast<FunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), fun_inst_def->pos().column());
          NativeFunction *native_fun = dynamic_cast<NativeFunction *>(fun_inst_def->fun_inst()->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, native_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), native_fun->annotations().size());
          auto annotation_iter = native_fun->annotations().begin();
          CPPUNIT_ASSERT_EQUAL(string("memoized"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().column());
          annotation_iter++;
          CPPUNIT_ASSERT_EQUAL(string("lazy"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), (*annotation_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::INLINE, native_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, native_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), native_fun->args().size());
          auto arg_iter = native_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(27), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(36), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(39), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int16" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(native_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(47), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), native_fun->native_fun_ident());
        }        
      }

      void ParserTests::test_parser_parses_user_defined_function_instance_template_definition()
      {
        istringstream iss("\
template instance\n\
f(x, y) = g(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionInstanceDefinition *fun_def = dynamic_cast<FunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun_inst()->fun().get());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->body());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_user_defined_function_instance_template_definition_with_annotations_and_modifiers_and_types()
      {
        istringstream iss("\
template instance\n\
@lazy @memoized\n\
inline primitive f(x: T(t, u), y): Int = g(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionInstanceDefinition *fun_inst_def = dynamic_cast<FunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), fun_inst_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_inst_def->fun_inst()->fun().get());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->annotations().size());
          auto annotation_iter = user_defined_fun->annotations().begin();
          CPPUNIT_ASSERT_EQUAL(string("lazy"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().column());
          annotation_iter++;
          CPPUNIT_ASSERT_EQUAL(string("memoized"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), (*annotation_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::INLINE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->args().size());
          auto type_arg_iter = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          type_arg_iter++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr3->ident());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(32), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->result_type_expr());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(36), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT(nullptr != user_defined_fun->body());          
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(42), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(42), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(44), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(47), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_type_synonym_variable_definition()
      {
        istringstream iss("\
type T = Int\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_var_def->pos().column());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_type_synonym_variable_definition_with_modifier()
      {
        istringstream iss("\
private type T = Int\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), type_var_def->pos().column());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_datatype_variable_definition()
      {
        istringstream iss("\
datatype T = C(Int, Int8)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_var_def->pos().column());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          auto constr_iter = datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_datatype_variable_definition_with_modifier()
      {
        istringstream iss("\
private datatype T = C(Int8, Int)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), type_var_def->pos().column());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          auto constr_iter = datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(24), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_unique_datatype_variable_definition()
      {
        istringstream iss("\
unique datatype T = C(Int, WChar)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_var_def->pos().column());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
          auto constr_iter = unique_datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_unique_datatype_variable_definition_with_modifier()
      {
        istringstream iss("\
private unique datatype T = C(WChar, Int)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), type_var_def->pos().column());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
          auto constr_iter = unique_datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(38), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_datatype_variable_definition_without_constructors()
      {
        istringstream iss("\
datatype T\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_var_def->pos().column());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
      }
      
      void ParserTests::test_parser_parses_datatype_variable_definition_without_constructors_with_modifier()
      {
        istringstream iss("\
private datatype T\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), type_var_def->pos().column());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
      }

      void ParserTests::test_parser_parses_unique_datatype_variable_definition_without_constructors()
      {
        istringstream iss("\
unique datatype T\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_var_def->pos().column());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
        }
      }

      void ParserTests::test_parser_parses_unique_datatype_variable_definition_without_constructors_with_modifier()
      {
        istringstream iss("\
private unique datatype T\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeVariableDefinition *type_var_def = dynamic_cast<TypeVariableDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_var_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), type_var_def->pos().column());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_def->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
        }
      }      

      void ParserTests::test_parser_parses_type_synonym_function_definition()
      {
        istringstream iss("\
template\n\
type T(t, u) = U(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_fun_def->pos().column());
          TypeSynonymFunction *type_synonym_fun = dynamic_cast<TypeSynonymFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun);
          CPPUNIT_ASSERT_EQUAL(true, type_synonym_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_synonym_fun->args().size());
          auto type_arg_iter = type_synonym_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun->body());
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(type_synonym_fun->body());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          type_arg_iter1++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr3->ident());
        }
      }
      
      void ParserTests::test_parser_parses_type_synonym_function_definition_with_modifier_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private type T(t, u) = U(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), type_fun_def->pos().column());
          TypeSynonymFunction *type_synonym_fun = dynamic_cast<TypeSynonymFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_synonym_fun->inst_type_params().size());
          auto inst_type_param_iter = type_synonym_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_synonym_fun->args().size());
          auto type_arg_iter = type_synonym_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun->body());
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(type_synonym_fun->body());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(24), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          type_arg_iter1++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(29), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr3->ident());
        }
      }

      void ParserTests::test_parser_parses_type_synonym_function_definition_without_body()
      {
        istringstream iss("\
template\n\
type T(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_fun_def->pos().column());
          TypeSynonymFunction *type_synonym_fun = dynamic_cast<TypeSynonymFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun);
          CPPUNIT_ASSERT_EQUAL(true, type_synonym_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_synonym_fun->args().size());
          auto type_arg_iter = type_synonym_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == type_synonym_fun->body());
        }
      }

      void ParserTests::test_parser_parses_type_synonym_function_definition_without_body_with_modifier_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private type T(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());        
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), type_fun_def->pos().column());
          TypeSynonymFunction *type_synonym_fun = dynamic_cast<TypeSynonymFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_synonym_fun->inst_type_params().size());
          auto inst_type_param_iter = type_synonym_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_synonym_fun->args().size());
          auto type_arg_iter = type_synonym_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == type_synonym_fun->body());
        }
      }
      
      void ParserTests::test_parser_parses_datatype_function_definition()
      {
        istringstream iss("\
template\n\
datatype T(t, u) = C(U(t, u), Int8)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_fun_def->pos().column());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          auto constr_iter = datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(24), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          type_arg_iter1++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(27), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr3->ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), type_var_expr4->pos().column());
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          list<string> expected_type_idents4 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents4.size(), type_rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents4.begin(), expected_type_idents4.end(), type_rel_ident4->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_datatype_function_definition_with_modifier_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private datatype T(t, u) = C(Int8, U(t, u))\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), type_fun_def->pos().column());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->inst_type_params().size());
          auto inst_type_param_iter = datatype_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          auto constr_iter = datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          field_type_iter++;
          TypeApplication *type_app2 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(36), type_app2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_app2->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->args().size());
          auto type_arg_iter2 = type_app2->args().begin();
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(38), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr3->ident());
          type_arg_iter2++;
          TypeParameterExpression *type_param_expr4 = dynamic_cast<TypeParameterExpression *>(type_arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(41), type_param_expr4->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr4->ident());
        }
      }
      
      void ParserTests::test_parser_parses_unique_datatype_function_definition()
      {
        istringstream iss("\
template\n\
unique datatype T(t, u) = C(U(t, u), WChar)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_fun_def->pos().column());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
          auto constr_iter = unique_datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(29), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          type_arg_iter1++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(34), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr3->ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(38), type_var_expr4->pos().column());
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          list<string> expected_type_idents4 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents4.size(), type_rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents4.begin(), expected_type_idents4.end(), type_rel_ident4->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_unique_datatype_function_definition_with_modifier_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private unique datatype T(t, u) = C(WChar, U(t, u))\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), type_fun_def->pos().column());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->inst_type_params().size());
          auto inst_type_param_iter = datatype_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(27), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
          auto constr_iter = unique_datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(37), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          field_type_iter++;
          TypeApplication *type_app2 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(44), type_app2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_app2->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->args().size());
          auto type_arg_iter2 = type_app2->args().begin();
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(46), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr3->ident());
          type_arg_iter2++;
          TypeParameterExpression *type_param_expr4 = dynamic_cast<TypeParameterExpression *>(type_arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(49), type_param_expr4->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr4->ident());
        }
      }
      
      void ParserTests::test_parser_parses_datatype_function_definition_without_constructors()
      {
        istringstream iss("\
template\n\
datatype T(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_fun_def->pos().column());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
      }

      void ParserTests::test_parser_parses_datatype_function_definition_without_constructors_with_modifier_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private datatype T(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), type_fun_def->pos().column());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->inst_type_params().size());
          auto inst_type_param_iter = datatype_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
      }
      
      void ParserTests::test_parser_parses_unique_datatype_function_definition_without_constructors()
      {
        istringstream iss("\
template\n\
unique datatype T(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_fun_def->pos().column());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
        }
      }
      
      void ParserTests::test_parser_parses_unique_datatype_function_definition_without_constructors_with_modifier_and_instance_type_parameters()
      {
        istringstream iss("\
template(t, u)\n\
private unique datatype T(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionDefinition *type_fun_def = dynamic_cast<TypeFunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), type_fun_def->pos().column());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_def->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->inst_type_params().size());
          auto inst_type_param_iter = datatype_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*inst_type_param_iter)->ident());
          inst_type_param_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*inst_type_param_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*inst_type_param_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), (*inst_type_param_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*inst_type_param_iter)->ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(27), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), (*type_arg_iter)->ident());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*type_arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), (*type_arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), (*type_arg_iter)->ident());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
        }
      }
      
      void ParserTests::test_parser_parses_type_synonym_instance_definition()
      {
        istringstream iss("\
instance\n\
type T(Int, Int8) = Int\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_fun_inst_def->pos().column());
          TypeSynonymFunctionInstance *type_synonym_fun_inst = dynamic_cast<TypeSynonymFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun_inst);
          CPPUNIT_ASSERT_EQUAL(false, type_synonym_fun_inst->is_template()); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_synonym_fun_inst->args().size());
          auto type_arg_iter = type_synonym_fun_inst->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          type_arg_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(type_synonym_fun_inst->body());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_datatype_instance_definition()
      {
        istringstream iss("\
instance\n\
datatype T(Int, Int8) = C(Int, Int8)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_fun_inst_def->pos().column());
          DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
          CPPUNIT_ASSERT_EQUAL(false, datatype_fun_inst->is_template()); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2),datatype_fun_inst->args().size());
          auto type_arg_iter = datatype_fun_inst->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          type_arg_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun_inst->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          auto constr_iter = datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(27), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          field_type_iter++;
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(32), type_var_expr4->pos().column());
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          list<string> expected_type_idents4 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents4.size(), type_rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents4.begin(), expected_type_idents4.end(), type_rel_ident4->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_unique_datatype_instance_definition()
      {
        istringstream iss("\
instance\n\
unique datatype T(WChar, Int) = C(WChar, Int)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_fun_inst_def->pos().column());
          DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
          CPPUNIT_ASSERT_EQUAL(false, datatype_fun_inst->is_template()); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2),datatype_fun_inst->args().size());
          auto type_arg_iter = datatype_fun_inst->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          type_arg_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun_inst->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
          auto constr_iter = unique_datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(35), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          field_type_iter++;
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(42), type_var_expr4->pos().column());
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          list<string> expected_type_idents4 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents4.size(), type_rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents4.begin(), expected_type_idents4.end(), type_rel_ident4->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_datatype_instance_definition_without_constructors()
      {
        istringstream iss("\
instance\n\
datatype T(Int, Int8)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_fun_inst_def->pos().column());
          DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
          CPPUNIT_ASSERT_EQUAL(false, datatype_fun_inst->is_template()); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2),datatype_fun_inst->args().size());
          auto type_arg_iter = datatype_fun_inst->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          type_arg_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun_inst->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
      }
      
      void ParserTests::test_parser_parses_unique_datatype_instance_definition_without_constructors()
      {
        istringstream iss("\
instance\n\
unique datatype T(WChar, Int)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_fun_inst_def->pos().column());
          DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
          CPPUNIT_ASSERT_EQUAL(false, datatype_fun_inst->is_template()); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2),datatype_fun_inst->args().size());
          auto type_arg_iter = datatype_fun_inst->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          type_arg_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun_inst->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
        }
      }

      void ParserTests::test_parser_parses_type_synonym_instance_template_definition()
      {
        istringstream iss("\
template instance\n\
type T(U(t, u), Int8) = U(t, u)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_fun_inst_def->pos().column());
          TypeSynonymFunctionInstance *type_synonym_fun_inst = dynamic_cast<TypeSynonymFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun_inst);
          CPPUNIT_ASSERT_EQUAL(true, type_synonym_fun_inst->is_template());
          auto type_arg_iter = type_synonym_fun_inst->args().begin();
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          type_arg_iter1++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr3->ident());
          type_arg_iter++;
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_var_expr4->pos().column());
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          list<string> expected_type_idents4 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents4.size(), type_rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents4.begin(), expected_type_idents4.end(), type_rel_ident4->idents().begin()));
          TypeApplication *type_app5 = dynamic_cast<TypeApplication *>(type_synonym_fun_inst->body());
          CPPUNIT_ASSERT(nullptr != type_app5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), type_app5->pos().column());
          RelativeIdentifier *type_rel_ident5 = dynamic_cast<RelativeIdentifier *>(type_app5->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident5);
          list<string> expected_type_idents5 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents5.size(), type_rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents5.begin(), expected_type_idents5.end(), type_rel_ident5->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app5->args().size());
          auto type_arg_iter5 = type_app5->args().begin();
          TypeParameterExpression *type_param_expr6 = dynamic_cast<TypeParameterExpression *>(type_arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(27), type_param_expr6->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr6->ident());
          type_arg_iter5++;
          TypeParameterExpression *type_param_expr7 = dynamic_cast<TypeParameterExpression *>(type_arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr7);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr7->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr7->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), type_param_expr7->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr7->ident());
        }
      }
      
      void ParserTests::test_parser_parses_datatype_instance_template_definition()
      {
        istringstream iss("\
template instance\n\
datatype T(U(t, u), Int8) = C(U(t, u), Int8)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_fun_inst_def->pos().column());
          DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun_inst->is_template());
          auto type_arg_iter = datatype_fun_inst->args().begin();
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          type_arg_iter1++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr3->ident());
          type_arg_iter++;
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), type_var_expr4->pos().column());
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          list<string> expected_type_idents4 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents4.size(), type_rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents4.begin(), expected_type_idents4.end(), type_rel_ident4->idents().begin()));
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun_inst->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          auto constr_iter = datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeApplication *type_app5 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), type_app5->pos().column());
          RelativeIdentifier *type_rel_ident5 = dynamic_cast<RelativeIdentifier *>(type_app5->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident5);
          list<string> expected_type_idents5 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents5.size(), type_rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents5.begin(), expected_type_idents5.end(), type_rel_ident5->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app5->args().size());
          auto type_arg_iter5 = type_app5->args().begin();
          TypeParameterExpression *type_param_expr6 = dynamic_cast<TypeParameterExpression *>(type_arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(33), type_param_expr6->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr6->ident());
          type_arg_iter5++;
          TypeParameterExpression *type_param_expr7 = dynamic_cast<TypeParameterExpression *>(type_arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr7);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr7->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr7->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(36), type_param_expr7->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr7->ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr8 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr8);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr8->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr8->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(40), type_var_expr8->pos().column());
          RelativeIdentifier *type_rel_ident8 = dynamic_cast<RelativeIdentifier *>(type_var_expr8->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident8);
          list<string> expected_type_idents8 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents8.size(), type_rel_ident8->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents8.begin(), expected_type_idents8.end(), type_rel_ident8->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_unique_datatype_instance_template_definition()
      {
        istringstream iss("\
template instance\n\
unique datatype T(WChar, U(t, u)) = C(WChar, U(t, u))\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_fun_inst_def->pos().column());
          DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun_inst->is_template());
          auto type_arg_iter = datatype_fun_inst->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          type_arg_iter++;
          TypeApplication *type_app2 = dynamic_cast<TypeApplication *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), type_app2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_app2->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->args().size());
          auto type_arg_iter1 = type_app2->args().begin();
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr3->ident());
          type_arg_iter1++;
          TypeParameterExpression *type_param_expr4 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), type_param_expr4->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr4->ident());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun_inst->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
          auto constr_iter = unique_datatype->constrs().begin();
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_iter->get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, constr->access_modifier());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, constr->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(string("C"), constr->ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr5 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(39), type_var_expr5->pos().column());
          RelativeIdentifier *type_rel_ident5 = dynamic_cast<RelativeIdentifier *>(type_var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident5);
          list<string> expected_type_idents5 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents5.size(), type_rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents5.begin(), expected_type_idents5.end(), type_rel_ident5->idents().begin()));
          field_type_iter++;
          TypeApplication *type_app6 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(46), type_app6->pos().column());
          RelativeIdentifier *type_rel_ident6 = dynamic_cast<RelativeIdentifier *>(type_app6->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident6);
          list<string> expected_type_idents6 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents6.size(), type_rel_ident6->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents6.begin(), expected_type_idents6.end(), type_rel_ident6->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app6->args().size());
          auto type_arg_iter6 = type_app6->args().begin();
          TypeParameterExpression *type_param_expr7 = dynamic_cast<TypeParameterExpression *>(type_arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr7);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr7->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr7->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(48), type_param_expr7->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr7->ident());
          type_arg_iter6++;
          TypeParameterExpression *type_param_expr8 = dynamic_cast<TypeParameterExpression *>(type_arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr8);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr8->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr8->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(51), type_param_expr8->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr8->ident());
        }
      }
      
      void ParserTests::test_parser_parses_datatype_instance_template_definition_without_constructors()
      {
        istringstream iss("\
template instance\n\
datatype T(U(t, u), Int8)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_fun_inst_def->pos().column());
          DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun_inst->is_template());
          auto type_arg_iter = datatype_fun_inst->args().begin();
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_app1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), type_param_expr2->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr2->ident());
          type_arg_iter1++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr3->ident());
          type_arg_iter++;
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), type_var_expr4->pos().column());
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          list<string> expected_type_idents4 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents4.size(), type_rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents4.begin(), expected_type_idents4.end(), type_rel_ident4->idents().begin()));
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun_inst->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
      }

      void ParserTests::test_parser_parses_unique_datatype_instance_template_definition_without_constructors()
      {
        istringstream iss("\
template instance\n\
unique datatype T(WChar, U(t, u))\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          TypeFunctionInstanceDefinition *type_fun_inst_def = dynamic_cast<TypeFunctionInstanceDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != type_fun_inst_def);
          CPPUNIT_ASSERT_EQUAL(string("T"), type_fun_inst_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_fun_inst_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_fun_inst_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_fun_inst_def->pos().column());
          DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(type_fun_inst_def->fun_inst().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun_inst->is_template());
          auto type_arg_iter = datatype_fun_inst->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "WChar" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          type_arg_iter++;
          TypeApplication *type_app2 = dynamic_cast<TypeApplication *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), type_app2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_app2->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_app2->args().size());
          auto type_arg_iter1 = type_app2->args().begin();
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), type_param_expr3->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("t"), type_param_expr3->ident());
          type_arg_iter1++;
          TypeParameterExpression *type_param_expr4 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_param_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), type_param_expr4->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("u"), type_param_expr4->ident());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun_inst->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
        }
      }
      
      void ParserTests::test_parser_parses_arguments_without_types()
      {
        istringstream iss("\
f(x, y, z) = g(x, y, z)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("z"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_arguments_with_types()
      {
        istringstream iss("\
f(x: Int8, y: Int16, z: Int) = g(x, y, z)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));          
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int16" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));          
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("z"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(25), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));          
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(32), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(32), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(34), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(37), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(40), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin()));
        }
      }

      void ParserTests::test_parser_parses_typed_arguments()
      {
        istringstream iss("\
native f(x: Int8, y: Int16, z: Int): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), fun_def->pos().column());
          NativeFunction *native_fun = dynamic_cast<NativeFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, native_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, native_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, native_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, native_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), native_fun->args().size());
          auto arg_iter = native_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int16" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(29), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("z"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(32), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(native_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(38), type_var_expr4->pos().column());
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          list<string> expected_type_idents4 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents4.size(), type_rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents4.begin(), expected_type_idents4.end(), type_rel_ident4->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), native_fun->native_fun_ident());
        }
      }
      
      void ParserTests::test_parser_parses_arguments_without_types_for_binary_operator()
      {
        istringstream iss("\
x + y = f(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("+"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "f" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }        
      }
      
      void ParserTests::test_parser_parses_arguments_with_types_for_binary_operator()
      {
        istringstream iss("\
(x: Int8) + (y: Int16) = f(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("+"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));          
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int16" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));          
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "f" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_typed_arguments_for_binary_operator()
      {
        istringstream iss("\
native (x: Int8) + (y: Int16): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("+"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), fun_def->pos().column());
          NativeFunction *native_fun = dynamic_cast<NativeFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, native_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, native_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, native_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, native_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), native_fun->args().size());
          auto arg_iter = native_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          arg_iter++;
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("y"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(24), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int16" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(native_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(32), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), native_fun->native_fun_ident());
        }
      }
      
      void ParserTests::test_parser_parses_argument_without_type_for_unary_operator()
      {
        istringstream iss("\
-x = f(x)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("unary_-"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "f" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
        }        
      }
      
      void ParserTests::test_parser_parses_argument_with_type_for_unary_operator()
      {
        istringstream iss("\
-(x: Int8) = f(x)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("unary_-"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));          
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "f" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
        }
      }
      
      void ParserTests::test_parser_parses_typed_argument_for_unary_operator()
      {
        istringstream iss("\
native -(x: Int8): Int = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());
          CPPUNIT_ASSERT_EQUAL(string("unary_-"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), fun_def->pos().column());
          NativeFunction *native_fun = dynamic_cast<NativeFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, native_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, native_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, native_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, native_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), native_fun->args().size());
          auto arg_iter = native_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "Int8" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(native_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), type_var_expr2->pos().column());
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          list<string> expected_type_idents2 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents2.size(), type_rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents2.begin(), expected_type_idents2.end(), type_rel_ident2->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(string("somefun"), native_fun->native_fun_ident());
        }
      }
      
      void ParserTests::test_parser_parses_annotations()
      {
        istringstream iss("\
@lazy\n\
@memoized\n\
f() = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->annotations().size());
          auto annotation_iter = user_defined_fun->annotations().begin();
          CPPUNIT_ASSERT_EQUAL(string("lazy"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().column());
          annotation_iter++;
          CPPUNIT_ASSERT_EQUAL(string("memoized"), (*annotation_iter)->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*annotation_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*annotation_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value->i());
        }
      }

      void ParserTests::test_parser_parses_if_expression()
      {
        istringstream iss("\
f() =\n\
  if(True)\n\
    x + 1\n\
  else\n\
    2\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), match1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), match1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "True" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          VariableConstructorPattern *var_constr_pattern2 = dynamic_cast<VariableConstructorPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_constr_pattern2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_constr_pattern2->pos().column());
          AbsoluteIdentifier *abs_ident2 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern2->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident2);
          list<string> expected_idents2 { "stdlib", "True" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), abs_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), abs_ident2->idents().begin())); 
          NonUniqueApplication *app3 = dynamic_cast<NonUniqueApplication *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != app3);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app3->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app3->args().size());
          auto arg_iter3 = app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin())); 
          arg_iter3++;
          Literal *literal5 = dynamic_cast<Literal *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal5->pos().column());
          IntValue *int_value5 = dynamic_cast<IntValue *>(literal5->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value5);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value5->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value5->i());
          case_iter1++;
          VariableConstructorPattern *var_constr_pattern5 = dynamic_cast<VariableConstructorPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_constr_pattern5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_constr_pattern5->pos().column());
          AbsoluteIdentifier *abs_ident5 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern5->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident5);
          list<string> expected_idents5 { "stdlib", "False" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), abs_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), abs_ident5->idents().begin())); 
          Literal *literal6 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal6->pos().column());
          IntValue *int_value6 = dynamic_cast<IntValue *>(literal6->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value6);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value6->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value6->i());
        }
      }
      
      void ParserTests::test_parser_parses_if_expression_with_nested_if_expressions()
      {
        istringstream iss("\
f() =\n\
  if(if(x) True else False)\n\
    if(y) 1 else 2\n\
  else\n\
    if(z) 3 else 4\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), match1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), match1->pos().column());
          Match *match2 = dynamic_cast<Match *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != match2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), match2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), match2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(match2->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match2->cases().size());
          auto case_iter2 = match2->cases().begin();
          VariableConstructorPattern *var_constr_pattern3 = dynamic_cast<VariableConstructorPattern *>((*case_iter2)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_constr_pattern3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), var_constr_pattern3->pos().column());
          AbsoluteIdentifier *abs_ident3 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern3->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident3);
          list<string> expected_idents3 { "stdlib", "True" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), abs_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), abs_ident3->idents().begin())); 
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>((*case_iter2)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "True" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin())); 
          case_iter2++;
          VariableConstructorPattern *var_constr_pattern5 = dynamic_cast<VariableConstructorPattern *>((*case_iter2)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_constr_pattern5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), var_constr_pattern5->pos().column());
          AbsoluteIdentifier *abs_ident5 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern5->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident5);
          list<string> expected_idents5 { "stdlib", "False" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), abs_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), abs_ident5->idents().begin())); 
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>((*case_iter2)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr6->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(22), var_expr6->pos().column());
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          list<string> expected_idents6 { "False" };
          CPPUNIT_ASSERT_EQUAL(expected_idents6.size(), rel_ident6->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents6.begin(), expected_idents6.end(), rel_ident6->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          VariableConstructorPattern *var_constr_pattern7 = dynamic_cast<VariableConstructorPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern7->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_constr_pattern7->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_constr_pattern7->pos().column());
          AbsoluteIdentifier *abs_ident7 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern7->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident7);
          list<string> expected_idents7 { "stdlib", "True" };
          CPPUNIT_ASSERT_EQUAL(expected_idents7.size(), abs_ident7->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents7.begin(), expected_idents7.end(), abs_ident7->idents().begin())); 
          Match *match8 = dynamic_cast<Match *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != match8);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), match8->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), match8->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), match8->pos().column());
          VariableExpression *var_expr8 = dynamic_cast<VariableExpression *>(match8->expr());
          CPPUNIT_ASSERT(nullptr != var_expr8);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr8->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr8->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_expr8->pos().column());
          RelativeIdentifier *rel_ident8 = dynamic_cast<RelativeIdentifier *>(var_expr8->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident8);
          list<string> expected_idents8 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents8.size(), rel_ident8->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents8.begin(), expected_idents8.end(), rel_ident8->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match8->cases().size());
          auto case_iter8 = match8->cases().begin();
          VariableConstructorPattern *var_constr_pattern9 = dynamic_cast<VariableConstructorPattern *>((*case_iter8)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern9->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_constr_pattern9->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_constr_pattern9->pos().column());
          AbsoluteIdentifier *abs_ident9 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern9->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident9);
          list<string> expected_idents9 { "stdlib", "True" };
          CPPUNIT_ASSERT_EQUAL(expected_idents9.size(), abs_ident9->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents9.begin(), expected_idents9.end(), abs_ident9->idents().begin())); 
          Literal *literal10 = dynamic_cast<Literal *>((*case_iter8)->expr());
          CPPUNIT_ASSERT(nullptr != literal10);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal10->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal10->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal10->pos().column());
          IntValue *int_value10 = dynamic_cast<IntValue *>(literal10->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value10);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value10->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value10->i());
          case_iter8++;
          VariableConstructorPattern *var_constr_pattern11 = dynamic_cast<VariableConstructorPattern *>((*case_iter8)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern11->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_constr_pattern11->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_constr_pattern11->pos().column());
          AbsoluteIdentifier *abs_ident11 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern11->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident11);
          list<string> expected_idents11 { "stdlib", "False" };
          CPPUNIT_ASSERT_EQUAL(expected_idents11.size(), abs_ident11->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents11.begin(), expected_idents11.end(), abs_ident11->idents().begin())); 
          Literal *literal12 = dynamic_cast<Literal *>((*case_iter8)->expr());
          CPPUNIT_ASSERT(nullptr != literal12);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal12->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal12->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), literal12->pos().column());
          IntValue *int_value12 = dynamic_cast<IntValue *>(literal12->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value12);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value12->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value12->i());
          case_iter1++;
          VariableConstructorPattern *var_constr_pattern13 = dynamic_cast<VariableConstructorPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern13->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_constr_pattern13->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_constr_pattern13->pos().column());
          AbsoluteIdentifier *abs_ident13 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern13->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident13);
          list<string> expected_idents13 { "stdlib", "False" };
          CPPUNIT_ASSERT_EQUAL(expected_idents13.size(), abs_ident13->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents13.begin(), expected_idents13.end(), abs_ident13->idents().begin())); 
          Match *match14 = dynamic_cast<Match *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != match14);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), match14->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), match14->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), match14->pos().column());
          VariableExpression *var_expr14 = dynamic_cast<VariableExpression *>(match14->expr());
          CPPUNIT_ASSERT(nullptr != var_expr14);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr14->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr14->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_expr14->pos().column());
          RelativeIdentifier *rel_ident14 = dynamic_cast<RelativeIdentifier *>(var_expr14->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident14);
          list<string> expected_idents14 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents14.size(), rel_ident14->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents14.begin(), expected_idents14.end(), rel_ident14->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match14->cases().size());
          auto case_iter14 = match14->cases().begin();
          VariableConstructorPattern *var_constr_pattern15 = dynamic_cast<VariableConstructorPattern *>((*case_iter14)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern15->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_constr_pattern15->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_constr_pattern15->pos().column());
          AbsoluteIdentifier *abs_ident15 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern15->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident15);
          list<string> expected_idents15 { "stdlib", "True" };
          CPPUNIT_ASSERT_EQUAL(expected_idents15.size(), abs_ident15->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents15.begin(), expected_idents15.end(), abs_ident15->idents().begin())); 
          Literal *literal16 = dynamic_cast<Literal *>((*case_iter14)->expr());
          CPPUNIT_ASSERT(nullptr != literal16);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal16->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal16->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal16->pos().column());
          IntValue *int_value16 = dynamic_cast<IntValue *>(literal16->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value16);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value16->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(3), int_value16->i());
          case_iter14++;
          VariableConstructorPattern *var_constr_pattern17 = dynamic_cast<VariableConstructorPattern *>((*case_iter14)->pattern());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_constr_pattern17->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_constr_pattern17->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_constr_pattern17->pos().column());
          AbsoluteIdentifier *abs_ident17 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern17->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident17);
          list<string> expected_idents17 { "stdlib", "False" };
          CPPUNIT_ASSERT_EQUAL(expected_idents17.size(), abs_ident17->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents17.begin(), expected_idents17.end(), abs_ident17->idents().begin())); 
          Literal *literal18 = dynamic_cast<Literal *>((*case_iter14)->expr());
          CPPUNIT_ASSERT(nullptr != literal18);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal18->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal18->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), literal18->pos().column());
          IntValue *int_value18 = dynamic_cast<IntValue *>(literal18->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value18);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value18->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(4), int_value18->i());          
        }
      }
      
      void ParserTests::test_parser_parses_let_expression()
      {
        istringstream iss("\
f() =\n\
  let x = 1\n\
      y = 2\n\
  in  x + y\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Let *let1 = dynamic_cast<Let *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != let1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), let1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), let1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), let1->pos().column());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), let1->binds().size());
          auto bind_iter1 = let1->binds().begin();
          VariableBinding *var_bind2 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind2);
          CPPUNIT_ASSERT_EQUAL(string("x"), var_bind2->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_bind2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_bind2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_bind2->pos().column());
          Literal *literal2 = dynamic_cast<Literal *>(var_bind2->expr());
          CPPUNIT_ASSERT(nullptr != literal2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), literal2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal2->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value2);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());          
          bind_iter1++;
          VariableBinding *var_bind3 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind3);
          CPPUNIT_ASSERT_EQUAL(string("y"), var_bind3->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_bind3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_bind3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_bind3->pos().column());
          Literal *literal3 = dynamic_cast<Literal *>(var_bind3->expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal3->pos().column());
          IntValue *int_value3 = dynamic_cast<IntValue *>(literal3->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value3);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value3->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value3->i());
          NonUniqueApplication *app4 = dynamic_cast<NonUniqueApplication *>(let1->expr());
          CPPUNIT_ASSERT(nullptr != app4);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app4->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), app4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app4->pos().column());
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(app4->fun());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app4->args().size());
          auto arg_iter4 = app4->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr5->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr5->pos().column());
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          list<string> expected_idents5 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), rel_ident5->idents().begin())); 
          arg_iter4++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr6->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr6->pos().column());
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          list<string> expected_idents6 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents6.size(), rel_ident6->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents6.begin(), expected_idents6.end(), rel_ident6->idents().begin())); 
        }
      }
      
      void ParserTests::test_parser_parses_let_expression_with_nested_let_expressions()
      {
        istringstream iss("\
f() =\n\
  let x = let y = 1\n\
          in  y\n\
  in  let z = x\n\
      in  z\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Let *let1 = dynamic_cast<Let *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != let1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), let1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), let1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), let1->pos().column());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), let1->binds().size());
          auto bind_iter1 = let1->binds().begin();
          VariableBinding *var_bind2 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind2);
          CPPUNIT_ASSERT_EQUAL(string("x"), var_bind2->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_bind2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_bind2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_bind2->pos().column());
          Let *let2 = dynamic_cast<Let *>(var_bind2->expr());
          CPPUNIT_ASSERT(nullptr != let2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), let2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), let2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), let2->pos().column());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), let2->binds().size());
          auto bind_iter2 = let2->binds().begin();
          VariableBinding *var_bind3 = dynamic_cast<VariableBinding *>(bind_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_bind3);
          CPPUNIT_ASSERT_EQUAL(string("y"), var_bind3->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_bind3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_bind3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), var_bind3->pos().column());
          Literal *literal3 = dynamic_cast<Literal *>(var_bind3->expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), literal3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), literal3->pos().column());
          IntValue *int_value3 = dynamic_cast<IntValue *>(literal3->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value3);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value3->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value3->i());
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(let2->expr());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin())); 
          Let *let5 = dynamic_cast<Let *>(let1->expr());
          CPPUNIT_ASSERT(nullptr != let5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), let5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), let5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), let5->pos().column());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), let5->binds().size());
          auto bind_iter5 = let5->binds().begin();
          VariableBinding *var_bind6 = dynamic_cast<VariableBinding *>(bind_iter5->get());
          CPPUNIT_ASSERT(nullptr != var_bind6);
          CPPUNIT_ASSERT_EQUAL(string("z"), var_bind6->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_bind6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_bind6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_bind6->pos().column());
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(var_bind6->expr());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr6->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), var_expr6->pos().column());
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          list<string> expected_idents6 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents6.size(), rel_ident6->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents6.begin(), expected_idents6.end(), rel_ident6->idents().begin())); 
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(let5->expr());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr7->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr7->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr7->pos().column());
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          list<string> expected_idents7 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents7.size(), rel_ident7->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents7.begin(), expected_idents7.end(), rel_ident7->idents().begin())); 
        }
      }
      
      void ParserTests::test_parser_parses_match_expression()
      {
        istringstream iss("\
f() =\n\
  1 match {\n\
    1 -> 2 + 3\n\
    _ -> 4\n\
  }\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), match1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), match1->pos().column());
          Literal *literal1 = dynamic_cast<Literal *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), literal1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal1->pos().column());
          IntValue *int_value1 = dynamic_cast<IntValue *>(literal1->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value1);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value1->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value1->i());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          LiteralPattern *literal_pattern2 = dynamic_cast<LiteralPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != literal_pattern2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal_pattern2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal_pattern2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal_pattern2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal_pattern2->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value2);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());
          NonUniqueApplication *app3 = dynamic_cast<NonUniqueApplication *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != app3);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app3->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app3->args().size());
          auto arg_iter3 = app3->args().begin();
          Literal *literal4 = dynamic_cast<Literal *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), literal4->pos().column());
          IntValue *int_value4 = dynamic_cast<IntValue *>(literal4->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value4);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value4->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value4->i());
          arg_iter3++;
          Literal *literal5 = dynamic_cast<Literal *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), literal5->pos().column());
          IntValue *int_value5 = dynamic_cast<IntValue *>(literal5->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value5);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value5->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(3), int_value5->i());
          case_iter1++;
          WildcardPattern *wildcard_pattern6 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), wildcard_pattern6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), wildcard_pattern6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), wildcard_pattern6->pos().column());
          Literal *literal7 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal7);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal7->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), literal7->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), literal7->pos().column());
          IntValue *int_value7 = dynamic_cast<IntValue *>(literal7->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value7);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value7->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(4), int_value7->i());
        }
      }
      
      void ParserTests::test_parser_parses_match_expression_with_nested_match_expression()
      {
        istringstream iss("\
f() =\n\
  x match {\n\
    1 ->\n\
      y match {\n\
        2 -> 3\n\
        _ -> 4\n\
      }\n\
    _ -> 5\n\
  }\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), match1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), match1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          LiteralPattern *literal_pattern2 = dynamic_cast<LiteralPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != literal_pattern2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal_pattern2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal_pattern2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal_pattern2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal_pattern2->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value2);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());
          Match *match3 = dynamic_cast<Match *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != match3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), match3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), match3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), match3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(match3->expr());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match3->cases().size());
          auto case_iter3 = match3->cases().begin();
          LiteralPattern *literal_pattern4 = dynamic_cast<LiteralPattern *>((*case_iter3)->pattern());
          CPPUNIT_ASSERT(nullptr != literal_pattern4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal_pattern4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal_pattern4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal_pattern4->pos().column());
          IntValue *int_value4 = dynamic_cast<IntValue *>(literal_pattern4->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value4);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value4->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value4->i());
          Literal *literal5 = dynamic_cast<Literal *>((*case_iter3)->expr());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), literal5->pos().column());
          IntValue *int_value5 = dynamic_cast<IntValue *>(literal5->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value5);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value5->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(3), int_value5->i());
          case_iter3++;
          WildcardPattern *wildcard_pattern6 = dynamic_cast<WildcardPattern *>((*case_iter3)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), wildcard_pattern6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), wildcard_pattern6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), wildcard_pattern6->pos().column());
          Literal *literal7 = dynamic_cast<Literal *>((*case_iter3)->expr());
          CPPUNIT_ASSERT(nullptr != literal7);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal7->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), literal7->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), literal7->pos().column());
          IntValue *int_value7 = dynamic_cast<IntValue *>(literal7->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value7);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value7->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(4), int_value7->i());
          case_iter1++;
          WildcardPattern *wildcard_pattern8 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern8);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), wildcard_pattern8->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), wildcard_pattern8->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), wildcard_pattern8->pos().column());
          Literal *literal9 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal9);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal9->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), literal9->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), literal9->pos().column());
          IntValue *int_value9 = dynamic_cast<IntValue *>(literal9->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value9);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value9->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(5), int_value9->i());
        }
      }
      
      void ParserTests::test_parser_parses_throw_expression()
      {
        istringstream iss("\
f() =\n\
  throw Exception\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Throw *throw1 = dynamic_cast<Throw *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != throw1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), throw1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), throw1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), throw1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(throw1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "Exception" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
        }
      }
      
      void ParserTests::test_parser_parses_typed_expression()
      {
        istringstream iss("\
f() = 1: Int\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          TypedExpression *typed_expr1 = dynamic_cast<TypedExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != typed_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), typed_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), typed_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), typed_expr1->pos().column());
          Literal *literal2 = dynamic_cast<Literal *>(typed_expr1->expr());
          CPPUNIT_ASSERT(nullptr != literal2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal2->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value2);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(typed_expr1->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), type_var_expr3->pos().column());
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          list<string> expected_type_idents3 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents3.size(), type_rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents3.begin(), expected_type_idents3.end(), type_rel_ident3->idents().begin())); 
        }
      }
      
      void ParserTests::test_parser_parses_typed_expression_with_nested_expression()
      {
        istringstream iss("\
f() = x + 1: Int\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          TypedExpression *typed_expr1 = dynamic_cast<TypedExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != typed_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), typed_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), typed_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), typed_expr1->pos().column());
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(typed_expr1->expr());
          CPPUNIT_ASSERT(nullptr != app2);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app2->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          arg_iter2++;
          Literal *literal4 = dynamic_cast<Literal *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != literal4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal4->pos().column());
          IntValue *int_value4 = dynamic_cast<IntValue *>(literal4->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value4);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value4->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value4->i());
          TypeVariableExpression *type_var_expr5 = dynamic_cast<TypeVariableExpression *>(typed_expr1->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), type_var_expr5->pos().column());
          RelativeIdentifier *type_rel_ident5 = dynamic_cast<RelativeIdentifier *>(type_var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident5);
          list<string> expected_type_idents5 { "Int" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents5.size(), type_rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents5.begin(), expected_type_idents5.end(), type_rel_ident5->idents().begin())); 
        }
      }
      
      void ParserTests::test_parser_parses_cons_expression()
      {
        istringstream iss("\
f() = 1 :: 2 :: Nil\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "::" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          Literal *literal2 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal2->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value2);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());
          arg_iter1++;
          NonUniqueApplication *app3 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app3);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app3->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "::" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter3 = app3->args().begin();
          Literal *literal4 = dynamic_cast<Literal *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), literal4->pos().column());
          IntValue *int_value4 = dynamic_cast<IntValue *>(literal4->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value4);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value4->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value4->i());
          arg_iter3++;
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr5->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), var_expr5->pos().column());
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          list<string> expected_idents5 { "Nil" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), rel_ident5->idents().begin())); 
        }
      }
      
      void ParserTests::test_parser_parses_cons_expression_with_nested_expressions()
      {
        istringstream iss("\
f() = x + 1 :: g(2)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "::" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app2);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app2->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          arg_iter2++;
          Literal *literal4 = dynamic_cast<Literal *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != literal4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal4->pos().column());
          IntValue *int_value4 = dynamic_cast<IntValue *>(literal4->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value4);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value4->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value4->i());
          arg_iter1++;
          NonUniqueApplication *app5 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app5);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app5->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), app5->pos().column());
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(app5->fun());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr5->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr5->pos().column());
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          list<string> expected_idents5 { "g" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), rel_ident5->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app5->args().size());
          auto arg_iter5 = app5->args().begin();
          Literal *literal6 = dynamic_cast<Literal *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != literal6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), literal6->pos().column());
          IntValue *int_value6 = dynamic_cast<IntValue *>(literal6->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value6);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value6->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value6->i());
        }
      }
      
      void ParserTests::test_parser_parses_expression()
      {
        istringstream iss("\
f() = x + y - 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "-" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app2);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app2->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          arg_iter2++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin()));
          arg_iter1++;
          Literal *literal5 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), literal5->pos().column());
          IntValue *int_value5 = dynamic_cast<IntValue *>(literal5->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value5);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value5->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value5->i());
        }
      }
      
      void ParserTests::test_parser_parses_expression_with_nested_expressions()
      {
        istringstream iss("\
f() = x * 2 + y / 3\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app2);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app2->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "*" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          arg_iter2++;
          Literal *literal4 = dynamic_cast<Literal *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != literal4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal4->pos().column());
          IntValue *int_value4 = dynamic_cast<IntValue *>(literal4->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value4);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value4->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value4->i());
          arg_iter1++;
          NonUniqueApplication *app5 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app5);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app5->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), app5->pos().column());
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(app5->fun());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr5->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), var_expr5->pos().column());
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          list<string> expected_idents5 { "/" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), rel_ident5->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app5->args().size());
          auto arg_iter5 = app5->args().begin();
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr6->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), var_expr6->pos().column());
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          list<string> expected_idents6 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents6.size(), rel_ident6->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents6.begin(), expected_idents6.end(), rel_ident6->idents().begin())); 
          arg_iter5++;
          Literal *literal7 = dynamic_cast<Literal *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != literal7);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal7->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal7->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), literal7->pos().column());
          IntValue *int_value7 = dynamic_cast<IntValue *>(literal7->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value7);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value7->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(3), int_value7->i());          
        }
      }
      
      void ParserTests::test_parser_parses_expression_with_nested_unary_operator_expressions()
      {
        istringstream iss("\
f() = -x % ~y\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "%" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app2);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app2->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "unary_-" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          arg_iter1++;
          NonUniqueApplication *app4 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app4);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app4->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app4->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), app4->pos().column());
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(app4->fun());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "unary_~" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app4->args().size());
          auto arg_iter4 = app4->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr5->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr5->pos().column());
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          list<string> expected_idents5 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), rel_ident5->idents().begin())); 
        }
      }
      
      void ParserTests::test_parser_parses_unary_operator_expression()
      {
        istringstream iss("\
f() = -1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "unary_-" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          Literal *literal2 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), literal2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal2->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value2);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());          
        }
      }
      
      void ParserTests::test_parser_parses_unary_operator_expression_with_nested_unary_operator_expression()
      {
        istringstream iss("\
f() = -~1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "unary_-" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app2);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app2->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "unary_~" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          Literal *literal3 = dynamic_cast<Literal *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal3->pos().column());
          IntValue *int_value3 = dynamic_cast<IntValue *>(literal3->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value3);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value3->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value3->i());          
        }
      }
      
      void ParserTests::test_parser_parses_field_expression()
      {
        istringstream iss("\
f() = x.1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Field *field1 = dynamic_cast<Field *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != field1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), field1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), field1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), field1->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), field1->i());
        }
      }
      
      void ParserTests::test_parser_parses_unique_field_expression()
      {
        istringstream iss("\
f(x: T) = x unique .1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueField *unique_field1 = dynamic_cast<UniqueField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_field1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), unique_field1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_field1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), unique_field1->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(unique_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), unique_field1->i());
        }
      }

      void ParserTests::test_parser_parses_set_unique_field_expression()
      {
        istringstream iss("\
f(x: T) = x unique .1 <- 2\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          SetUniqueField *set_unique_field1 = dynamic_cast<SetUniqueField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != set_unique_field1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), set_unique_field1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), set_unique_field1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), set_unique_field1->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(set_unique_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), set_unique_field1->i());
          Literal *literal3 = dynamic_cast<Literal *>(set_unique_field1->value_expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), literal3->pos().column());
          IntValue *int_value3 = dynamic_cast<IntValue *>(literal3->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value3);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value3->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value3->i());          
        }
      }
      
      void ParserTests::test_parser_parses_named_field_expression()
      {
        istringstream iss("\
f() = (x).field\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NamedField *named_field1 = dynamic_cast<NamedField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != named_field1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), named_field1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), named_field1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), named_field1->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(named_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(string("field"), named_field1->ident());
        }
      }
      
      void ParserTests::test_parser_parses_unique_named_field_expression()
      {
        istringstream iss("\
f(x: T) = x unique .field\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueNamedField *unique_named_field1 = dynamic_cast<UniqueNamedField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_named_field1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), unique_named_field1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_named_field1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), unique_named_field1->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(unique_named_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(string("field"), unique_named_field1->ident());
        }
      }
      
      void ParserTests::test_parser_parses_set_unique_named_field_expression()
      {
        istringstream iss("\
f(x: T) = x unique .field <- 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("x"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          SetUniqueNamedField *set_unique_named_field1 = dynamic_cast<SetUniqueNamedField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != set_unique_named_field1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), set_unique_named_field1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), set_unique_named_field1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), set_unique_named_field1->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(set_unique_named_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(string("field"), set_unique_named_field1->ident());
          Literal *literal3 = dynamic_cast<Literal *>(set_unique_named_field1->value_expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), literal3->pos().column());
          IntValue *int_value3 = dynamic_cast<IntValue *>(literal3->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value3);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value3->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value3->i());
        }
      }
      
      void ParserTests::test_parser_parses_applications()
      {
        istringstream iss("\
f() = f2()\n\
\n\
g() = g2(x)\n\
\n\
h() = h2(x, y + 1, z)\n\
\n\
i() = i2(x, y)(z, 2)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "f2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(true, app1->args().empty());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "h2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          NonUniqueApplication *app3 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app3);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app3->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app3->args().size());
          auto arg_iter3 = app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin()));
          arg_iter3++;
          Literal *literal5 = dynamic_cast<Literal *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), literal5->pos().column());
          IntValue *int_value5 = dynamic_cast<IntValue *>(literal5->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value5);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value5->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value5->i());
          arg_iter1++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr6->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), var_expr6->pos().column());
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          list<string> expected_idents6 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents6.size(), rel_ident6->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents6.begin(), expected_idents6.end(), rel_ident6->idents().begin()));
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != app2);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app2->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "i2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
          arg_iter2++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr5->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr5->pos().column());
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          list<string> expected_idents5 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), rel_ident5->idents().begin()));
          arg_iter1++;
          Literal *literal6 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(19), literal6->pos().column());
          IntValue *int_value6 = dynamic_cast<IntValue *>(literal6->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value6);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value6->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value6->i());
        }
      }
      
      void ParserTests::test_parser_parses_primitive_applications()
      {
        istringstream iss("\
f() = f2 primitive ()\n\
\n\
g() = g2 primitive (x)\n\
\n\
h() = h2 primitive (x, y + 1, z)\n\
\n\
i() = i2 primitive (x, y) primitive (z, 2)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "f2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(true, app1->args().empty());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "h2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          NonUniqueApplication *app3 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app3);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app3->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(24), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app3->args().size());
          auto arg_iter3 = app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(24), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin()));
          arg_iter3++;
          Literal *literal5 = dynamic_cast<Literal *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), literal5->pos().column());
          IntValue *int_value5 = dynamic_cast<IntValue *>(literal5->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value5);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value5->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value5->i());
          arg_iter1++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr6->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(31), var_expr6->pos().column());
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          list<string> expected_idents6 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents6.size(), rel_ident6->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents6.begin(), expected_idents6.end(), rel_ident6->idents().begin()));
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app1->pos().column());
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != app2);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::PRIMITIVE, app2->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "i2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(21), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
          arg_iter2++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(24), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr5->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(38), var_expr5->pos().column());
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          list<string> expected_idents5 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), rel_ident5->idents().begin()));
          arg_iter1++;
          Literal *literal6 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(41), literal6->pos().column());
          IntValue *int_value6 = dynamic_cast<IntValue *>(literal6->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value6);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value6->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value6->i());
        }
      }
      
      void ParserTests::test_parser_parses_unique_applications()
      {
        istringstream iss("\
f(f2: T) = f2 unique ()\n\
\n\
g(g2: U) = g2 unique (x)\n\
\n\
h(h2: V) = h2 unique (x, y + 1, z)\n\
\n\
i(i2: W) = i2 unique (x, y) unique (z, 2)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("f2"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "T" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueApplication *unique_app1 = dynamic_cast<UniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), unique_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), unique_app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(unique_app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "f2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(true, unique_app1->args().empty());
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("g2"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "U" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueApplication *unique_app1 = dynamic_cast<UniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), unique_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), unique_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), unique_app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(unique_app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "g2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_app1->args().size());
          auto arg_iter1 = unique_app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("h"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("h2"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "V" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueApplication *unique_app1 = dynamic_cast<UniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), unique_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), unique_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), unique_app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(unique_app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr1->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), var_expr1->pos().column());
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          list<string> expected_idents1 { "h2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents1.size(), rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents1.begin(), expected_idents1.end(), rel_ident1->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), unique_app1->args().size());
          auto arg_iter1 = unique_app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin()));
          arg_iter1++;
          NonUniqueApplication *app3 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app3);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app3->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), app3->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(28), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "+" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app3->args().size());
          auto arg_iter3 = app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin()));
          arg_iter3++;
          Literal *literal5 = dynamic_cast<Literal *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal5->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(30), literal5->pos().column());
          IntValue *int_value5 = dynamic_cast<IntValue *>(literal5->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value5);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value5->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value5->i());
          arg_iter1++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr6->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_expr6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(33), var_expr6->pos().column());
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          list<string> expected_idents6 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents6.size(), rel_ident6->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents6.begin(), expected_idents6.end(), rel_ident6->idents().begin()));
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("i"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), (*arg_iter)->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), (*arg_iter)->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter)->pos().column());
          CPPUNIT_ASSERT_EQUAL(string("i2"), (*arg_iter)->ident());
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), type_var_expr1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), type_var_expr1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), type_var_expr1->pos().column());
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          list<string> expected_type_idents1 { "W" };
          CPPUNIT_ASSERT_EQUAL(expected_type_idents1.size(), type_rel_ident1->idents().size());
          CPPUNIT_ASSERT(equal(expected_type_idents1.begin(), expected_type_idents1.end(), type_rel_ident1->idents().begin()));
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueApplication *unique_app1 = dynamic_cast<UniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), unique_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), unique_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), unique_app1->pos().column());
          UniqueApplication *unique_app2 = dynamic_cast<UniqueApplication *>(unique_app1->fun());
          CPPUNIT_ASSERT(nullptr != unique_app2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), unique_app2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), unique_app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), unique_app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(unique_app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(12), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "i2" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), unique_app2->args().size());
          auto arg_iter2 = unique_app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(23), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
          arg_iter2++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr4->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(26), var_expr4->pos().column());
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          list<string> expected_idents4 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents4.size(), rel_ident4->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents4.begin(), expected_idents4.end(), rel_ident4->idents().begin()));
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), unique_app1->args().size());
          auto arg_iter1 = unique_app1->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr5->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), var_expr5->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(37), var_expr5->pos().column());
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          list<string> expected_idents5 { "z" };
          CPPUNIT_ASSERT_EQUAL(expected_idents5.size(), rel_ident5->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents5.begin(), expected_idents5.end(), rel_ident5->idents().begin()));
          arg_iter1++;
          Literal *literal6 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal6);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal6->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(40), literal6->pos().column());
          IntValue *int_value6 = dynamic_cast<IntValue *>(literal6->literal_value());
          CPPUNIT_ASSERT(nullptr != int_value6);
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value6->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(2), int_value6->i());
        }
      }
      
      void ParserTests::test_parser_parses_builtin_applications()
      {
        istringstream iss("\
f() = #itoi8(x)\n\
\n\
g() = #iadd(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.defs().size());
        auto def_list_iter = tree.defs().begin();
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*def_list_iter)->size());
        auto def_iter = (*def_list_iter)->begin();
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("f"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), builtin_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), builtin_app1->pos().column());
          CPPUNIT_ASSERT_EQUAL(BuiltinFunction::ITOI8, builtin_app1->fun());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(14), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
        }
        def_iter++;
        {
          FunctionDefinition *fun_def = dynamic_cast<FunctionDefinition *>(def_iter->get());
          CPPUNIT_ASSERT(nullptr != fun_def);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, fun_def->access_modifier());          
          CPPUNIT_ASSERT_EQUAL(string("g"), fun_def->ident());
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), fun_def->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_def->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), fun_def->pos().column());
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_def->fun().get());
          CPPUNIT_ASSERT_EQUAL(false, user_defined_fun->is_template());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->annotations().empty());
          CPPUNIT_ASSERT_EQUAL(InlineModifier::NONE, user_defined_fun->inline_modifier());
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, user_defined_fun->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), builtin_app1->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), builtin_app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), builtin_app1->pos().column());
          CPPUNIT_ASSERT_EQUAL(BuiltinFunction::IADD, builtin_app1->fun());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), var_expr2->pos().column());
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          list<string> expected_idents2 { "x" };
          CPPUNIT_ASSERT_EQUAL(expected_idents2.size(), rel_ident2->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents2.begin(), expected_idents2.end(), rel_ident2->idents().begin())); 
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), var_expr3->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_expr3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), var_expr3->pos().column());
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          list<string> expected_idents3 { "y" };
          CPPUNIT_ASSERT_EQUAL(expected_idents3.size(), rel_ident3->idents().size());
          CPPUNIT_ASSERT(equal(expected_idents3.begin(), expected_idents3.end(), rel_ident3->idents().begin()));
        }
      }
    }
  }
}
