/****************************************************************************
 *   Copyright (C) 2021 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#include <sstream>
#include "frontend/resolver_tests.hpp"

using namespace std;

namespace lesfl
{
  namespace frontend
  {
    namespace test
    {
      CPPUNIT_TEST_SUITE_REGISTRATION(ResolverTests);

      void ResolverTests::setUp()
      {
        _M_builtin_type_adder = new BuiltinTypeAdder();
        _M_parser = new Parser();
        _M_resolver = new Resolver();
      }

      void ResolverTests::tearDown()
      {
        delete _M_resolver;
        delete _M_parser;
        delete _M_builtin_type_adder;
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_simple_definitions()
      {
        istringstream iss("\
v = 1\n\
\n\
f(x) = #iadd(x, 2)\n\
\n\
g(x) = #isub(f(x), v)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier g_abs_ident(list<string> { "g" });
        CPPUNIT_ASSERT_EQUAL(true, g_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          arg_iter1++;
          Literal *literal3 = dynamic_cast<Literal *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(g_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          NonUniqueApplication *app2 = dynamic_cast<NonUniqueApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != app2);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(app2->fun());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(f_abs_ident.key_ident() == rel_ident2->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app2->args().size());
          auto arg_iter2 = app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident3->index());
          arg_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(v_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_modules()
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
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_abs_ident(list<string> { "somelib" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_f_abs_ident(list<string> { "somelib", "f" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_f_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_module2_abs_ident(list<string> { "somelib", "module1", "module2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_module2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_module2_g_abs_ident(list<string> { "somelib", "module1", "module2", "g" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_module2_g_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module3_abs_ident(list<string> { "somelib2", "module3" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module3_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module3_h_abs_ident(list<string> { "somelib2", "module3", "h" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module3_h_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier i_abs_ident(list<string> { "i" });
        CPPUNIT_ASSERT_EQUAL(true, i_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib_f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_module2_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib_module1_module2_g_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module3_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib2_module3_h_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(i_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_module1_module2_g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(somelib2_module3_h_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(i_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_variable()
      {
        istringstream iss("\
v = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_variable_with_access_modifier_and_type()
      {
        istringstream iss("\
import stdlib\n\
\n\
private v: Int64 = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr != user_defined_var->type_expr());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident->key_ident());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_external_variable()
      {
        istringstream iss("\
import stdlib\n\
\n\
extern v: Int64 = somevar\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ExternalVariable *external_var = dynamic_cast<ExternalVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != external_var);
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(external_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_external_variable_with_access_modifier()
      {
        istringstream iss("\
import stdlib\n\
\n\
private extern v: Int64 = somevar\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          ExternalVariable *external_var = dynamic_cast<ExternalVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != external_var);
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(external_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }

      void ResolverTests::test_resolver_adds_alias_variable()
      {
        istringstream iss("\
v = a\n\
\n\
a = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          AliasVariable *alias_var = dynamic_cast<AliasVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != alias_var);
          CPPUNIT_ASSERT(nullptr == alias_var->type_expr());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(alias_var->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_alias_variable_with_access_modifier_and_type()
      {
        istringstream iss("\
import stdlib\n\
\n\
private v: Int64 = a\n\
\n\
a = 1\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          AliasVariable *alias_var = dynamic_cast<AliasVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != alias_var);
          CPPUNIT_ASSERT(nullptr != alias_var->type_expr());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(alias_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident->key_ident());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(alias_var->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident->key_ident());  
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_variable_template()
      {
        istringstream iss("\
template\n\
v = #[]\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->inst_type_params().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_var->value());
          ArrayValue *array_value = dynamic_cast<ArrayValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != array_value);
          CPPUNIT_ASSERT_EQUAL(true, array_value->elems().empty());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }

      void ResolverTests::test_resolver_adds_user_defined_variable_template_with_access_modifier_and_type_and_instance_type_parameter()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private v: Array(t) = #[]\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_var->inst_type_params().size());
          auto inst_type_param_iter = user_defined_var->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          CPPUNIT_ASSERT(nullptr != user_defined_var->type_expr());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app->args().size());
          auto type_arg_iter = type_app->args().begin();
          TypeParameterExpression *type_param_expr1 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr1->index());
          CPPUNIT_ASSERT(nullptr != user_defined_var->value());
          ArrayValue *array_value = dynamic_cast<ArrayValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != array_value);
          CPPUNIT_ASSERT_EQUAL(true, array_value->elems().empty());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_variable_template_without_value()
      {
        istringstream iss("\
import stdlib\n\
\n\
template\n\
v: Array(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->inst_type_params().empty());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app->args().size());
          auto type_arg_iter = type_app->args().begin();
          TypeParameterExpression *type_param_expr1 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr1->index());
          CPPUNIT_ASSERT(nullptr == user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_variable_template_without_value_with_access_modifier_and_instance_type_parameter()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private v: Array(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_var->inst_type_params().size());
          auto inst_type_param_iter = user_defined_var->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app->args().size());
          auto type_arg_iter = type_app->args().begin();
          TypeParameterExpression *type_param_expr1 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr1->index());
          CPPUNIT_ASSERT(nullptr == user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_alias_variable_template()
      {
        istringstream iss("\
template\n\
v = a\n\
\n\
template\n\
a = #[]\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          AliasVariable *alias_var = dynamic_cast<AliasVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != alias_var);
          CPPUNIT_ASSERT_EQUAL(true, alias_var->inst_type_params().empty());
          CPPUNIT_ASSERT(nullptr == alias_var->type_expr());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(alias_var->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident->key_ident());          
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->inst_type_params().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_var->value());
          ArrayValue *array_value = dynamic_cast<ArrayValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != array_value);
          CPPUNIT_ASSERT_EQUAL(true, array_value->elems().empty());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_alias_variable_template_with_access_modifier_and_type_and_instance_type_parameter()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private v: Array(t) = a\n\
\n\
template\n\
a = #[]\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          AliasVariable *alias_var = dynamic_cast<AliasVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != alias_var);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), alias_var->inst_type_params().size());
          auto inst_type_param_iter = alias_var->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(alias_var->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app);          
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app->args().size());
          auto type_arg_iter = type_app->args().begin();
          TypeParameterExpression *type_param_expr1 = dynamic_cast<TypeParameterExpression *>(type_arg_iter->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr1->index());
          RelativeIdentifier *rel_ident = dynamic_cast<RelativeIdentifier *>(alias_var->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident->key_ident());          
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->inst_type_params().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_var->value());
          ArrayValue *array_value = dynamic_cast<ArrayValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != array_value);
          CPPUNIT_ASSERT_EQUAL(true, array_value->elems().empty());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_function()
      {
        istringstream iss("\
f(x, y) = #itoi8(#iadd(x, y))\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          BuiltinApplication *builtin_app2 = dynamic_cast<BuiltinApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != builtin_app2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app2->args().size());
          auto arg_iter2 = builtin_app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident3->index());
          arg_iter2++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident4->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_function_with_access_modifier_and_types()
      {
        istringstream iss("\
import stdlib\n\
\n\
private f(x, y: Int64): Int8 = #itoi8(#iadd(x, y))\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->result_type_expr());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident3->key_ident());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          BuiltinApplication *builtin_app2 = dynamic_cast<BuiltinApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != builtin_app2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app2->args().size());
          auto arg_iter2 = builtin_app2->args().begin();
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident3->index());
          arg_iter2++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident4->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }

      void ResolverTests::test_resolver_adds_external_function()
      {
        istringstream iss("\
import stdlib\n\
\n\
extern f(x: Int16, y: Int64): Int8 = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int16_abs_ident(list<string> { "stdlib", "Int16" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int16_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          ExternalFunction *external_fun = dynamic_cast<ExternalFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != external_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), external_fun->args().size());
          auto arg_iter = external_fun->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int16_abs_ident.key_ident() == type_rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(external_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_external_function_with_access_modifier()
      {
        istringstream iss("\
import stdlib\n\
\n\
private extern f(x: Int16, y: Int64): Int8 = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int16_abs_ident(list<string> { "stdlib", "Int16" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int16_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          ExternalFunction *external_fun = dynamic_cast<ExternalFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != external_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), external_fun->args().size());
          auto arg_iter = external_fun->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int16_abs_ident.key_ident() == type_rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(external_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_native_function()
      {
        istringstream iss("\
import stdlib\n\
\n\
native f(x: Int64, y: Int16): Int8 = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int16_abs_ident(list<string> { "stdlib", "Int16" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int16_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          NativeFunction *native_fun = dynamic_cast<NativeFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != native_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), native_fun->args().size());
          auto arg_iter = native_fun->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int16_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(native_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }

      void ResolverTests::test_resolver_adds_native_function_with_access_modifier()
      {
        istringstream iss("\
import stdlib\n\
\n\
private native f(x: Int64, y: Int16): Int8 = somefun\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int16_abs_ident(list<string> { "stdlib", "Int16" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int16_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          NativeFunction *native_fun = dynamic_cast<NativeFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != native_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), native_fun->args().size());
          auto arg_iter = native_fun->args().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int16_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(native_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }

      void ResolverTests::test_resolver_adds_user_defined_function_template()
      {
        istringstream iss("\
template\n\
f(x, y) = #anth(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->body());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident3->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_function_template_with_access_modifier_and_types_and_instance_type_parameter()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private f(x: Array(t), y): t = #anth(x, y)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->inst_type_params().size());
          auto inst_type_param_iter = user_defined_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->result_type_expr());
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr3->index());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->body());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident3->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_function_template_without_body()
      {
        istringstream iss("\
import stdlib\n\
\n\
template\n\
f(x: Array(t), y: Int64): t\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app1);          
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr3->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_user_defined_function_template_without_body_with_access_modifier_and_instance_type_parameter()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private f(x: Array(t), y: Int64): t\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->inst_type_params().size());
          auto inst_type_param_iter = user_defined_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr3->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }

      void ResolverTests::test_resolver_adds_type_synonym_variable()
      {
        istringstream iss("\
import stdlib\n\
\n\
type T = Int64\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_type_synonym_variable_with_access_modifier()
      {
        istringstream iss("\
import stdlib\n\
\n\
private type T = Int64\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }

      void ResolverTests::test_resolver_adds_datatype_variable()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C(Int64, Int8)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident2->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }

      void ResolverTests::test_resolver_adds_datatype_variable_with_access_modifier()
      {
        istringstream iss("\
import stdlib\n\
\n\
private datatype T = C(Int8, Int64)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }

      void ResolverTests::test_resolver_adds_unique_datatype_variable()
      {
        istringstream iss("\
import stdlib\n\
\n\
unique datatype T = C(Int64, Int8)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident2->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_unique_datatype_variable_with_access_modifier()
      {
        istringstream iss("\
import stdlib\n\
\n\
private unique datatype T = C(Int8, Int64)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_datatype_variable_without_constructors()
      {
        istringstream iss("\
datatype T\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_datatype_variable_without_constructors_with_access_modifier()
      {
        istringstream iss("\
private datatype T\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_unique_datatype_variable_without_constructors()
      {
        istringstream iss("\
unique datatype T\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_unique_datatype_variable_without_constructors_with_access_modifier()
      {
        istringstream iss("\
private unique datatype T\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
      }
      
      void ResolverTests::test_resolver_adds_type_synonym_function()
      {
        istringstream iss("\
import stdlib\n\
\n\
template\n\
type T(t) = Array(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_info->access_modifier());
          TypeSynonymFunction *type_synonym_fun = dynamic_cast<TypeSynonymFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun);
          CPPUNIT_ASSERT_EQUAL(true, type_synonym_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_synonym_fun->args().size());
          auto type_arg_iter = type_synonym_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun->body());
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(type_synonym_fun->body());
          CPPUNIT_ASSERT(nullptr != type_app1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }

      void ResolverTests::test_resolver_adds_type_synonym_function_with_access_modifier_and_instance_type_parameter()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private type T(t) = Array(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_info->access_modifier());
          TypeSynonymFunction *type_synonym_fun = dynamic_cast<TypeSynonymFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_synonym_fun->inst_type_params().size());
          auto inst_type_param_iter = type_synonym_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_synonym_fun->args().size());
          auto type_arg_iter = type_synonym_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun->body());
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(type_synonym_fun->body());
          CPPUNIT_ASSERT(nullptr != type_app1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }

      void ResolverTests::test_resolver_adds_type_synonym_function_without_body()
      {
        istringstream iss("\
template\n\
type T(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_info->access_modifier());
          TypeSynonymFunction *type_synonym_fun = dynamic_cast<TypeSynonymFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun);
          CPPUNIT_ASSERT_EQUAL(true, type_synonym_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_synonym_fun->args().size());
          auto type_arg_iter = type_synonym_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == type_synonym_fun->body());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }

      void ResolverTests::test_resolver_adds_type_synonym_function_without_body_with_access_modifier_and_instance_type_parameter()
      {
        istringstream iss("\
template(t)\n\
private type T(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_info->access_modifier());
          TypeSynonymFunction *type_synonym_fun = dynamic_cast<TypeSynonymFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_synonym_fun->inst_type_params().size());
          auto inst_type_param_iter = type_synonym_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_synonym_fun->args().size());
          auto type_arg_iter = type_synonym_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == type_synonym_fun->body());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }

      void ResolverTests::test_resolver_adds_datatype_function()
      {
        istringstream iss("\
import stdlib\n\
\n\
template\n\
datatype T(t) = C(Array(t), Int64)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_info->access_modifier());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          field_type_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }
      
      void ResolverTests::test_resolver_adds_datatype_function_with_access_modifier_and_instance_type_parameter()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private datatype T(t) = C(Int64, Array(t))\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_info->access_modifier());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->inst_type_params().size());
          auto inst_type_param_iter = datatype_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeApplication *type_app2 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_app2->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident2->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app2->args().size());
          auto type_arg_iter2 = type_app2->args().begin();
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr3->index());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }
      
      void ResolverTests::test_resolver_adds_unique_datatype_function()
      {
        istringstream iss("\
import stdlib\n\
\n\
template\n\
unique datatype T(t) = C(Array(t), Int64)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_info->access_modifier());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeApplication *type_app1 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          field_type_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }
      
      void ResolverTests::test_resolver_adds_unique_datatype_function_with_access_modifier_and_instance_type_parameter()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private unique datatype T(t) = C(Int64, Array(t))\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_array_abs_ident(list<string> { "stdlib", "Array" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_array_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_info->access_modifier());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->inst_type_params().size());
          auto inst_type_param_iter = datatype_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), unique_datatype->constrs().size());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeApplication *type_app2 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_app2->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident2->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app2->args().size());
          auto type_arg_iter2 = type_app2->args().begin();
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(type_arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr3->index());
          
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }
      
      void ResolverTests::test_resolver_adds_datatype_function_without_constructors()
      {
        istringstream iss("\
template\n\
datatype T(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_info->access_modifier());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }
      
      void ResolverTests::test_resolver_adds_datatype_function_without_constructors_with_access_modifier_and_instance_type_parameter()
      {
        istringstream iss("\
template(t)\n\
private datatype T(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_info->access_modifier());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->inst_type_params().size());
          auto inst_type_param_iter = datatype_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }
      
      void ResolverTests::test_resolver_adds_unique_datatype_function_without_constructors()
      {
        istringstream iss("\
template\n\
unique datatype T(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_info->access_modifier());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(true, datatype_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }
      
      void ResolverTests::test_resolver_adds_unique_datatype_function_without_constrcutors_with_access_modifier_and_instance_type_parameter()
      {
        istringstream iss("\
template(t)\n\
private unique datatype T(t)\n\
");
        vector<Source> sources;
        sources.push_back(Source("test.lesfl", iss));
        list<Error> errors;
        Tree tree;
        CPPUNIT_ASSERT_EQUAL(true, _M_builtin_type_adder->add_builtin_types(tree));
        CPPUNIT_ASSERT_EQUAL(true, _M_parser->parse(sources, tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        CPPUNIT_ASSERT_EQUAL(true, _M_resolver->resolve(tree, errors));
        CPPUNIT_ASSERT(errors.empty());
        AbsoluteIdentifier root_abs_ident;
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_info->access_modifier());
          DatatypeFunction *datatype_fun = dynamic_cast<DatatypeFunction *>(type_fun_info->fun().get());
          CPPUNIT_ASSERT(nullptr != datatype_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->inst_type_params().size());
          auto inst_type_param_iter = datatype_fun->inst_type_params().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*inst_type_param_iter)->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun->args().size());
          auto type_arg_iter = datatype_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_fun->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(true, unique_datatype->constrs().empty());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
      }
    }
  }
}
