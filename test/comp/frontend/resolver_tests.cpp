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
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident->key_ident());          
          CPPUNIT_ASSERT(nullptr != type_app);
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
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_var->inst_type_params().empty());
          TypeApplication *type_app = dynamic_cast<TypeApplication *>(user_defined_var->type_expr());
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident->key_ident());          
          CPPUNIT_ASSERT(nullptr != type_app);
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
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident->key_ident());          
          CPPUNIT_ASSERT(nullptr != type_app);
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
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_app->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident->key_ident());          
          CPPUNIT_ASSERT(nullptr != type_app);
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
    }
  }
}
