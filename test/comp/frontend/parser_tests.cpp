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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("x"), arg_iter->get()->ident());
          CPPUNIT_ASSERT(nullptr == arg_iter->get()->type_expr());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          CPPUNIT_ASSERT_EQUAL(FunctionModifier::NONE, app1->fun_modifier());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
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
          CPPUNIT_ASSERT_EQUAL(5UL, app2->pos().line());
          CPPUNIT_ASSERT_EQUAL(8UL, app2->pos().column());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          CPPUNIT_ASSERT_EQUAL(5UL, var_expr2->pos().line());
          CPPUNIT_ASSERT_EQUAL(12UL, var_expr2->pos().column());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
          CPPUNIT_ASSERT(nullptr != var_expr3);
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app3->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app3->pos().column());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(app3->fun());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app4->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), app4->pos().column());
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(app4->fun());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app6->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), app6->pos().column());
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(app6->fun());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal2->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), literal2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal2->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());
          arg_iter1++;
          Literal *literal3 = dynamic_cast<Literal *>(arg_iter1->get());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), app1->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), app1->pos().column());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal2->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), literal2->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), literal2->pos().column());
          IntValue *int_value2 = dynamic_cast<IntValue *>(literal2->literal_value());
          CPPUNIT_ASSERT_EQUAL(IntType::INT64, int_value2->int_type());
          CPPUNIT_ASSERT_EQUAL(static_cast<int64_t>(1), int_value2->i());
          arg_iter1++;
          Literal *literal3 = dynamic_cast<Literal *>(arg_iter1->get());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          CharValue *char_value = dynamic_cast<CharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideCharValue *wide_char_value = dynamic_cast<WideCharValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          IntValue *int_value = dynamic_cast<IntValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(11), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(13), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(15), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(17), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
          CPPUNIT_ASSERT_EQUAL(FloatType::SINGLE, float_value->float_type());
          CPPUNIT_ASSERT(isinf(float_value->f()));
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          FloatValue *float_value = dynamic_cast<FloatValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          StringValue *string_value = dynamic_cast<StringValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          StringValue *string_value = dynamic_cast<StringValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          StringValue *string_value = dynamic_cast<StringValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideStringValue *wide_string_value = dynamic_cast<WideStringValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideStringValue *wide_string_value = dynamic_cast<WideStringValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideStringValue *wide_string_value = dynamic_cast<WideStringValue *>(literal->literal_value());
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
          CPPUNIT_ASSERT_EQUAL(string("test.lesfl"), literal->pos().source().file_name());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), literal->pos().line());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(7), literal->pos().column());
          WideStringValue *wide_string_value = dynamic_cast<WideStringValue *>(literal->literal_value());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), wide_string_value->string().length());
          CPPUNIT_ASSERT_EQUAL(L'\uabcd', wide_string_value->string()[0]);
          CPPUNIT_ASSERT_EQUAL(static_cast<wchar_t>(0x001234ab), wide_string_value->string()[1]);
        }
      } 
    }
  }
}
