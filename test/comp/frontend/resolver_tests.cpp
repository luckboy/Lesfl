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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_module1_module2_g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(somelib2_module3_h_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(i_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
       }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_adds_constructors()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C\n\
           | D()\n\
           | E(Int64, WChar)\n\
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
        AbsoluteIdentifier stdlib_w_char_abs_ident(list<string> { "stdlib", "WChar" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_w_char_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier e_abs_ident(list<string> { "E" });
        CPPUNIT_ASSERT_EQUAL(true, e_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(true, constr->field_types().empty());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(e_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(stdlib_w_char_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(e_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_adds_function_constructors_for_unique_datatype()
      {
        istringstream iss("\
import stdlib\n\
\n\
unique datatype T = C()\n\
                  | D(Int64, WChar)\n\
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
        AbsoluteIdentifier stdlib_w_char_abs_ident(list<string> { "stdlib", "WChar" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_w_char_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          UniqueDatatype *unique_datatype = dynamic_cast<UniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != unique_datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), unique_datatype->constrs().size());
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
          CPPUNIT_ASSERT_EQUAL(true, constr->field_types().empty());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(stdlib_w_char_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_adds_public_constructor_for_public_datatype()
      {
        istringstream iss("\
datatype T = C\n\
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
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_adds_private_constructor_for_public_datatype()
      {
        istringstream iss("\
datatype T = private C\n\
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_adds_public_constructor_for_private_datatype()
      {
        istringstream iss("\
private datatype T = C\n\
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
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }

      void ResolverTests::test_resolver_adds_private_constructor_for_private_datatype()
      {
        istringstream iss("\
private datatype T = private C\n\
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
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_adds_variable_constructor()
      {
        istringstream iss("\
datatype T = C\n\
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
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_adds_function_constructor()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C(Char, WChar, Int64)\n\
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
        AbsoluteIdentifier stdlib_char_abs_ident(list<string> { "stdlib", "Char" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_char_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_w_char_abs_ident(list<string> { "stdlib", "WChar" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_w_char_abs_ident.set_key_ident(*(tree.ident_table())));
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_char_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_w_char_abs_ident.key_ident() == type_rel_ident2->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_adds_named_field_constructor()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C {\n\
    field1: Int64,\n\
    field2: WChar,\n\
    field3: Char\n\
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
        AbsoluteIdentifier stdlib_char_abs_ident(list<string> { "stdlib", "Char" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_char_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_w_char_abs_ident(list<string> { "stdlib", "WChar" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_w_char_abs_ident.set_key_ident(*(tree.ident_table())));
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
          NamedFieldConstructor *constr = dynamic_cast<NamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_w_char_abs_ident.key_ident() == type_rel_ident2->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_char_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_adds_constructor_in_type_function_instance_where_access_modifier_is_not_updated()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
datatype T(t)\n\
\n\
instance\n\
datatype T(Int64) = C\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_info->access_modifier());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_fun_info->insts()->size());
          auto inst_iter = type_fun_info->insts()->begin();
          {
            DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(inst_iter->get());
            CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun_inst->args().size());
            auto type_arg_iter = datatype_fun_inst->args().begin();
            TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
            CPPUNIT_ASSERT(nullptr != type_var_expr1);
            RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
            CPPUNIT_ASSERT(nullptr != type_rel_ident1);
            CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
            CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
            NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun_inst->datatype());
            CPPUNIT_ASSERT(nullptr != datatype);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          }
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(inst_iter->get() == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr != var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_inst_pairs().size());
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_inst_pairs()[0].key_ident);
          CPPUNIT_ASSERT(inst_iter->get() == tree.uncompiled_type_fun_inst_pairs()[0].inst.get());
        }
      }

      void ResolverTests::test_resolver_adds_public_constructor_in_type_function_instance_for_public_datatype()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
datatype T(t)\n\
\n\
instance\n\
datatype T(Int64) = C\n\
\n\
f() = C\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_info->access_modifier());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_fun_info->insts()->size());
          auto inst_iter = type_fun_info->insts()->begin();
          {
            DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(inst_iter->get());
            CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun_inst->args().size());
            auto type_arg_iter = datatype_fun_inst->args().begin();
            TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
            CPPUNIT_ASSERT(nullptr != type_var_expr1);
            RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
            CPPUNIT_ASSERT(nullptr != type_rel_ident1);
            CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
            CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
            NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun_inst->datatype());
            CPPUNIT_ASSERT(nullptr != datatype);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          }
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(inst_iter->get() == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_inst_pairs().size());
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_inst_pairs()[0].key_ident);
          CPPUNIT_ASSERT(inst_iter->get() == tree.uncompiled_type_fun_inst_pairs()[0].inst.get());
        }
      }
      
      void ResolverTests::test_resolver_adds_private_constructor_in_type_function_instance_for_public_datatype()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
datatype T(t)\n\
\n\
instance\n\
datatype T(Int64) = private C\n\
\n\
f() = C\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_fun_info->access_modifier());
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_fun_info->insts()->size());
          auto inst_iter = type_fun_info->insts()->begin();
          {
            DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(inst_iter->get());
            CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun_inst->args().size());
            auto type_arg_iter = datatype_fun_inst->args().begin();
            TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
            CPPUNIT_ASSERT(nullptr != type_var_expr1);
            RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
            CPPUNIT_ASSERT(nullptr != type_rel_ident1);
            CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
            CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
            NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun_inst->datatype());
            CPPUNIT_ASSERT(nullptr != datatype);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          }
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(inst_iter->get() == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_inst_pairs().size());
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_inst_pairs()[0].key_ident);
          CPPUNIT_ASSERT(inst_iter->get() == tree.uncompiled_type_fun_inst_pairs()[0].inst.get());
        }
      }
      
      void ResolverTests::test_resolver_adds_public_constructor_in_type_function_instance_for_private_datatype()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private datatype T(t)\n\
\n\
instance\n\
datatype T(Int64) = C\n\
\n\
f() = C\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_fun_info->insts()->size());
          auto inst_iter = type_fun_info->insts()->begin();
          {
            DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(inst_iter->get());
            CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun_inst->args().size());
            auto type_arg_iter = datatype_fun_inst->args().begin();
            TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
            CPPUNIT_ASSERT(nullptr != type_var_expr1);
            RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
            CPPUNIT_ASSERT(nullptr != type_rel_ident1);
            CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
            CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
            NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun_inst->datatype());
            CPPUNIT_ASSERT(nullptr != datatype);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          }
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(inst_iter->get() == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_inst_pairs().size());
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_inst_pairs()[0].key_ident);
          CPPUNIT_ASSERT(inst_iter->get() == tree.uncompiled_type_fun_inst_pairs()[0].inst.get());
        }
      }
      
      void ResolverTests::test_resolver_adds_private_constructor_in_type_function_instance_for_private_datatype()
      {
        istringstream iss("\
import stdlib\n\
\n\
template(t)\n\
private datatype T(t)\n\
\n\
instance\n\
datatype T(Int64) = private C\n\
\n\
f() = C\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_fun_info->insts()->size());
          auto inst_iter = type_fun_info->insts()->begin();
          {
            DatatypeFunctionInstance *datatype_fun_inst = dynamic_cast<DatatypeFunctionInstance *>(inst_iter->get());
            CPPUNIT_ASSERT(nullptr != datatype_fun_inst);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype_fun_inst->args().size());
            auto type_arg_iter = datatype_fun_inst->args().begin();
            TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_arg_iter->get());
            CPPUNIT_ASSERT(nullptr != type_var_expr1);
            RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
            CPPUNIT_ASSERT(nullptr != type_rel_ident1);
            CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
            CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
            NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_fun_inst->datatype());
            CPPUNIT_ASSERT(nullptr != datatype);
            CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
          }
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(true, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(inst_iter->get() == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_inst_pairs().size());
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(t_abs_ident.key_ident());
          auto inst_iter = type_fun_info->insts()->begin();
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_inst_pairs()[0].key_ident);
          CPPUNIT_ASSERT(inst_iter->get() == tree.uncompiled_type_fun_inst_pairs()[0].inst.get());
        }
      }
      
      void ResolverTests::test_resolver_resolves_variable_absolute_identifiers()
      {
        istringstream iss("\
module somelib {\n\
  a = 1\n\
\n\
  module module1 {\n\
    b = 2\n\
  }\n\
}\n\
\n\
module somelib2 {\n\
  f() = .somelib.a\n\
\n\
  g() = .somelib.module1.b\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "a" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_b_abs_ident(list<string> { "somelib", "module1", "b" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_f_abs_ident(list<string> { "somelib2", "f" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_f_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_g_abs_ident(list<string> { "somelib2", "g" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_g_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib_module1_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib2_f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          AbsoluteIdentifier *abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != abs_ident1);
          CPPUNIT_ASSERT_EQUAL(true, abs_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == abs_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib2_g_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          AbsoluteIdentifier *abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != abs_ident1);
          CPPUNIT_ASSERT_EQUAL(true, abs_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_module1_b_abs_ident.key_ident() == abs_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_module1_b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(somelib2_f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(somelib2_g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_variable_relative_identifiers_for_current_module()
      {
        istringstream iss("\
module somelib {\n\
  module module1 {\n\
    a = 1\n\
  }\n\
\n\
  b = 2\n\
}\n\
\n\
module predef {\n\
  c = 3\n\
}\n\
\n\
import somelib\n\
\n\
module somelib2 {\n\
  module module1 {\n\
    a = 4\n\
  }\n\
\n\
  b = 5\n\
\n\
  c = 6\n\
\n\
  f() = module1.a\n\
\n\
  g() = b\n\
\n\
  h() = c\n\
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
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_a_abs_ident(list<string> { "somelib", "module1", "a" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_b_abs_ident(list<string> { "somelib", "b" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_abs_ident(list<string> { "predef" });
        CPPUNIT_ASSERT_EQUAL(true, predef_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_c_abs_ident(list<string> { "predef", "c" });
        CPPUNIT_ASSERT_EQUAL(true, predef_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module1_abs_ident(list<string> { "somelib2", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module1_a_abs_ident(list<string> { "somelib2", "module1", "a" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module1_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_b_abs_ident(list<string> { "somelib2", "b" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_c_abs_ident(list<string> { "somelib2", "c" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_f_abs_ident(list<string> { "somelib2", "f" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_f_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_g_abs_ident(list<string> { "somelib2", "g" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_g_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_h_abs_ident(list<string> { "somelib2", "h" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_h_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib_module1_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(predef_c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module1_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib2_module1_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib2_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib2_c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib2_f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib2_module1_a_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib2_g_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib2_b_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib2_h_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib2_c_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_module1_a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(predef_c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(somelib2_module1_a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT(somelib2_b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[4]);
        CPPUNIT_ASSERT(somelib2_c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[5]);
        CPPUNIT_ASSERT(somelib2_f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[6]);
        CPPUNIT_ASSERT(somelib2_g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[7]);
        CPPUNIT_ASSERT(somelib2_h_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[8]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_variable_relative_identifiers_for_imported_modules()
      {
        istringstream iss("\
module predef {\n\
  a = 1\n\
}\n\
\n\
module somelib {\n\
  a = 2\n\
\n\
  module module1 {\
    b = 3\n\
  }\n\
\n\
  c = 4\n\
}\n\
\n\
module somelib2 {\n\
  module module2 {\n\
    c = 5\n\
  }\n\
  \n\
  module module3 {\n\
    c = 6\n\
  }\n\
}\n\
\n\
import somelib\n\
\n\
module somelib3 {\n\
  import .somelib2.module2\n\
  import .somelib2.module3\n\
\n\
  f() = a\n\
\n\
  g() = module1.b\n\
\n\
  h() = c\n\
}\n\
\n\
module somelib4 {\n\
  i() = c\n\
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
        AbsoluteIdentifier predef_abs_ident(list<string> { "predef" });
        CPPUNIT_ASSERT_EQUAL(true, predef_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_a_abs_ident(list<string> { "predef", "a" });
        CPPUNIT_ASSERT_EQUAL(true, predef_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_abs_ident(list<string> { "somelib" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "a" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_b_abs_ident(list<string> { "somelib", "module1", "b" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_c_abs_ident(list<string> { "somelib", "c" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module2_abs_ident(list<string> { "somelib2", "module2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module2_c_abs_ident(list<string> { "somelib2", "module2", "c" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module2_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module3_abs_ident(list<string> { "somelib2", "module3" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module3_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module3_c_abs_ident(list<string> { "somelib2", "module3", "c" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module3_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_abs_ident(list<string> { "somelib3" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_f_abs_ident(list<string> { "somelib3", "f" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_f_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_g_abs_ident(list<string> { "somelib3", "g" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_g_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_h_abs_ident(list<string> { "somelib3", "h" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_h_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib4_abs_ident(list<string> { "somelib4" });
        CPPUNIT_ASSERT_EQUAL(true, somelib4_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib4_i_abs_ident(list<string> { "somelib4", "i" });
        CPPUNIT_ASSERT_EQUAL(true, somelib4_i_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(predef_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib_module1_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib_c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module2_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib2_module2_c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module3_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib2_module3_c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib3_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib3_f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib3_g_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_module1_b_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib3_h_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib2_module3_c_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib4_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib4_i_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_c_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(predef_a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(somelib_module1_b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(somelib_c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT(somelib2_module2_c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[4]);
        CPPUNIT_ASSERT(somelib2_module3_c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[5]);
        CPPUNIT_ASSERT(somelib3_f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[6]);
        CPPUNIT_ASSERT(somelib3_g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[7]);
        CPPUNIT_ASSERT(somelib3_h_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[8]);
        CPPUNIT_ASSERT(somelib4_i_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[9]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_variable_relative_identifiers_for_predef_module()
      {
        istringstream iss("\
module predef {\n\
  a = 1\n\
\n\
  module module1 {\n\
    b = 2\n\
  }\n\
}\n\
\n\
module somelib {\n\
  f() = a\n\
\n\
  g() = module1.b\n\
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
        AbsoluteIdentifier predef_abs_ident(list<string> { "predef" });
        CPPUNIT_ASSERT_EQUAL(true, predef_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_a_abs_ident(list<string> { "predef", "a" });
        CPPUNIT_ASSERT_EQUAL(true, predef_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_module1_abs_ident(list<string> { "predef", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, predef_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_module1_b_abs_ident(list<string> { "predef", "module1", "b" });
        CPPUNIT_ASSERT_EQUAL(true, predef_module1_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_abs_ident(list<string> { "somelib" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_f_abs_ident(list<string> { "somelib", "f" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_f_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_g_abs_ident(list<string> { "somelib", "g" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_g_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(predef_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_module1_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(predef_module1_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
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
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(predef_a_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(somelib_g_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(predef_module1_b_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(predef_a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(predef_module1_b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(somelib_f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(somelib_g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_variable_absolute_identifier_for_private_variable()
      {
        istringstream iss("\
module somelib {\n\
  private a = 1\n\
\n\
  f() = .somelib.a\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "a" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_f_abs_ident(list<string> { "somelib", "f" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
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
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          AbsoluteIdentifier *abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != abs_ident1);
          CPPUNIT_ASSERT_EQUAL(true, abs_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == abs_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_variable_relative_identifier_for_private_variable()
      {
        istringstream iss("\
module somelib {\n\
  private a = 1\n\
\n\
  f() = a\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "a" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_f_abs_ident(list<string> { "somelib", "f" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
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
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_variable_absolute_identifiers()
      {
        istringstream iss("\
module somelib {\n\
  module module1 {\n\
    datatype A\n\
  }\n\
\n\
  datatype B\n\
}\n\
\n\
module somelib2 {\n\
  type T = .somelib.module1.A\n\
\n\
  type U = .somelib.B\n\
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
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_a_abs_ident(list<string> { "somelib", "module1", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_b_abs_ident(list<string> { "somelib", "B" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_t_abs_ident(list<string> { "somelib2", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_u_abs_ident(list<string> { "somelib2", "U" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_u_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_module1_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib2_t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          AbsoluteIdentifier *type_abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_abs_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_abs_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_module1_a_abs_ident.key_ident() == type_abs_ident1->key_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib2_u_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          AbsoluteIdentifier *type_abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_abs_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_abs_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_b_abs_ident.key_ident() == type_abs_ident1->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_module1_a_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_b_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT(somelib2_t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[2]);
        CPPUNIT_ASSERT(somelib2_u_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_variable_relative_identifiers_for_current_module()
      {
        istringstream iss("\
module somelib {\n\
  datatype A\n\
  \n\
  module module1 {\n\
    datatype B\n\
  }\n\
}\n\
\n\
module predef {\n\
  datatype C\n\
}\n\
\n\
import somelib\n\
\n\
module somelib2 {\n\
  datatype A\n\
  \n\
  module module1 {\n\
    datatype B\n\
  }\n\
\n\
  datatype C\n\
\n\
  type T = A\n\
\n\
  type U = module1.B\n\
\n\
  type V = C\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_b_abs_ident(list<string> { "somelib", "module1", "B" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_abs_ident(list<string> { "predef" });
        CPPUNIT_ASSERT_EQUAL(true, predef_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_c_abs_ident(list<string> { "predef", "C" });
        CPPUNIT_ASSERT_EQUAL(true, predef_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_a_abs_ident(list<string> { "somelib2", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module1_abs_ident(list<string> { "somelib2", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module1_b_abs_ident(list<string> { "somelib2", "module1", "B" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module1_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_c_abs_ident(list<string> { "somelib2", "C" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_t_abs_ident(list<string> { "somelib2", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_u_abs_ident(list<string> { "somelib2", "U" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_u_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_v_abs_ident(list<string> { "somelib2", "V" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_module1_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(predef_c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module1_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib2_module1_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib2_c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib2_t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib2_a_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib2_u_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib2_module1_b_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib2_v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib2_c_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_module1_b_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT(predef_c_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[2]);
        CPPUNIT_ASSERT(somelib2_a_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[3]);
        CPPUNIT_ASSERT(somelib2_module1_b_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[4]);
        CPPUNIT_ASSERT(somelib2_c_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[5]);
        CPPUNIT_ASSERT(somelib2_t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[6]);
        CPPUNIT_ASSERT(somelib2_u_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[7]);
        CPPUNIT_ASSERT(somelib2_v_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[8]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_variable_relative_identifiers_for_imported_modules()
      {
        istringstream iss("\
module somelib {\n\
  datatype A\n\
\n\
  module module1 {\
    datatype B\n\
  }\n\
\n\
  datatype C\n\
}\n\
\n\
module predef {\n\
  datatype C\n\
}\n\
\n\
module somelib2 {\n\
  module module2 {\n\
    datatype A\n\
  }\n\
  \n\
  module module3 {\n\
    datatype A\n\
  }\n\
}\n\
\n\
import somelib\n\
\n\
module somelib3 {\n\
  import .somelib2.module2\n\
  import .somelib2.module3\n\
\n\
  type T = A\n\
\n\
  type U = module1.B\n\
\n\
  type V = C\n\
}\n\
\n\
module somelib4 {\n\
  type W = A\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_b_abs_ident(list<string> { "somelib", "module1", "B" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_c_abs_ident(list<string> { "somelib", "C" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_abs_ident(list<string> { "predef" });
        CPPUNIT_ASSERT_EQUAL(true, predef_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_c_abs_ident(list<string> { "predef", "C" });
        CPPUNIT_ASSERT_EQUAL(true, predef_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module2_abs_ident(list<string> { "somelib2", "module2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module2_a_abs_ident(list<string> { "somelib2", "module2", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module2_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module3_abs_ident(list<string> { "somelib2", "module3" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module3_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module3_a_abs_ident(list<string> { "somelib2", "module3", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module3_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_abs_ident(list<string> { "somelib3" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_t_abs_ident(list<string> { "somelib3", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_u_abs_ident(list<string> { "somelib3", "U" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_u_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_v_abs_ident(list<string> { "somelib3", "V" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib4_abs_ident(list<string> { "somelib4" });
        CPPUNIT_ASSERT_EQUAL(true, somelib4_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib4_w_abs_ident(list<string> { "somelib4", "W" });
        CPPUNIT_ASSERT_EQUAL(true, somelib4_w_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_module1_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(predef_c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module2_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib2_module2_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module3_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib2_module3_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib3_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib3_t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib2_module3_a_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib3_u_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_module1_b_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib3_v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_c_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib4_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib4_w_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_module1_b_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT(somelib_c_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[2]);
        CPPUNIT_ASSERT(predef_c_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[3]);
        CPPUNIT_ASSERT(somelib2_module2_a_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[4]);
        CPPUNIT_ASSERT(somelib2_module3_a_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[5]);
        CPPUNIT_ASSERT(somelib3_t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[6]);
        CPPUNIT_ASSERT(somelib3_u_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[7]);
        CPPUNIT_ASSERT(somelib3_v_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[8]);
        CPPUNIT_ASSERT(somelib4_w_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[9]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_variable_relative_identifiers_for_predef_module()
      {
        istringstream iss("\
module predef {\n\
  module module1 {\n\
    datatype A\n\
  }\n\
\n\
  datatype B\n\
}\n\
\n\
module somelib {\n\
  type T = module1.A\n\
\n\
  type U = B\n\
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
        AbsoluteIdentifier predef_abs_ident(list<string> { "predef" });
        CPPUNIT_ASSERT_EQUAL(true, predef_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_module1_abs_ident(list<string> { "predef", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, predef_module1_abs_ident.set_key_ident(*(tree.ident_table())));        
        AbsoluteIdentifier predef_module1_a_abs_ident(list<string> { "predef", "module1", "A" });
        CPPUNIT_ASSERT_EQUAL(true, predef_module1_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_b_abs_ident(list<string> { "predef", "B" });
        CPPUNIT_ASSERT_EQUAL(true, predef_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_abs_ident(list<string> { "somelib" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_t_abs_ident(list<string> { "somelib", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_u_abs_ident(list<string> { "somelib", "U" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_u_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_module1_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(predef_module1_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(predef_b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(predef_module1_a_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_u_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(predef_b_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(predef_module1_a_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(predef_b_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT(somelib_t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[2]);
        CPPUNIT_ASSERT(somelib_u_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_variable_absolute_identifier_for_private_type_variable()
      {
        istringstream iss("\
module somelib {\n\
  private datatype A\n\
\n\
  type T = .somelib.A\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_t_abs_ident(list<string> { "somelib", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          AbsoluteIdentifier *type_abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_abs_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_abs_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == type_abs_ident1->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_variable_relative_identifier_for_private_type_variable()
      {
        istringstream iss("\
module somelib {\n\
  private datatype A\n\
\n\
  type T = A\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_t_abs_ident(list<string> { "somelib", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(true, datatype->constrs().empty());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(somelib_t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          TypeSynonymVariable *type_synonym_var = dynamic_cast<TypeSynonymVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != type_synonym_var);
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(type_synonym_var->expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == type_rel_ident1->key_ident());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_function_absolute_identifiers()
      {
        istringstream iss("\
module somelib {\n\
  template\n\
  datatype A(t)\n\
\n\
  module module1 {\n\
    template\n\
    datatype B(t)\n\
  }\n\
}\n\
\n\
module somelib2 {\n\
  template\n\
  type T(t) = .somelib.A(t)\n\
\n\
  template\n\
  type U(t) = .somelib.module1.B(t)\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_b_abs_ident(list<string> { "somelib", "module1", "B" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_t_abs_ident(list<string> { "somelib2", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_u_abs_ident(list<string> { "somelib2", "U" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_u_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_a_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_module1_b_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_abs_ident.key_ident()));        
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_t_abs_ident.key_ident());
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
          AbsoluteIdentifier *type_abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_abs_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_abs_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == type_abs_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_u_abs_ident.key_ident());
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
          AbsoluteIdentifier *type_abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_abs_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_abs_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_module1_b_abs_ident.key_ident() == type_abs_ident1->key_ident());          
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
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_module1_b_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[1]);
        CPPUNIT_ASSERT(somelib2_t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[2]);
        CPPUNIT_ASSERT(somelib2_u_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_function_relative_identifiers_for_current_module()
      {
        istringstream iss("\
module somelib {\n\
  template\n\
  datatype A(t)\n\
\n\
  module module1 {\n\
    template\n\
    datatype C(t)\n\
  }\n\
}\n\
\n\
module predef {\n\
  template\n\
  datatype B(t)\n\
}\n\
\n\
import somelib\n\
\n\
module somelib2 {\n\
  template\n\
  datatype A(t)\n\
\n\
  template\n\
  datatype B(t)\n\
\n\
  module module1 {\n\
    template\n\
    datatype C(t)\n\
  }\n\
\n\
  template\n\
  type T(t) = A(t)\n\
\n\
  template\n\
  type U(t) = B(t)\n\
\n\
  template\n\
  type V(t) = module1.C(t)\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_c_abs_ident(list<string> { "somelib", "module1", "C" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_abs_ident(list<string> { "predef" });
        CPPUNIT_ASSERT_EQUAL(true, predef_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_b_abs_ident(list<string> { "predef", "B" });
        CPPUNIT_ASSERT_EQUAL(true, predef_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_a_abs_ident(list<string> { "somelib2", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_b_abs_ident(list<string> { "somelib2", "B" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module1_abs_ident(list<string> { "somelib2", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module1_c_abs_ident(list<string> { "somelib2", "module1", "C" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module1_c_abs_ident.set_key_ident(*(tree.ident_table())));        
        AbsoluteIdentifier somelib2_t_abs_ident(list<string> { "somelib2", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_u_abs_ident(list<string> { "somelib2", "U" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_u_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_v_abs_ident(list<string> { "somelib2", "V" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_a_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_module1_c_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(predef_b_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_a_abs_ident.key_ident());
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
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_b_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module1_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_module1_c_abs_ident.key_ident());
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
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_t_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(somelib2_a_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_u_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(somelib2_b_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_v_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(somelib2_module1_c_abs_ident.key_ident() == type_rel_ident1->key_ident());          
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
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(9), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_module1_c_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[1]);
        CPPUNIT_ASSERT(predef_b_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[2]);
        CPPUNIT_ASSERT(somelib2_a_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[3]);
        CPPUNIT_ASSERT(somelib2_b_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[4]);
        CPPUNIT_ASSERT(somelib2_module1_c_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[5]);
        CPPUNIT_ASSERT(somelib2_t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[6]);
        CPPUNIT_ASSERT(somelib2_u_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[7]);
        CPPUNIT_ASSERT(somelib2_v_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[8]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_function_relative_identifiers_for_imported_modules()
      {
        istringstream iss("\
module somelib {\n\
  module module1 {\n\
    template\n\
    datatype A(t)\n\
  }\n\
\n\
  template\n\
  datatype B(t)\n\
\n\
  template\n\
  datatype C(t)\n\
}\n\
\n\
module predef {\n\
  template\n\
  datatype C(t)\n\
}\n\
\n\
module somelib2 {\n\
  module module2 {\n\
    template\n\
    datatype B(t)\n\
  }\n\
  \n\
  module module3 {\n\
    template\n\
    datatype B(t)\n\
  }\n\
}\n\
\n\
import somelib\n\
\n\
module somelib3 {\n\
  import .somelib2.module2\n\
  import .somelib2.module3\n\
\n\
  template\n\
  type T(t) = module1.A(t)\n\
\n\
  template\n\
  type U(t) = B(t)\n\
\n\
  template\n\
  type V(t) = C(t)\n\
}\n\
\n\
module somelib4 {\n\
  template\n\
  type W(t) = B(t)\n\
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
        AbsoluteIdentifier somelib_module1_abs_ident(list<string> { "somelib", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_module1_a_abs_ident(list<string> { "somelib", "module1", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_module1_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_b_abs_ident(list<string> { "somelib", "B" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_c_abs_ident(list<string> { "somelib", "C" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_abs_ident(list<string> { "predef" });
        CPPUNIT_ASSERT_EQUAL(true, predef_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_c_abs_ident(list<string> { "predef", "C" });
        CPPUNIT_ASSERT_EQUAL(true, predef_c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_abs_ident(list<string> { "somelib2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module2_abs_ident(list<string> { "somelib2", "module2" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_abs_ident.set_key_ident(*(tree.ident_table())));        
        AbsoluteIdentifier somelib2_module2_b_abs_ident(list<string> { "somelib2", "module2", "B" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module2_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module3_abs_ident(list<string> { "somelib2", "module3" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module3_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib2_module3_b_abs_ident(list<string> { "somelib2", "module3", "B" });
        CPPUNIT_ASSERT_EQUAL(true, somelib2_module3_b_abs_ident.set_key_ident(*(tree.ident_table())));        
        AbsoluteIdentifier somelib3_abs_ident(list<string> { "somelib3" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_t_abs_ident(list<string> { "somelib3", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_u_abs_ident(list<string> { "somelib3", "U" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_u_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib3_v_abs_ident(list<string> { "somelib3", "V" });
        CPPUNIT_ASSERT_EQUAL(true, somelib3_v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib4_abs_ident(list<string> { "somelib4" });
        CPPUNIT_ASSERT_EQUAL(true, somelib4_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib4_w_abs_ident(list<string> { "somelib4", "W" });
        CPPUNIT_ASSERT_EQUAL(true, somelib4_w_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_module1_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_module1_a_abs_ident.key_ident());
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
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_b_abs_ident.key_ident());
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
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_c_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(predef_c_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module2_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_module2_b_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib2_module3_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib2_module3_b_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib3_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib3_t_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(somelib_module1_a_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib3_u_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(somelib2_module3_b_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib3_v_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(somelib_c_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib4_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib4_w_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(somelib_b_abs_ident.key_ident() == type_rel_ident1->key_ident());          
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
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(somelib_module1_a_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_b_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[1]);
        CPPUNIT_ASSERT(somelib_c_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[2]);
        CPPUNIT_ASSERT(predef_c_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[3]);
        CPPUNIT_ASSERT(somelib2_module2_b_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[4]);
        CPPUNIT_ASSERT(somelib2_module3_b_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[5]);
        CPPUNIT_ASSERT(somelib3_t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[6]);
        CPPUNIT_ASSERT(somelib3_u_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[7]);
        CPPUNIT_ASSERT(somelib3_v_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[8]);
        CPPUNIT_ASSERT(somelib4_w_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[9]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }

      void ResolverTests::test_resolver_resolves_type_function_relative_identifiers_for_predef_module()
      {
        istringstream iss("\
module predef {\n\
  template\n\
  datatype A(t)\n\
\n\
  module module1 {\n\
    template\n\
    datatype B(t)\n\
  }\n\
}\n\
\n\
module somelib {\n\
  template\n\
  type T(t) = A(t)\n\
\n\
  template\n\
  type U(t) = module1.B(t)\n\
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
        AbsoluteIdentifier predef_abs_ident(list<string> { "predef" });
        CPPUNIT_ASSERT_EQUAL(true, predef_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_a_abs_ident(list<string> { "predef", "A" });
        CPPUNIT_ASSERT_EQUAL(true, predef_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_module1_abs_ident(list<string> { "predef", "module1" });
        CPPUNIT_ASSERT_EQUAL(true, predef_module1_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier predef_module1_b_abs_ident(list<string> { "predef", "module1", "B" });
        CPPUNIT_ASSERT_EQUAL(true, predef_module1_b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_abs_ident(list<string> { "somelib" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_t_abs_ident(list<string> { "somelib", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_u_abs_ident(list<string> { "somelib", "U" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_u_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(predef_a_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(predef_module1_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(predef_module1_b_abs_ident.key_ident());
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
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_t_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(predef_a_abs_ident.key_ident() == type_rel_ident1->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app1->args().size());
          auto type_arg_iter1 = type_app1->args().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(type_arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }        
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_u_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(predef_module1_b_abs_ident.key_ident() == type_rel_ident1->key_ident());          
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
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(predef_a_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT(predef_module1_b_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[1]);
        CPPUNIT_ASSERT(somelib_t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[2]);
        CPPUNIT_ASSERT(somelib_u_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_function_absolute_identifier_for_private_type_function()
      {
        istringstream iss("\
module somelib {\n\
  template\n\
  private datatype A(t)\n\
\n\
  template\n\
  type T(t) = .somelib.A(t)\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_t_abs_ident(list<string> { "somelib", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_info->access_modifier());
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
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_t_abs_ident.key_ident());
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
          AbsoluteIdentifier *type_abs_ident1 = dynamic_cast<AbsoluteIdentifier *>(type_app1->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_abs_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_abs_ident1->has_key_ident());
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == type_abs_ident1->key_ident());          
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
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_type_function_relative_identifier_for_private_type_function()
      {
        istringstream iss("\
module somelib {\n\
  template\n\
  private datatype A(t)\n\
\n\
  template\n\
  type T(t) = A(t)\n\
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
        AbsoluteIdentifier somelib_a_abs_ident(list<string> { "somelib", "A" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier somelib_t_abs_ident(list<string> { "somelib", "T" });
        CPPUNIT_ASSERT_EQUAL(true, somelib_t_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(somelib_abs_ident.key_ident()));
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_fun_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::PRIVATE, type_fun_info->access_modifier());
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
        {
          TypeFunctionInfo *type_fun_info = tree.type_fun_info(somelib_t_abs_ident.key_ident());
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
          CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == type_rel_ident1->key_ident());          
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
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(somelib_a_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT(somelib_t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_arguments()
      {
        istringstream iss("\
f(x, y, z) = #iadd(#iadd(x, y), z)\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
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
          arg_iter1++;
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident5->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_arguments_and_covered_global_variable()
      {
        istringstream iss("\
x = 1\n\
\n\
f(x, y) = #iadd(x, y)\n\
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
        AbsoluteIdentifier x_abs_ident(list<string> { "x" });
        CPPUNIT_ASSERT_EQUAL(true, x_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(x_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(x_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_local_variables_and_let_expression()
      {
        istringstream iss("\
f(x) =\n\
  let y = 1\n\
      z = 2\n\
  in  #iadd(#iadd(x, y), z)\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Let *let1 = dynamic_cast<Let *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != let1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), let1->binds().size());
          auto bind_iter1 = let1->binds().begin();
          VariableBinding *var_bind2 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_bind2->index());
          Literal *literal2 = dynamic_cast<Literal *>(var_bind2->expr());
          CPPUNIT_ASSERT(nullptr != literal2);
          bind_iter1++;
          VariableBinding *var_bind3 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_bind3->index());
          Literal *literal3 = dynamic_cast<Literal *>(var_bind3->expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          BuiltinApplication *builtin_app4 = dynamic_cast<BuiltinApplication *>(let1->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app4->args().size());
          auto arg_iter4 = builtin_app4->args().begin();
          BuiltinApplication *builtin_app5 = dynamic_cast<BuiltinApplication *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != builtin_app5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app5->args().size());
          auto arg_iter5 = builtin_app5->args().begin();
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident6->index());
          arg_iter5++;
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident7->index());
          arg_iter4++;
          VariableExpression *var_expr8 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr8);
          RelativeIdentifier *rel_ident8 = dynamic_cast<RelativeIdentifier *>(var_expr8->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident8);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident8->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident8->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_local_variables_and_let_expression_and_tuple_binding()
      {
        istringstream iss("\
f(x) =\n\
  let (y, z, _) = (1, 2, 3)\n\
  in  #iadd(#iadd(x, y), z)\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Let *let1 = dynamic_cast<Let *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != let1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), let1->binds().size());
          auto bind_iter1 = let1->binds().begin();
          TupleBinding *tuple_bind1 = dynamic_cast<TupleBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != tuple_bind1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tuple_bind1->vars().size());
          auto var_iter1 = tuple_bind1->vars().begin();
          CPPUNIT_ASSERT(nullptr != var_iter1->get());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*var_iter1)->index());
          var_iter1++;
          CPPUNIT_ASSERT(nullptr != var_iter1->get());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*var_iter1)->index());
          var_iter1++;
          CPPUNIT_ASSERT(nullptr == var_iter1->get());
          Tuple *tuple2 = dynamic_cast<Tuple *>(tuple_bind1->expr());
          CPPUNIT_ASSERT(nullptr != tuple2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tuple2->fields().size());
          auto field_iter2 = tuple2->fields().begin();
          Literal *literal3 = dynamic_cast<Literal *>(field_iter2->get());
          CPPUNIT_ASSERT(nullptr != literal3);
          field_iter2++;
          Literal *literal4 = dynamic_cast<Literal *>(field_iter2->get());
          CPPUNIT_ASSERT(nullptr != literal4);
          field_iter2++;
          Literal *literal5 = dynamic_cast<Literal *>(field_iter2->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          BuiltinApplication *builtin_app6 = dynamic_cast<BuiltinApplication *>(let1->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app6);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app6->args().size());
          auto arg_iter6 = builtin_app6->args().begin();
          BuiltinApplication *builtin_app7 = dynamic_cast<BuiltinApplication *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != builtin_app7);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app7->args().size());
          auto arg_iter7 = builtin_app7->args().begin();
          VariableExpression *var_expr8 = dynamic_cast<VariableExpression *>(arg_iter7->get());
          CPPUNIT_ASSERT(nullptr != var_expr8);
          RelativeIdentifier *rel_ident8 = dynamic_cast<RelativeIdentifier *>(var_expr8->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident8);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident8->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident8->index());
          arg_iter7++;
          VariableExpression *var_expr9 = dynamic_cast<VariableExpression *>(arg_iter7->get());
          CPPUNIT_ASSERT(nullptr != var_expr9);
          RelativeIdentifier *rel_ident9 = dynamic_cast<RelativeIdentifier *>(var_expr9->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident9);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident9->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident9->index());
          arg_iter6++;
          VariableExpression *var_expr10 = dynamic_cast<VariableExpression *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr10);
          RelativeIdentifier *rel_ident10 = dynamic_cast<RelativeIdentifier *>(var_expr10->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident10);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident10->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident10->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }

      void ResolverTests::test_resolver_resolves_identifiers_for_covered_local_variables_and_let_expression()
      {
        istringstream iss("\
f(x, y) =\n\
  #iadd(\n\
    let x = 1\n\
        y = 2\n\
    in  #iadd(x, y), #iadd(x, y))\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          Let *let2 = dynamic_cast<Let *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != let2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), let2->binds().size());
          auto bind_iter2 = let2->binds().begin();
          VariableBinding *var_bind3 = dynamic_cast<VariableBinding *>(bind_iter2->get());          
          CPPUNIT_ASSERT(nullptr != var_bind3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_bind3->index());
          Literal *literal3 = dynamic_cast<Literal *>(var_bind3->expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          bind_iter2++;
          VariableBinding *var_bind4 = dynamic_cast<VariableBinding *>(bind_iter2->get());          
          CPPUNIT_ASSERT(nullptr != var_bind4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_bind4->index());
          Literal *literal4 = dynamic_cast<Literal *>(var_bind4->expr());
          CPPUNIT_ASSERT(nullptr != literal4);
          BuiltinApplication *builtin_app5 = dynamic_cast<BuiltinApplication *>(let2->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app5->args().size());
          auto arg_iter5 = builtin_app5->args().begin();
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident6->index());
          arg_iter5++;
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), rel_ident7->index());
          arg_iter1++;
          BuiltinApplication *builtin_app8 = dynamic_cast<BuiltinApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != builtin_app8);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app8->args().size());
          auto arg_iter8 = builtin_app8->args().begin();
          VariableExpression *var_expr9 = dynamic_cast<VariableExpression *>(arg_iter8->get());
          CPPUNIT_ASSERT(nullptr != var_expr9);
          RelativeIdentifier *rel_ident9 = dynamic_cast<RelativeIdentifier *>(var_expr9->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident9);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident9->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident9->index());
          arg_iter8++;
          VariableExpression *var_expr10 = dynamic_cast<VariableExpression *>(arg_iter8->get());
          CPPUNIT_ASSERT(nullptr != var_expr10);
          RelativeIdentifier *rel_ident10 = dynamic_cast<RelativeIdentifier *>(var_expr10->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident10);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident10->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident10->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_covered_local_variables_and_nested_let_expression()
      {
        istringstream iss("\
f(x, y) =\n\
  #iadd(\n\
    let x = 1\n\
        y = 2\n\
    in  #iadd(\n\
          let x = 3\n\
              y = 4\n\
          in #iadd(x, y), #iadd(x, y)), #iadd(x, y))\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
          auto arg_iter1 = builtin_app1->args().begin();
          Let *let2 = dynamic_cast<Let *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != let2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), let2->binds().size());
          auto bind_iter2 = let2->binds().begin();
          VariableBinding *var_bind3 = dynamic_cast<VariableBinding *>(bind_iter2->get());          
          CPPUNIT_ASSERT(nullptr != var_bind3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_bind3->index());
          Literal *literal3 = dynamic_cast<Literal *>(var_bind3->expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          bind_iter2++;
          VariableBinding *var_bind4 = dynamic_cast<VariableBinding *>(bind_iter2->get());          
          CPPUNIT_ASSERT(nullptr != var_bind4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_bind4->index());
          Literal *literal4 = dynamic_cast<Literal *>(var_bind4->expr());
          CPPUNIT_ASSERT(nullptr != literal4);
          BuiltinApplication *builtin_app5 = dynamic_cast<BuiltinApplication *>(let2->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app5->args().size());
          auto arg_iter5 = builtin_app5->args().begin();
          Let *let6 = dynamic_cast<Let *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != let6);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), let6->binds().size());
          auto bind_iter6 = let6->binds().begin();
          VariableBinding *var_bind7 = dynamic_cast<VariableBinding *>(bind_iter6->get());          
          CPPUNIT_ASSERT(nullptr != var_bind7);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_bind7->index());
          Literal *literal7 = dynamic_cast<Literal *>(var_bind7->expr());
          CPPUNIT_ASSERT(nullptr != literal7);
          bind_iter6++;
          VariableBinding *var_bind8 = dynamic_cast<VariableBinding *>(bind_iter6->get());          
          CPPUNIT_ASSERT(nullptr != var_bind8);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_bind8->index());
          Literal *literal8 = dynamic_cast<Literal *>(var_bind8->expr());
          CPPUNIT_ASSERT(nullptr != literal8);
          BuiltinApplication *builtin_app9 = dynamic_cast<BuiltinApplication *>(let6->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app9);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app9->args().size());
          auto arg_iter9 = builtin_app9->args().begin();
          VariableExpression *var_expr10 = dynamic_cast<VariableExpression *>(arg_iter9->get());
          CPPUNIT_ASSERT(nullptr != var_expr10);
          RelativeIdentifier *rel_ident10 = dynamic_cast<RelativeIdentifier *>(var_expr10->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident10);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident10->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), rel_ident10->index());
          arg_iter9++;
          VariableExpression *var_expr11 = dynamic_cast<VariableExpression *>(arg_iter9->get());
          CPPUNIT_ASSERT(nullptr != var_expr11);
          RelativeIdentifier *rel_ident11 = dynamic_cast<RelativeIdentifier *>(var_expr11->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident11);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident11->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), rel_ident11->index());
          arg_iter5++;
          BuiltinApplication *builtin_app12 = dynamic_cast<BuiltinApplication *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != builtin_app12);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app12->args().size());
          auto arg_iter12 = builtin_app12->args().begin();
          VariableExpression *var_expr13 = dynamic_cast<VariableExpression *>(arg_iter12->get());
          CPPUNIT_ASSERT(nullptr != var_expr13);
          RelativeIdentifier *rel_ident13 = dynamic_cast<RelativeIdentifier *>(var_expr13->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident13);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident13->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident13->index());
          arg_iter12++;
          VariableExpression *var_expr14 = dynamic_cast<VariableExpression *>(arg_iter12->get());
          CPPUNIT_ASSERT(nullptr != var_expr14);
          RelativeIdentifier *rel_ident14 = dynamic_cast<RelativeIdentifier *>(var_expr14->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident14);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident14->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), rel_ident14->index());
          arg_iter1++;
          BuiltinApplication *builtin_app15 = dynamic_cast<BuiltinApplication *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != builtin_app15);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app15->args().size());
          auto arg_iter15 = builtin_app15->args().begin();
          VariableExpression *var_expr16 = dynamic_cast<VariableExpression *>(arg_iter15->get());
          CPPUNIT_ASSERT(nullptr != var_expr16);
          RelativeIdentifier *rel_ident16 = dynamic_cast<RelativeIdentifier *>(var_expr16->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident16);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident16->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident16->index());
          arg_iter15++;
          VariableExpression *var_expr17 = dynamic_cast<VariableExpression *>(arg_iter15->get());
          CPPUNIT_ASSERT(nullptr != var_expr17);
          RelativeIdentifier *rel_ident17 = dynamic_cast<RelativeIdentifier *>(var_expr17->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident17);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident17->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident17->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_used_local_variables_in_other_variables_and_let_expression()
      {
        istringstream iss("\
f() =\n\
  let x = a\n\
      y = b\n\
      a = 1\n\
      b = 2\n\
  in  #iadd(x, y)\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Let *let1 = dynamic_cast<Let *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != let1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), let1->binds().size());
          auto bind_iter1 = let1->binds().begin();
          VariableBinding *var_bind2 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), var_bind2->index());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(var_bind2->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident2->index());
          bind_iter1++;
          VariableBinding *var_bind3 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_bind3->index());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(var_bind3->expr());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), rel_ident3->index());
          bind_iter1++;
          VariableBinding *var_bind4 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_bind4->index());
          Literal *literal4 = dynamic_cast<Literal *>(var_bind4->expr());
          CPPUNIT_ASSERT(nullptr != literal4);
          bind_iter1++;
          VariableBinding *var_bind5 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_bind5->index());
          Literal *literal5 = dynamic_cast<Literal *>(var_bind5->expr());
          CPPUNIT_ASSERT(nullptr != literal5);
          BuiltinApplication *builtin_app6 = dynamic_cast<BuiltinApplication *>(let1->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app6);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app6->args().size());
          auto arg_iter6 = builtin_app6->args().begin();
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident7->index());
          arg_iter6++;
          VariableExpression *var_expr8 = dynamic_cast<VariableExpression *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr8);
          RelativeIdentifier *rel_ident8 = dynamic_cast<RelativeIdentifier *>(var_expr8->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident8);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident8->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident8->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }

      void ResolverTests::test_resolver_resolves_identifiers_for_local_variable_and_let_expression_and_recursive_lambda()
      {
        istringstream iss("\
module stdlib {\n\
  datatype Bool = False | True\n\
}\n\
\n\
f() =\n\
  let g = \\(x) -> if(#ile(x, 1)) x else #imul(x, g(#isub(x, 1)))\n\
  in  g(5)\n\
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
        AbsoluteIdentifier stdlib_abs_ident(list<string> { "stdlib" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_bool_abs_ident(list<string> { "stdlib", "Bool" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_bool_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_false_abs_ident(list<string> { "stdlib", "False" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_false_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_true_abs_ident(list<string> { "stdlib", "True" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_true_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(stdlib_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(stdlib_bool_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(stdlib_false_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(stdlib_bool_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(stdlib_true_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(stdlib_bool_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Let *let1 = dynamic_cast<Let *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != let1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), let1->binds().size());
          auto bind_iter1 = let1->binds().begin();
          VariableBinding *var_bind2 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), var_bind2->index());
          Literal *literal2 = dynamic_cast<Literal *>(var_bind2->expr());
          CPPUNIT_ASSERT(nullptr != literal2);
          NonUniqueLambdaValue *lambda_value2 = dynamic_cast<NonUniqueLambdaValue *>(literal2->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), lambda_value2->args().size());
          auto arg_iter2 = lambda_value2->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter2)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter2)->index());
          CPPUNIT_ASSERT(nullptr == lambda_value2->result_type_expr());
          Match *match3 = dynamic_cast<Match *>(lambda_value2->body());
          CPPUNIT_ASSERT(nullptr != match3);
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(match3->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident4->index());
          arg_iter3++;
          Literal *literal5 = dynamic_cast<Literal *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match3->cases().size());
          auto case_iter3 = match3->cases().begin();
          VariableConstructorPattern *var_constr_pattern6 = dynamic_cast<VariableConstructorPattern *>((*case_iter3)->pattern());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern6);
          AbsoluteIdentifier *abs_ident6 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern6->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident6);
          CPPUNIT_ASSERT_EQUAL(true, abs_ident6->has_key_ident());
          CPPUNIT_ASSERT(stdlib_true_abs_ident.key_ident() == abs_ident6->key_ident());
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>((*case_iter3)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident7->index());
          case_iter3++;
          VariableConstructorPattern *var_constr_pattern8 = dynamic_cast<VariableConstructorPattern *>((*case_iter3)->pattern());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern8);
          AbsoluteIdentifier *abs_ident8 = dynamic_cast<AbsoluteIdentifier *>(var_constr_pattern8->constr_ident());
          CPPUNIT_ASSERT(nullptr != abs_ident8);
          CPPUNIT_ASSERT_EQUAL(true, abs_ident8->has_key_ident());
          CPPUNIT_ASSERT(stdlib_false_abs_ident.key_ident() == abs_ident8->key_ident());
          BuiltinApplication *builtin_app9 = dynamic_cast<BuiltinApplication *>((*case_iter3)->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app9);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app9->args().size());
          auto arg_iter9 = builtin_app9->args().begin();
          VariableExpression *var_expr10 = dynamic_cast<VariableExpression *>(arg_iter9->get());
          CPPUNIT_ASSERT(nullptr != var_expr10);
          RelativeIdentifier *rel_ident10 = dynamic_cast<RelativeIdentifier *>(var_expr10->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident10);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident10->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident10->index());
          arg_iter9++;
          NonUniqueApplication *app11 = dynamic_cast<NonUniqueApplication *>(arg_iter9->get());
          CPPUNIT_ASSERT(nullptr != app11);
          VariableExpression *var_expr11 = dynamic_cast<VariableExpression *>(app11->fun());
          CPPUNIT_ASSERT(nullptr != var_expr11);
          RelativeIdentifier *rel_ident11 = dynamic_cast<RelativeIdentifier *>(var_expr11->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident11);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident11->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident11->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app11->args().size());
          auto arg_iter11 = app11->args().begin();
          BuiltinApplication *builtin_app12 = dynamic_cast<BuiltinApplication *>(arg_iter11->get());
          CPPUNIT_ASSERT(nullptr != builtin_app12);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app12->args().size());
          auto arg_iter12 = builtin_app12->args().begin();
          VariableExpression *var_expr13 = dynamic_cast<VariableExpression *>(arg_iter12->get());
          CPPUNIT_ASSERT(nullptr != var_expr13);
          RelativeIdentifier *rel_ident13 = dynamic_cast<RelativeIdentifier *>(var_expr13->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident13);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident13->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident13->index());
          arg_iter12++;
          Literal *literal14 = dynamic_cast<Literal *>(arg_iter12->get());
          CPPUNIT_ASSERT(nullptr != literal14);
          NonUniqueApplication *app15 = dynamic_cast<NonUniqueApplication *>(let1->expr());
          CPPUNIT_ASSERT(nullptr != app15);
          VariableExpression *var_expr15 = dynamic_cast<VariableExpression *>(app15->fun());
          CPPUNIT_ASSERT(nullptr != var_expr15);
          RelativeIdentifier *rel_ident15 = dynamic_cast<RelativeIdentifier *>(var_expr15->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident15);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident15->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident11->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), app15->args().size());
          auto arg_iter15 = app15->args().begin();
          Literal *literal16 = dynamic_cast<Literal *>(arg_iter15->get());
          CPPUNIT_ASSERT(nullptr != literal16);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(stdlib_false_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(stdlib_true_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(stdlib_bool_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_local_variables_and_match_expression()
      {
        istringstream iss("\
f(x) =\n\
  x match {\n\
    (y, z) -> (x, #iadd(y, z))\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          NonUniqueTuplePattern *tuple_pattern2 =  dynamic_cast<NonUniqueTuplePattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != tuple_pattern2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple_pattern2->field_patterns().size());
          auto field_pattern_iter2 = tuple_pattern2->field_patterns().begin();
          VariablePattern *var_pattern3 = dynamic_cast<VariablePattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_pattern3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern3->index());
          field_pattern_iter2++;
          VariablePattern *var_pattern4 = dynamic_cast<VariablePattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_pattern4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_pattern4->index());
          NonUniqueTuple *tuple5 = dynamic_cast<NonUniqueTuple *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != tuple5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple5->fields().size());
          auto field_iter5 = tuple5->fields().begin();
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(field_iter5->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident6->index());
          field_iter5++;
          BuiltinApplication *builtin_app7 = dynamic_cast<BuiltinApplication *>(field_iter5->get());
          CPPUNIT_ASSERT(nullptr != builtin_app7);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app7->args().size());
          auto arg_iter7 = builtin_app7->args().begin();
          VariableExpression *var_expr8 = dynamic_cast<VariableExpression *>(arg_iter7->get());
          CPPUNIT_ASSERT(nullptr != var_expr8);
          RelativeIdentifier *rel_ident8 = dynamic_cast<RelativeIdentifier *>(var_expr8->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident8);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident8->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident8->index());
          arg_iter7++;
          VariableExpression *var_expr9 = dynamic_cast<VariableExpression *>(arg_iter7->get());
          CPPUNIT_ASSERT(nullptr != var_expr9);
          RelativeIdentifier *rel_ident9 = dynamic_cast<RelativeIdentifier *>(var_expr9->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident9);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident9->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident9->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_covered_local_variables_and_match_expression()
      {
        istringstream iss("\
f(x, y) =\n\
  (x match {\n\
    (x, y) -> #iadd(x, y)\n\
  }, x, #iadd(y, 1))\n\
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
          NonUniqueTuple *tuple1 = dynamic_cast<NonUniqueTuple *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != tuple1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tuple1->fields().size());
          auto field_iter1 = tuple1->fields().begin();
          Match *match2 = dynamic_cast<Match *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != match2);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(match2->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), match2->cases().size());
          auto case_iter2 = match2->cases().begin();
          NonUniqueTuplePattern *tuple_pattern3 =  dynamic_cast<NonUniqueTuplePattern *>((*case_iter2)->pattern());
          CPPUNIT_ASSERT(nullptr != tuple_pattern3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple_pattern3->field_patterns().size());
          auto field_pattern_iter3 = tuple_pattern3->field_patterns().begin();
          VariablePattern *var_pattern4 = dynamic_cast<VariablePattern *>(field_pattern_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_pattern4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_pattern4->index());
          field_pattern_iter3++;
          VariablePattern *var_pattern5 = dynamic_cast<VariablePattern *>(field_pattern_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_pattern5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_pattern5->index());          
          BuiltinApplication *builtin_app6 = dynamic_cast<BuiltinApplication *>((*case_iter2)->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app6);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app6->args().size());
          auto arg_iter6 = builtin_app6->args().begin();
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident7->index());
          arg_iter6++;
          VariableExpression *var_expr8 = dynamic_cast<VariableExpression *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr8);
          RelativeIdentifier *rel_ident8 = dynamic_cast<RelativeIdentifier *>(var_expr8->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident8);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident8->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), rel_ident8->index());
          field_iter1++;
          VariableExpression *var_expr9 = dynamic_cast<VariableExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr9);
          RelativeIdentifier *rel_ident9 = dynamic_cast<RelativeIdentifier *>(var_expr9->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident9);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident9->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident9->index());
          field_iter1++;
          BuiltinApplication *builtin_app10 = dynamic_cast<BuiltinApplication *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != builtin_app10);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app10->args().size());
          auto arg_iter10 = builtin_app10->args().begin();
          VariableExpression *var_expr11 = dynamic_cast<VariableExpression *>(arg_iter10->get());
          CPPUNIT_ASSERT(nullptr != var_expr11);
          RelativeIdentifier *rel_ident11 = dynamic_cast<RelativeIdentifier *>(var_expr11->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident11);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident11->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident11->index());
          arg_iter10++;
          Literal *literal12 = dynamic_cast<Literal *>(arg_iter10->get());
          CPPUNIT_ASSERT(nullptr != literal12);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_covered_local_variables_and_nested_match_expression()
      {
        istringstream iss("\
f(x, y) =\n\
  (x match {\n\
    (x, y) ->\n\
      (x match {\n\
        (x, y) -> #iadd(x, y)\n\
      }, x,  #iadd(y, 2))\n\
  }, x, #iadd(y, 1))\n\
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
          NonUniqueTuple *tuple1 = dynamic_cast<NonUniqueTuple *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != tuple1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tuple1->fields().size());
          auto field_iter1 = tuple1->fields().begin();
          Match *match2 = dynamic_cast<Match *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != match2);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(match2->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), match2->cases().size());
          auto case_iter2 = match2->cases().begin();
          NonUniqueTuplePattern *tuple_pattern3 =  dynamic_cast<NonUniqueTuplePattern *>((*case_iter2)->pattern());
          CPPUNIT_ASSERT(nullptr != tuple_pattern3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple_pattern3->field_patterns().size());
          auto field_pattern_iter3 = tuple_pattern3->field_patterns().begin();
          VariablePattern *var_pattern4 = dynamic_cast<VariablePattern *>(field_pattern_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_pattern4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_pattern4->index());
          field_pattern_iter3++;
          VariablePattern *var_pattern5 = dynamic_cast<VariablePattern *>(field_pattern_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_pattern5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), var_pattern5->index());          
          NonUniqueTuple *tuple6 = dynamic_cast<NonUniqueTuple *>((*case_iter2)->expr());
          CPPUNIT_ASSERT(nullptr != tuple6);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tuple6->fields().size());
          auto field_iter6 = tuple6->fields().begin();
          Match *match7 = dynamic_cast<Match *>(field_iter6->get());
          CPPUNIT_ASSERT(nullptr != match7);
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(match7->expr());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident7->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), match7->cases().size());
          auto case_iter7 = match7->cases().begin();
          NonUniqueTuplePattern *tuple_pattern8 =  dynamic_cast<NonUniqueTuplePattern *>((*case_iter7)->pattern());
          CPPUNIT_ASSERT(nullptr != tuple_pattern8);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple_pattern8->field_patterns().size());
          auto field_pattern_iter8 = tuple_pattern8->field_patterns().begin();
          VariablePattern *var_pattern9 = dynamic_cast<VariablePattern *>(field_pattern_iter8->get());
          CPPUNIT_ASSERT(nullptr != var_pattern9);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), var_pattern9->index());
          field_pattern_iter8++;
          VariablePattern *var_pattern10 = dynamic_cast<VariablePattern *>(field_pattern_iter8->get());
          CPPUNIT_ASSERT(nullptr != var_pattern10);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), var_pattern10->index());          
          BuiltinApplication *builtin_app11 = dynamic_cast<BuiltinApplication *>((*case_iter7)->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app11);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app11->args().size());
          auto arg_iter11 = builtin_app11->args().begin();
          VariableExpression *var_expr12 = dynamic_cast<VariableExpression *>(arg_iter11->get());
          CPPUNIT_ASSERT(nullptr != var_expr12);
          RelativeIdentifier *rel_ident12 = dynamic_cast<RelativeIdentifier *>(var_expr12->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident12);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident12->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), rel_ident12->index());
          arg_iter11++;
          VariableExpression *var_expr13 = dynamic_cast<VariableExpression *>(arg_iter11->get());
          CPPUNIT_ASSERT(nullptr != var_expr13);
          RelativeIdentifier *rel_ident13 = dynamic_cast<RelativeIdentifier *>(var_expr13->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident13);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident13->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), rel_ident13->index());
          field_iter6++;
          VariableExpression *var_expr14 = dynamic_cast<VariableExpression *>(field_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr14);
          RelativeIdentifier *rel_ident14 = dynamic_cast<RelativeIdentifier *>(var_expr14->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident14);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident14->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident14->index());
          field_iter6++;
          BuiltinApplication *builtin_app15 = dynamic_cast<BuiltinApplication *>(field_iter6->get());
          CPPUNIT_ASSERT(nullptr != builtin_app15);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app15->args().size());
          auto arg_iter15 = builtin_app15->args().begin();
          VariableExpression *var_expr16 = dynamic_cast<VariableExpression *>(arg_iter15->get());
          CPPUNIT_ASSERT(nullptr != var_expr16);
          RelativeIdentifier *rel_ident16 = dynamic_cast<RelativeIdentifier *>(var_expr16->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident16);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident16->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), rel_ident16->index());
          arg_iter15++;
          Literal *literal17 = dynamic_cast<Literal *>(arg_iter15->get());
          CPPUNIT_ASSERT(nullptr != literal17);
          field_iter1++;
          VariableExpression *var_expr18 = dynamic_cast<VariableExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr18);
          RelativeIdentifier *rel_ident18 = dynamic_cast<RelativeIdentifier *>(var_expr18->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident18);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident18->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident18->index());
          field_iter1++;
          BuiltinApplication *builtin_app19 = dynamic_cast<BuiltinApplication *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != builtin_app19);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app19->args().size());
          auto arg_iter19 = builtin_app19->args().begin();
          VariableExpression *var_expr20 = dynamic_cast<VariableExpression *>(arg_iter19->get());
          CPPUNIT_ASSERT(nullptr != var_expr20);
          RelativeIdentifier *rel_ident20 = dynamic_cast<RelativeIdentifier *>(var_expr20->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident20);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident20->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident20->index());
          arg_iter19++;
          Literal *literal21 = dynamic_cast<Literal *>(arg_iter19->get());
          CPPUNIT_ASSERT(nullptr != literal21);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_local_variables_and_lambda_value()
      {
        istringstream iss("\
f(x) = \\(y, z) -> #iadd(#iadd(x, y), z)\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          NonUniqueLambdaValue *lambda_value1 = dynamic_cast<NonUniqueLambdaValue *>(literal1->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lambda_value1->args().size());
          auto arg_iter1 = lambda_value1->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter1)->index());
          arg_iter1++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter1)->index());
          CPPUNIT_ASSERT(nullptr == lambda_value1->result_type_expr());
          BuiltinApplication *builtin_app2 = dynamic_cast<BuiltinApplication *>(lambda_value1->body());
          CPPUNIT_ASSERT(nullptr != builtin_app2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app2->args().size());
          auto arg_iter2 = builtin_app2->args().begin();
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident4->index());
          arg_iter3++;
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident5->index());
          arg_iter2++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident6->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_covered_local_variables_and_lambda_value()
      {
        istringstream iss("\
f(x, y) = (\\(x, y) -> #iadd(x, y), #iadd(x, y))\n\
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
          NonUniqueTuple *tuple1 = dynamic_cast<NonUniqueTuple *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != tuple1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple1->fields().size());
          auto field_iter1 = tuple1->fields().begin();
          Literal *literal2 = dynamic_cast<Literal *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal2);
          NonUniqueLambdaValue *lambda_value2 = dynamic_cast<NonUniqueLambdaValue *>(literal2->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lambda_value2->args().size());
          auto arg_iter2 = lambda_value2->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter2)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter2)->index());
          arg_iter2++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter2)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter2)->index());
          CPPUNIT_ASSERT(nullptr == lambda_value2->result_type_expr());
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(lambda_value2->body());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident4->index());
          arg_iter3++;
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), rel_ident5->index());
          field_iter1++;
          BuiltinApplication *builtin_app6 = dynamic_cast<BuiltinApplication *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != builtin_app6);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app6->args().size());
          auto arg_iter6 = builtin_app6->args().begin();
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident7->index());
          arg_iter6++;
          VariableExpression *var_expr8 = dynamic_cast<VariableExpression *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr8);
          RelativeIdentifier *rel_ident8 = dynamic_cast<RelativeIdentifier *>(var_expr8->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident8);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident8->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident8->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_covered_local_variables_and_nested_lambda_value()
      {
        istringstream iss("\
f(x, y) = (\\(x, y) -> (\\(x, y) -> #iadd(x, y), #iadd(x, y)), #iadd(x, y))\n\
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
          NonUniqueTuple *tuple1 = dynamic_cast<NonUniqueTuple *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != tuple1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple1->fields().size());
          auto field_iter1 = tuple1->fields().begin();
          Literal *literal2 = dynamic_cast<Literal *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal2);
          NonUniqueLambdaValue *lambda_value2 = dynamic_cast<NonUniqueLambdaValue *>(literal2->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lambda_value2->args().size());
          auto arg_iter2 = lambda_value2->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter2)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter2)->index());
          arg_iter2++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter2)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter2)->index());
          CPPUNIT_ASSERT(nullptr == lambda_value2->result_type_expr());
          NonUniqueTuple *tuple3 = dynamic_cast<NonUniqueTuple *>(lambda_value2->body());
          CPPUNIT_ASSERT(nullptr != tuple3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple3->fields().size());
          auto field_iter3 = tuple3->fields().begin();
          Literal *literal4 = dynamic_cast<Literal *>(field_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal4);
          NonUniqueLambdaValue *lambda_value4 = dynamic_cast<NonUniqueLambdaValue *>(literal4->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lambda_value4->args().size());
          auto arg_iter4 = lambda_value4->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter4)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), (*arg_iter4)->index());
          arg_iter4++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter4)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), (*arg_iter4)->index());
          CPPUNIT_ASSERT(nullptr == lambda_value2->result_type_expr());
          BuiltinApplication *builtin_app5 = dynamic_cast<BuiltinApplication *>(lambda_value4->body());
          CPPUNIT_ASSERT(nullptr != builtin_app5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app5->args().size());
          auto arg_iter5 = builtin_app5->args().begin();
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), rel_ident6->index());
          arg_iter5++;
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(5), rel_ident7->index());
          field_iter3++;
          BuiltinApplication *builtin_app8 = dynamic_cast<BuiltinApplication *>(field_iter3->get());
          CPPUNIT_ASSERT(nullptr != builtin_app8);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app8->args().size());
          auto arg_iter8 = builtin_app8->args().begin();
          VariableExpression *var_expr9 = dynamic_cast<VariableExpression *>(arg_iter8->get());
          CPPUNIT_ASSERT(nullptr != var_expr9);
          RelativeIdentifier *rel_ident9 = dynamic_cast<RelativeIdentifier *>(var_expr9->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident9);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident9->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident9->index());
          arg_iter8++;
          VariableExpression *var_expr10 = dynamic_cast<VariableExpression *>(arg_iter8->get());
          CPPUNIT_ASSERT(nullptr != var_expr10);
          RelativeIdentifier *rel_ident10 = dynamic_cast<RelativeIdentifier *>(var_expr10->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident10);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident10->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), rel_ident10->index());
          field_iter1++;
          BuiltinApplication *builtin_app11 = dynamic_cast<BuiltinApplication *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != builtin_app11);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app11->args().size());
          auto arg_iter11 = builtin_app11->args().begin();
          VariableExpression *var_expr12 = dynamic_cast<VariableExpression *>(arg_iter11->get());
          CPPUNIT_ASSERT(nullptr != var_expr12);
          RelativeIdentifier *rel_ident12 = dynamic_cast<RelativeIdentifier *>(var_expr12->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident12);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident12->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident12->index());
          arg_iter11++;
          VariableExpression *var_expr13 = dynamic_cast<VariableExpression *>(arg_iter11->get());
          CPPUNIT_ASSERT(nullptr != var_expr13);
          RelativeIdentifier *rel_ident13 = dynamic_cast<RelativeIdentifier *>(var_expr13->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident13);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident13->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident13->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_covered_local_variables_and_primitive_lambda_value()
      {
        istringstream iss("\
f(x, y) = (primitive \\(x, y) -> #iadd(x, y), #iadd(x, y))\n\
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
          NonUniqueTuple *tuple1 = dynamic_cast<NonUniqueTuple *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != tuple1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple1->fields().size());
          auto field_iter1 = tuple1->fields().begin();
          Literal *literal2 = dynamic_cast<Literal *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != literal2);
          NonUniqueLambdaValue *lambda_value2 = dynamic_cast<NonUniqueLambdaValue *>(literal2->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lambda_value2->args().size());
          auto arg_iter2 = lambda_value2->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter2)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter2)->index());
          arg_iter2++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter2)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), (*arg_iter2)->index());
          CPPUNIT_ASSERT(nullptr == lambda_value2->result_type_expr());
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(lambda_value2->body());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident4->index());
          arg_iter3++;
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), rel_ident5->index());
          field_iter1++;
          BuiltinApplication *builtin_app6 = dynamic_cast<BuiltinApplication *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != builtin_app6);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app6->args().size());
          auto arg_iter6 = builtin_app6->args().begin();
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident7->index());
          arg_iter6++;
          VariableExpression *var_expr8 = dynamic_cast<VariableExpression *>(arg_iter6->get());
          CPPUNIT_ASSERT(nullptr != var_expr8);
          RelativeIdentifier *rel_ident8 = dynamic_cast<RelativeIdentifier *>(var_expr8->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident8);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident8->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident8->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_type_arguments()
      {
        istringstream iss("\
template\n\
type T(t, u, v) = (t, u, v)\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), type_synonym_fun->args().size());
          auto type_arg_iter = type_synonym_fun->args().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*type_arg_iter)->index());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*type_arg_iter)->index());
          type_arg_iter++;
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*type_arg_iter)->index());
          CPPUNIT_ASSERT(nullptr != type_synonym_fun->body());
          NonUniqueTupleType *tuple_type1 = dynamic_cast<NonUniqueTupleType *>(type_synonym_fun->body());
          CPPUNIT_ASSERT(nullptr != tuple_type1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tuple_type1->field_types().size());
          auto field_type_iter1 = tuple_type1->field_types().begin();
          TypeParameterExpression *type_param_expr2 = dynamic_cast<TypeParameterExpression *>(field_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr2->index());
          field_type_iter1++;
          TypeParameterExpression *type_param_expr3 = dynamic_cast<TypeParameterExpression *>(field_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_param_expr3->index());
          field_type_iter1++;
          TypeParameterExpression *type_param_expr4 = dynamic_cast<TypeParameterExpression *>(field_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), type_param_expr4->index());
          CPPUNIT_ASSERT(nullptr != type_fun_info->insts());
          CPPUNIT_ASSERT_EQUAL(true, type_fun_info->insts()->empty());
        }
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_fun_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_fun_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_type_parameters_and_variable_template()
      {
        istringstream iss("\
import stdlib\n\
\n\
template\n\
v: (Array(t), Array(u)) = (#[]: Array(t), #[]: Array(u))\n\
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
          CPPUNIT_ASSERT(nullptr != user_defined_var->type_expr());
          NonUniqueTupleType *tuple_type = dynamic_cast<NonUniqueTupleType *>(user_defined_var->type_expr());
          CPPUNIT_ASSERT(nullptr != tuple_type);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple_type->field_types().size());
          auto field_type_iter = tuple_type->field_types().begin();
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
          TypeApplication *type_app3 = dynamic_cast<TypeApplication *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_app3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_app3->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app3->args().size());
          auto type_arg_iter3 = type_app3->args().begin();
          TypeParameterExpression *type_param_expr4 = dynamic_cast<TypeParameterExpression *>(type_arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_param_expr4->index());
          CPPUNIT_ASSERT(nullptr != user_defined_var->value());
          TupleValue *tuple_value = dynamic_cast<TupleValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != tuple_value);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple_value->fields().size());
          auto field_iter = tuple_value->fields().begin();
          TypedValue *typed_value1 = dynamic_cast<TypedValue *>(field_iter->get());
          CPPUNIT_ASSERT(nullptr != typed_value1);
          ArrayValue *array_value2 = dynamic_cast<ArrayValue *>(typed_value1->value());
          CPPUNIT_ASSERT(nullptr != array_value2);
          CPPUNIT_ASSERT_EQUAL(true, array_value2->elems().empty());
          TypeApplication *type_app5 = dynamic_cast<TypeApplication *>(typed_value1->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app5);
          RelativeIdentifier *type_rel_ident5 = dynamic_cast<RelativeIdentifier *>(type_app5->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident5);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident5->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident5->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app5->args().size());
          auto type_arg_iter5 = type_app5->args().begin();
          TypeParameterExpression *type_param_expr6 = dynamic_cast<TypeParameterExpression *>(type_arg_iter5->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr6);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr6->index());
          field_iter++;
          TypedValue *typed_value3 = dynamic_cast<TypedValue *>(field_iter->get());
          CPPUNIT_ASSERT(nullptr != typed_value3);
          ArrayValue *array_value4 = dynamic_cast<ArrayValue *>(typed_value3->value());
          CPPUNIT_ASSERT(nullptr != array_value4);
          CPPUNIT_ASSERT_EQUAL(true, array_value4->elems().empty());
          TypeApplication *type_app7 = dynamic_cast<TypeApplication *>(typed_value3->type_expr());
          CPPUNIT_ASSERT(nullptr != type_app7);
          RelativeIdentifier *type_rel_ident7 = dynamic_cast<RelativeIdentifier *>(type_app7->fun_ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident7);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident7->has_key_ident());
          CPPUNIT_ASSERT(stdlib_array_abs_ident.key_ident() == type_rel_ident7->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_app7->args().size());
          auto type_arg_iter7 = type_app7->args().begin();
          TypeParameterExpression *type_param_expr8 = dynamic_cast<TypeParameterExpression *>(type_arg_iter7->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr8);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_param_expr8->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_for_type_parameters_and_function_template()
      {
        istringstream iss("\
import stdlib\n\
\n\
template\n\
f(x: Array(t), y, z): (t, u) = (#anth(x, y): t, z: u)\n\
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->inst_type_params().empty());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), user_defined_fun->args().size());
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
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->result_type_expr());
          NonUniqueTupleType *tuple_type4 = dynamic_cast<NonUniqueTupleType *>(user_defined_fun->result_type_expr());
          CPPUNIT_ASSERT(nullptr != tuple_type4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple_type4->field_types().size());
          auto field_type_iter4 = tuple_type4->field_types().begin();
          TypeParameterExpression *type_param_expr5 = dynamic_cast<TypeParameterExpression *>(field_type_iter4->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr5->index());
          field_type_iter4++;
          TypeParameterExpression *type_param_expr6 = dynamic_cast<TypeParameterExpression *>(field_type_iter4->get());
          CPPUNIT_ASSERT(nullptr != type_param_expr6);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_param_expr6->index());
          CPPUNIT_ASSERT(nullptr != user_defined_fun->body());
          NonUniqueTuple *tuple1 = dynamic_cast<NonUniqueTuple *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != tuple1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple1->fields().size());
          auto field_iter1 = tuple1->fields().begin();
          TypedExpression *typed_expr2 = dynamic_cast<TypedExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != typed_expr2);
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(typed_expr2->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident4->index());
          arg_iter3++;
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident5->index());
          TypeParameterExpression *type_param_expr7 = dynamic_cast<TypeParameterExpression *>(typed_expr2->type_expr());
          CPPUNIT_ASSERT(nullptr != type_param_expr7);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), type_param_expr7->index());
          field_iter1++;
          TypedExpression *typed_expr6 = dynamic_cast<TypedExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != typed_expr6);
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(typed_expr6->expr());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident7->index());
          TypeParameterExpression *type_param_expr8 = dynamic_cast<TypeParameterExpression *>(typed_expr6->type_expr());
          CPPUNIT_ASSERT(nullptr != type_param_expr8);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), type_param_expr8->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_eager_annotation()
      {
        istringstream iss("\
@eager\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_lazy_annotation()
      {
        istringstream iss("\
@lazy\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_memoized_annotation()
      {
        istringstream iss("\
@memoized\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_unmemoized_annotation()
      {
        istringstream iss("\
@unmemoized\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_eager_annotation_and_memoized_annotation()
      {
        istringstream iss("\
@eager\n\
@memoized\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_eager_annotation_and_unmemoized_annotation()
      {
        istringstream iss("\
@eager\n\
@unmemoized\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_lazy_annotation_and_memoized_annotation()
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_lazy_annotation_and_unmemoized_annotation()
      {
        istringstream iss("\
@lazy\n\
@unmemoized\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_onlyeager_annotation()
      {
        istringstream iss("\
@onlyeager\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_onlylazy_annotation()
      {
        istringstream iss("\
@onlylazy\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_checks_onlymemoized_annotation()
      {
        istringstream iss("\
@onlymemoized\n\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_let_expression()
      {
        istringstream iss("\
a = 1\n\
\n\
f(x) =\n\
  let y = a\n\
      z = x\n\
  in  #iadd(y, z)\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Let *let1 = dynamic_cast<Let *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != let1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), let1->binds().size());
          auto bind_iter1 = let1->binds().begin();
          VariableBinding *var_bind2 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_bind2->index());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(var_bind2->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          bind_iter1++;
          VariableBinding *var_bind3 = dynamic_cast<VariableBinding *>(bind_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_bind3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), var_bind3->index());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(var_bind3->expr());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident3->index());
          BuiltinApplication *builtin_app4 = dynamic_cast<BuiltinApplication *>(let1->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app4->args().size());
          auto arg_iter4 = builtin_app4->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident5->index());
          arg_iter4++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident6->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_match_expression()
      {
        istringstream iss("\
a = 1\n\
\n\
f(x, y) =\n\
  x match {\n\
    1 -> a\n\
    _ -> y\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          LiteralPattern *literal_pattern2 =  dynamic_cast<LiteralPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != literal_pattern2);
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident3->key_ident());
          case_iter1++;
          WildcardPattern *wildcard_pattern4 =  dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern4);
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident5->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }

      void ResolverTests::test_resolver_resolves_identifiers_from_throw_expression()
      {
        istringstream iss("\
datatype Exception = Exception\n\
\n\
f() = throw Exception\n\
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
        AbsoluteIdentifier exception_abs_ident(list<string> { "Exception" });
        CPPUNIT_ASSERT_EQUAL(true, exception_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(exception_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(exception_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(exception_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Throw *throw1 = dynamic_cast<Throw *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != throw1);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(throw1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(exception_abs_ident.key_ident() == rel_ident2->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(exception_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(exception_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_typed_expression()
      {
        istringstream iss("\
import stdlib\n\
\n\
a = 1\n\
\n\
f() = a: Int64\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          TypedExpression *typed_expr1 = dynamic_cast<TypedExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != typed_expr1);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(typed_expr1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(typed_expr1->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_field_expression()
      {
        istringstream iss("\
a = (1, 2)\n\
\n\
f() = a.0\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          VariableInfo *var_info = tree.var_info(a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          TupleValue *tuple_value = dynamic_cast<TupleValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != tuple_value);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tuple_value->fields().size());
          auto field_iter = tuple_value->fields().begin();
          VariableLiteralValue *var_literal_value1 = dynamic_cast<VariableLiteralValue *>(field_iter->get());
          CPPUNIT_ASSERT(nullptr != var_literal_value1);
          field_iter++;
          VariableLiteralValue *var_literal_value2 = dynamic_cast<VariableLiteralValue *>(field_iter->get());
          CPPUNIT_ASSERT(nullptr != var_literal_value2);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Field *field1 = dynamic_cast<Field *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != field1);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_unique_field_expression()
      {
        istringstream iss("\
import stdlib\n\
\n\
f(x: unique (Int64, Int64)) = x unique .0\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          UniqueTupleType *unique_tuple_type1 = dynamic_cast<UniqueTupleType *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != unique_tuple_type1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), unique_tuple_type1->field_types().size());
          auto field_type_iter1 = unique_tuple_type1->field_types().begin();
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          field_type_iter1++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(field_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueField *unique_field1 = dynamic_cast<UniqueField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_field1);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(unique_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }

      void ResolverTests::test_resolver_resolves_identifiers_from_set_unique_field_expression()
      {
        istringstream iss("\
import stdlib\n\
\n\
a = 1\n\
\n\
f(x: unique (Int64, Int64)) = x unique .0 <- a\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          UniqueTupleType *unique_tuple_type1 = dynamic_cast<UniqueTupleType *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != unique_tuple_type1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), unique_tuple_type1->field_types().size());
          auto field_type_iter1 = unique_tuple_type1->field_types().begin();
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          field_type_iter1++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(field_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          SetUniqueField *set_unique_field1 = dynamic_cast<SetUniqueField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != set_unique_field1);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(set_unique_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(set_unique_field1->value_expr());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_named_field_expression()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C {\n\
    field1: Int64,\n\
    field2: Int64\n\
  }\n\
\n\
a = C { field1 = 1, field2 = 2 }\n\
\n\
f() = (a).field1\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          NamedFieldConstructor *constr = dynamic_cast<NamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(a_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          NamedFieldConstructorValue *constr_value = dynamic_cast<NamedFieldConstructorValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != constr_value);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr_value->fields().size());
          auto field_iter = constr_value->fields().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*field_iter)->index());
          VariableLiteralValue *var_literal_value1 = dynamic_cast<VariableLiteralValue *>((*field_iter)->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value1);
          field_iter++;
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*field_iter)->index());
          VariableLiteralValue *var_literal_value2 = dynamic_cast<VariableLiteralValue *>((*field_iter)->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value2);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NamedField *named_field1 = dynamic_cast<NamedField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != named_field1);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(named_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_unique_named_field_expression()
      {
        istringstream iss("\
import stdlib\n\
\n\
unique datatype T = C {\n\
    field1: Int64,\n\
    field2: Int64\n\
  }\n\
\n\
f(x: T) = x unique .field1\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          NamedFieldConstructor *constr = dynamic_cast<NamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueNamedField *unique_named_field1 = dynamic_cast<UniqueNamedField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_named_field1);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(unique_named_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }

      void ResolverTests::test_resolver_resolves_identifiers_from_set_unique_named_field_expression()
      {
        istringstream iss("\
import stdlib\n\
\n\
unique datatype T = C {\n\
    field1: Int64,\n\
    field2: Int64\n\
  }\n\
\n\
a = 1\n\
\n\
f(x: T) = x unique .field1 <- a\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          NamedFieldConstructor *constr = dynamic_cast<NamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          SetUniqueNamedField *set_unique_named_field1 = dynamic_cast<SetUniqueNamedField *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != set_unique_named_field1);
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(set_unique_named_field1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(set_unique_named_field1->value_expr());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_application()
      {
        istringstream iss("\
a = 1\n\
\n\
b = 2\n\
\n\
f(x, y, z) = #iadd(#iadd(x, y), z)\n\
\n\
g(x) = f(a, x, b)\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier b_abs_ident(list<string> { "b" });
        CPPUNIT_ASSERT_EQUAL(true, b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier g_abs_ident(list<string> { "g" });
        CPPUNIT_ASSERT_EQUAL(true, g_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
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
          arg_iter1++;
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident5->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(f_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->args().size());
          auto arg_iter1 = app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
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
          CPPUNIT_ASSERT(b_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_primitive_application()
      {
        istringstream iss("\
a = 1\n\
\n\
b = 2\n\
\n\
primitive f(x, y, z) = #iadd(#iadd(x, y), z)\n\
\n\
g(x) = f primitive (x, a, b)\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier b_abs_ident(list<string> { "b" });
        CPPUNIT_ASSERT_EQUAL(true, b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier g_abs_ident(list<string> { "g" });
        CPPUNIT_ASSERT_EQUAL(true, g_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          BuiltinApplication *builtin_app1 = dynamic_cast<BuiltinApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != builtin_app1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app1->args().size());
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
          arg_iter1++;
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), rel_ident5->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          NonUniqueApplication *app1 = dynamic_cast<NonUniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != app1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(f_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), app1->args().size());
          auto arg_iter1 = app1->args().begin();
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
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident3->key_ident());
          arg_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(b_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(g_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_unique_application()
      {
        istringstream iss("\
import stdlib\n\
\n\
a = 1\n\
\n\
b = 2\n\
\n\
f(g: (Int64, Int64, Int64) unique -> Int64, x) = g unique (a, x, b)\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier b_abs_ident(list<string> { "b" });
        CPPUNIT_ASSERT_EQUAL(true, b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
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
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          UniqueFunctionType *unique_fun_type1 = dynamic_cast<UniqueFunctionType *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != unique_fun_type1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), unique_fun_type1->arg_types().size());
          auto arg_type_iter1 = unique_fun_type1->arg_types().begin();
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(arg_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          arg_type_iter1++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(arg_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
          arg_type_iter1++;
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(arg_type_iter1->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident4->key_ident());
          TypeVariableExpression *type_var_expr5 = dynamic_cast<TypeVariableExpression *>(unique_fun_type1->result_type());
          CPPUNIT_ASSERT(nullptr != type_var_expr5);
          RelativeIdentifier *type_rel_ident5 = dynamic_cast<RelativeIdentifier *>(type_var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident5);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident5->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident5->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          arg_iter++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueApplication *unique_app1 = dynamic_cast<UniqueApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_app1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(unique_app1->fun());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), unique_app1->args().size());
          auto arg_iter1 = unique_app1->args().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident3->index());
          arg_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(b_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_builtin_application()
      {
        istringstream iss("\
a = 1\n\
\n\
f(x) = #iadd(a, x)\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          arg_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(arg_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident3->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_literal()
      {
        istringstream iss("\
f() = 1\n\
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
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Literal *literal1 = dynamic_cast<Literal *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != literal1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifier_from_variable_expression()
      {
        istringstream iss("\
a = 1\n\
\n\
f() = a\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          FunctionVariable *fun_var = dynamic_cast<FunctionVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != fun_var);
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(true, user_defined_fun->args().empty());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_list()
      {
        istringstream iss("\
a = 1\n\
\n\
b = 2\n\
\n\
f(x) = [a, x, b]\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier b_abs_ident(list<string> { "b" });
        CPPUNIT_ASSERT_EQUAL(true, b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          List *list1 = dynamic_cast<List *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != list1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), list1->elems().size());
          auto elem_iter1 = list1->elems().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          elem_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident3->index());
          elem_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(b_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_array()
      {
        istringstream iss("\
a = 1\n\
\n\
b = 2\n\
\n\
f(x) = #[x, a, b]\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier b_abs_ident(list<string> { "b" });
        CPPUNIT_ASSERT_EQUAL(true, b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          NonUniqueArray *array1 = dynamic_cast<NonUniqueArray *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != array1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), array1->elems().size());
          auto elem_iter1 = array1->elems().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          elem_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident3->key_ident());
          elem_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(b_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_unique_array()
      {
        istringstream iss("\
a = 1\n\
\n\
b = 2\n\
\n\
f(x) = unique #[a, b, x]\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier b_abs_ident(list<string> { "b" });
        CPPUNIT_ASSERT_EQUAL(true, b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          UniqueArray *unique_array1 = dynamic_cast<UniqueArray *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_array1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), unique_array1->elems().size());
          auto elem_iter1 = unique_array1->elems().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          elem_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(b_abs_ident.key_ident() == rel_ident3->key_ident());
          elem_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident4->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_tuple()
      {
        istringstream iss("\
import stdlib\n\
\n\
a = 1\n\
\n\
b = 2\n\
\n\
f(x: Int64) = (a, x, b)\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier b_abs_ident(list<string> { "b" });
        CPPUNIT_ASSERT_EQUAL(true, b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          NonUniqueTuple *tuple1 = dynamic_cast<NonUniqueTuple *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != tuple1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tuple1->fields().size());
          auto field_iter1 = tuple1->fields().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          field_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident3->index());
          field_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(b_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_unique_tuple()
      {
        istringstream iss("\
import stdlib\n\
\n\
a = 1\n\
\n\
b = 2\n\
\n\
f(x: Int64) = unique (x, a, b)\n\
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
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier b_abs_ident(list<string> { "b" });
        CPPUNIT_ASSERT_EQUAL(true, b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          UniqueTuple *unique_tuple1 = dynamic_cast<UniqueTuple *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != unique_tuple1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), unique_tuple1->fields().size());
          auto field_iter1 = unique_tuple1->fields().begin();
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident2->index());
          field_iter1++;
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident3->key_ident());
          field_iter1++;
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(b_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_named_field_constructor_application()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C {\n\
    field1: Int64,\n\
    field2: Int64,\n\
    field3: Int64\n\
  }\n\
\n\
a = 1\n\
\n\
b = 2\n\
\n\
f(x) = C { field3 = a, field2 = x, field1 = b }\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier a_abs_ident(list<string> { "a" });
        CPPUNIT_ASSERT_EQUAL(true, a_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier b_abs_ident(list<string> { "b" });
        CPPUNIT_ASSERT_EQUAL(true, b_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          NamedFieldConstructor *constr = dynamic_cast<NamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(b_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          NamedFieldConstructorApplication *named_field_constr_app1 = dynamic_cast<NamedFieldConstructorApplication *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != named_field_constr_app1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(named_field_constr_app1->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), named_field_constr_app1->fields().size());
          auto field_iter1 = named_field_constr_app1->fields().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*field_iter1)->index());
          VariableExpression *var_expr2 = dynamic_cast<VariableExpression *>((*field_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(a_abs_ident.key_ident() == rel_ident2->key_ident());
          field_iter1++;
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*field_iter1)->index());
          VariableExpression *var_expr3 = dynamic_cast<VariableExpression *>((*field_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident3->index());
          field_iter1++;
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*field_iter1)->index());
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>((*field_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(b_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(a_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(b_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_typed_pattern()
      {
        istringstream iss("\
import stdlib\n\
\n\
f(x) =\n\
  x match {\n\
    y: Int64 -> y\n\
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
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          TypedPattern *typed_pattern2 = dynamic_cast<TypedPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != typed_pattern2);
          VariablePattern *var_pattern3 = dynamic_cast<VariablePattern *>(typed_pattern2->pattern());
          CPPUNIT_ASSERT(nullptr != var_pattern3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern3->index());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(typed_pattern2->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident->key_ident());
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident4->index());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifier_from_variable_constructor_pattern()
      {
        istringstream iss("\
datatype T = C\n\
\n\
f(x) =\n\
  x match {\n\
    C -> 1\n\
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
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          VariableConstructorPattern *var_constr_pattern2 = dynamic_cast<VariableConstructorPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern2->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident2->key_ident());
          Literal *literal3 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_function_constructor_pattern()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C | D\n\
\n\
datatype U = E(T, Int64, T)\n\
\n\
f(x) =\n\
  x match {\n\
    E(C, y, D) -> y\n\
    _ -> 1\n\
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
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));        
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier u_abs_ident(list<string> { "U" });
        CPPUNIT_ASSERT_EQUAL(true, u_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier e_abs_ident(list<string> { "E" });
        CPPUNIT_ASSERT_EQUAL(true, e_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(u_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(e_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(u_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          UnnamedFieldConstructorPattern *fun_constr_pattern2 = dynamic_cast<UnnamedFieldConstructorPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != fun_constr_pattern2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(fun_constr_pattern2->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(e_abs_ident.key_ident() == rel_ident2->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_constr_pattern2->field_patterns().size());
          auto field_pattern_iter2 = fun_constr_pattern2->field_patterns().begin();
          VariableConstructorPattern *var_constr_pattern3 = dynamic_cast<VariableConstructorPattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern3->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident3->key_ident());
          field_pattern_iter2++;
          VariablePattern *var_pattern4 = dynamic_cast<VariablePattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_pattern4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern4->index());
          field_pattern_iter2++;
          VariableConstructorPattern *var_constr_pattern5 = dynamic_cast<VariableConstructorPattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern5->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident5->key_ident());
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          case_iter1++;
          WildcardPattern *wildcard_pattern7 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern7);
          Literal *literal8 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal8);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(e_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(u_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_named_field_constructor_pattern()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C | D\n\
\n\
datatype U = E {\n\
    field1: T,\n\
    field2: Int64,\n\
    field3: T\n\
  }\n\
\n\
f(x) =\n\
  x match {\n\
    E { field3 = C, field2 = y, field1 = D } -> y\n\
    _ -> 1\n\
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
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));        
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier u_abs_ident(list<string> { "U" });
        CPPUNIT_ASSERT_EQUAL(true, u_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier e_abs_ident(list<string> { "E" });
        CPPUNIT_ASSERT_EQUAL(true, e_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(u_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(e_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          NamedFieldConstructor *constr = dynamic_cast<NamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(u_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          NamedFieldConstructorPattern *named_field_constr_pattern2 = dynamic_cast<NamedFieldConstructorPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != named_field_constr_pattern2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(named_field_constr_pattern2->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(e_abs_ident.key_ident() == rel_ident2->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), named_field_constr_pattern2->field_patterns().size());
          auto field_pattern_iter2 = named_field_constr_pattern2->field_patterns().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*field_pattern_iter2)->index());
          VariableConstructorPattern *var_constr_pattern3 = dynamic_cast<VariableConstructorPattern *>((*field_pattern_iter2)->pattern());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern3->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident3->key_ident());
          field_pattern_iter2++;
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*field_pattern_iter2)->index());
          VariablePattern *var_pattern4 = dynamic_cast<VariablePattern *>((*field_pattern_iter2)->pattern());
          CPPUNIT_ASSERT(nullptr != var_pattern4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern4->index());
          field_pattern_iter2++;
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*field_pattern_iter2)->index());
          VariableConstructorPattern *var_constr_pattern5 = dynamic_cast<VariableConstructorPattern *>((*field_pattern_iter2)->pattern());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern5->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident5->key_ident());
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          case_iter1++;
          WildcardPattern *wildcard_pattern7 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern7);
          Literal *literal8 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal8);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(e_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(u_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_list_pattern()
      {
        istringstream iss("\
datatype T = C | D\n\
\n\
f(x) =\n\
  x match {\n\
    [C, y, D] -> 1\n\
    _ -> 2\n\
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
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          ListPattern *list_pattern2 = dynamic_cast<ListPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != list_pattern2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), list_pattern2->elem_patterns().size());
          auto elem_pattern_iter2 = list_pattern2->elem_patterns().begin();
          VariableConstructorPattern *var_constr_pattern3 = dynamic_cast<VariableConstructorPattern *>(elem_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern3->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident3->key_ident());
          elem_pattern_iter2++;
          VariablePattern *var_pattern4 = dynamic_cast<VariablePattern *>(elem_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_pattern4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern4->index());
          elem_pattern_iter2++;
          VariableConstructorPattern *var_constr_pattern5 = dynamic_cast<VariableConstructorPattern *>(elem_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern5->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident5->key_ident());
          Literal *literal6 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal6);
          case_iter1++;
          WildcardPattern *wildcard_pattern7 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern7);
          Literal *literal8 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal8);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_array_pattern()
      {
        istringstream iss("\
datatype T = C | D\n\
\n\
f(x) =\n\
  x match {\n\
   #[y, C, D] -> 1\n\
    _ -> 2\n\
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
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          NonUniqueArrayPattern *array_pattern2 = dynamic_cast<NonUniqueArrayPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != array_pattern2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), array_pattern2->elem_patterns().size());
          auto elem_pattern_iter2 = array_pattern2->elem_patterns().begin();
          VariablePattern *var_pattern3 = dynamic_cast<VariablePattern *>(elem_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_pattern3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern3->index());
          elem_pattern_iter2++;
          VariableConstructorPattern *var_constr_pattern4 = dynamic_cast<VariableConstructorPattern *>(elem_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern4->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident4->key_ident());
          elem_pattern_iter2++;
          VariableConstructorPattern *var_constr_pattern5 = dynamic_cast<VariableConstructorPattern *>(elem_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern5->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident5->key_ident());
          Literal *literal6 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal6);
          case_iter1++;
          WildcardPattern *wildcard_pattern7 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern7);
          Literal *literal8 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal8);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_unique_array_pattern()
      {
        istringstream iss("\
datatype T = C | D\n\
\n\
f(x) =\n\
  x match {\n\
   unique #[C, D, y] -> 1\n\
    _ -> 2\n\
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
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          UniqueArrayPattern *unique_array_pattern2 = dynamic_cast<UniqueArrayPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != unique_array_pattern2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), unique_array_pattern2->elem_patterns().size());
          auto elem_pattern_iter2 = unique_array_pattern2->elem_patterns().begin();
          VariableConstructorPattern *var_constr_pattern3 = dynamic_cast<VariableConstructorPattern *>(elem_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern3->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident3->key_ident());
          elem_pattern_iter2++;
          VariableConstructorPattern *var_constr_pattern4 = dynamic_cast<VariableConstructorPattern *>(elem_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern4->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident4->key_ident());
          elem_pattern_iter2++;
          VariablePattern *var_pattern5 = dynamic_cast<VariablePattern *>(elem_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_pattern5);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern5->index());
          Literal *literal6 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal6);
          case_iter1++;
          WildcardPattern *wildcard_pattern7 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern7);
          Literal *literal8 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal8);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_tuple_pattern()
      {
        istringstream iss("\
datatype T = C | D\n\
\n\
f(x) =\n\
  x match {\n\
    (C, y, D) -> y\n\
    _ -> 1\n\
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
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          NonUniqueTuplePattern *tuple_pattern2 = dynamic_cast<NonUniqueTuplePattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != tuple_pattern2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tuple_pattern2->field_patterns().size());
          auto field_pattern_iter2 = tuple_pattern2->field_patterns().begin();
          VariableConstructorPattern *var_constr_pattern3 = dynamic_cast<VariableConstructorPattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern3->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident3->key_ident());
          field_pattern_iter2++;
          VariablePattern *var_pattern4 = dynamic_cast<VariablePattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_pattern4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern4->index());
          field_pattern_iter2++;
          VariableConstructorPattern *var_constr_pattern5 = dynamic_cast<VariableConstructorPattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern5->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident5->key_ident());
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          case_iter1++;
          WildcardPattern *wildcard_pattern7 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern7);
          Literal *literal8 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal8);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_unique_tuple_pattern()
      {
        istringstream iss("\
datatype T = C | D\n\
\n\
f(x) =\n\
  x match {\n\
    unique (y, C, D) -> y\n\
    _ -> 1\n\
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
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          UniqueTuplePattern *unique_tuple_pattern2 = dynamic_cast<UniqueTuplePattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != unique_tuple_pattern2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), unique_tuple_pattern2->field_patterns().size());
          auto field_pattern_iter2 = unique_tuple_pattern2->field_patterns().begin();
          VariablePattern *var_pattern3 = dynamic_cast<VariablePattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_pattern3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern3->index());
          field_pattern_iter2++;
          VariableConstructorPattern *var_constr_pattern4 = dynamic_cast<VariableConstructorPattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern4->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident4->key_ident());
          field_pattern_iter2++;
          VariableConstructorPattern *var_constr_pattern5 = dynamic_cast<VariableConstructorPattern *>(field_pattern_iter2->get());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern5->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident5->key_ident());
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          case_iter1++;
          WildcardPattern *wildcard_pattern7 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern7);
          Literal *literal8 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal8);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_literal_pattern()
      {
        istringstream iss("\
f(x) =\n\
  x match {\n\
    1 -> 2\n\
    _ -> 3\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          LiteralPattern *literal_pattern2 = dynamic_cast<LiteralPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != literal_pattern2);
          Literal *literal3 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          case_iter1++;
          WildcardPattern *wildcard_pattern4 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern4);
          Literal *literal5 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifier_from_variable_pattern()
      {
        istringstream iss("\
f(x) =\n\
  x match {\n\
    y -> #iadd(y, 1)\n\
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
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          VariablePattern *var_pattern2 = dynamic_cast<VariablePattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != var_pattern2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), var_pattern2->index());
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          VariableExpression *var_expr4 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident4->index());
          arg_iter3++;
          Literal *literal5 = dynamic_cast<Literal *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != literal5);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_as_pattern()
      {
        istringstream iss("\
datatype T = C\n\
\n\
f(x) =\n\
  x match {\n\
    y @ C -> 1\n\
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
        AbsoluteIdentifier t_abs_ident(list<string> { "T" });
        CPPUNIT_ASSERT_EQUAL(true, t_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
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
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          AsPattern *as_pattern2 = dynamic_cast<AsPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != as_pattern2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), as_pattern2->index());
          VariableConstructorPattern *var_constr_pattern3 = dynamic_cast<VariableConstructorPattern *>(as_pattern2->pattern());
          CPPUNIT_ASSERT(nullptr != var_constr_pattern3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_constr_pattern3->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident3->key_ident());
          Literal *literal4 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal4);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_wildcard_pattern()
      {
        istringstream iss("\
import stdlib\n\
\n\
f(x: Int64) =\n\
  x match {\n\
    _ -> 1\n\
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
          UserDefinedFunction *user_defined_fun = dynamic_cast<UserDefinedFunction *>(fun_var->fun().get());
          CPPUNIT_ASSERT(nullptr != user_defined_fun);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), user_defined_fun->args().size());
          auto arg_iter = user_defined_fun->args().begin();
          CPPUNIT_ASSERT(nullptr != (*arg_iter)->type_expr());
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*arg_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter)->index());
          CPPUNIT_ASSERT(nullptr == user_defined_fun->result_type_expr());
          Match *match1 = dynamic_cast<Match *>(user_defined_fun->body());
          CPPUNIT_ASSERT(nullptr != match1);
          VariableExpression *var_expr1 = dynamic_cast<VariableExpression *>(match1->expr());
          CPPUNIT_ASSERT(nullptr != var_expr1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident1->index());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), match1->cases().size());
          auto case_iter1 = match1->cases().begin();
          WildcardPattern *wildcard_pattern2 = dynamic_cast<WildcardPattern *>((*case_iter1)->pattern());
          CPPUNIT_ASSERT(nullptr != wildcard_pattern2);
          Literal *literal3 = dynamic_cast<Literal *>((*case_iter1)->expr());
          CPPUNIT_ASSERT(nullptr != literal3);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_typed_value()
      {
        istringstream iss("\
datatype T = C\n\
\n\
v = C: T\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
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
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          TypedValue *typed_value1 = dynamic_cast<TypedValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != typed_value1);
          VariableConstructorValue *var_constr_value2 = dynamic_cast<VariableConstructorValue *>(typed_value1->value());
          CPPUNIT_ASSERT(nullptr != var_constr_value2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_constr_value2->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident2->key_ident());
          TypeVariableExpression *type_var_expr = dynamic_cast<TypeVariableExpression *>(typed_value1->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr);
          RelativeIdentifier *type_rel_ident = dynamic_cast<RelativeIdentifier *>(type_var_expr->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_variable_literal_value()
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
          VariableLiteralValue *var_literal_value1 = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value1);
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_list_value()
      {
        istringstream iss("\
datatype T = C | D\n\
\n\
v = [C, C, D]\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          ListValue *list_value1 = dynamic_cast<ListValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != list_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), list_value1->elems().size());
          auto elem_iter1 = list_value1->elems().begin();
          VariableConstructorValue *var_constr_value2 = dynamic_cast<VariableConstructorValue *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_constr_value2->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident2->key_ident());
          elem_iter1++;
          VariableConstructorValue *var_constr_value3 = dynamic_cast<VariableConstructorValue *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_constr_value3->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident3->key_ident());
          elem_iter1++;
          VariableConstructorValue *var_constr_value4 = dynamic_cast<VariableConstructorValue *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_constr_value4->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_array_value()
      {
        istringstream iss("\
datatype T = C | D\n\
\n\
v = #[C, D, D]\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          ArrayValue *array_value1 = dynamic_cast<ArrayValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != array_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), array_value1->elems().size());
          auto elem_iter1 = array_value1->elems().begin();
          VariableConstructorValue *var_constr_value2 = dynamic_cast<VariableConstructorValue *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_constr_value2->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident2->key_ident());
          elem_iter1++;
          VariableConstructorValue *var_constr_value3 = dynamic_cast<VariableConstructorValue *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value3);
          RelativeIdentifier *rel_ident3 = dynamic_cast<RelativeIdentifier *>(var_constr_value3->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident3->key_ident());
          elem_iter1++;
          VariableConstructorValue *var_constr_value4 = dynamic_cast<VariableConstructorValue *>(elem_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_constr_value4->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_tuple_value()
      {
        istringstream iss("\
datatype T = C | D\n\
\n\
v = (C, 1, D)\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          TupleValue *tuple_value1 = dynamic_cast<TupleValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != tuple_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tuple_value1->fields().size());
          auto field_iter1 = tuple_value1->fields().begin();
          VariableConstructorValue *var_constr_value2 = dynamic_cast<VariableConstructorValue *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_constr_value2->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident2->key_ident());
          field_iter1++;
          VariableLiteralValue *var_literal_value3 = dynamic_cast<VariableLiteralValue *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_literal_value3);
          field_iter1++;
          VariableConstructorValue *var_constr_value4 = dynamic_cast<VariableConstructorValue *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_constr_value4->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifier_from_variable_constructor_value()
      {
        istringstream iss("\
datatype T = C\n\
\n\
v = (C)\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
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
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableConstructorValue *var_constr_value1 = dynamic_cast<VariableConstructorValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_constr_value1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(var_constr_value1->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_function_constructor_value()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C | D\n\
\n\
datatype U = E(T, Int64, T)\n\
\n\
v = E(C, 1, D)\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier u_abs_ident(list<string> { "U" });
        CPPUNIT_ASSERT_EQUAL(true, u_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier e_abs_ident(list<string> { "E" });
        CPPUNIT_ASSERT_EQUAL(true, e_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(u_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(e_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          UnnamedFieldConstructor *constr = dynamic_cast<UnnamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(u_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>(field_type_iter->get());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          UnnamedFieldConstructorValue *fun_constr_value1 = dynamic_cast<UnnamedFieldConstructorValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != fun_constr_value1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(fun_constr_value1->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(e_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), fun_constr_value1->fields().size());
          auto field_iter1 = fun_constr_value1->fields().begin();
          VariableConstructorValue *var_constr_value2 = dynamic_cast<VariableConstructorValue *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_constr_value2->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident2->key_ident());
          field_iter1++;
          VariableLiteralValue *var_literal_value3 = dynamic_cast<VariableLiteralValue *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_literal_value3);
          field_iter1++;
          VariableConstructorValue *var_constr_value4 = dynamic_cast<VariableConstructorValue *>(field_iter1->get());
          CPPUNIT_ASSERT(nullptr != var_constr_value4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_constr_value4->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(e_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(u_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_named_field_constructor_value()
      {
        istringstream iss("\
import stdlib\n\
\n\
datatype T = C | D\n\
\n\
datatype U = E {\n\
    field1: T,\n\
    field2: Int64,\n\
    field3: T\n\
  }\n\
\n\
v = E { field3 = C, field2 = 1, field1 = D }\n\
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
        AbsoluteIdentifier c_abs_ident(list<string> { "C" });
        CPPUNIT_ASSERT_EQUAL(true, c_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier d_abs_ident(list<string> { "D" });
        CPPUNIT_ASSERT_EQUAL(true, d_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier u_abs_ident(list<string> { "U" });
        CPPUNIT_ASSERT_EQUAL(true, u_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier e_abs_ident(list<string> { "E" });
        CPPUNIT_ASSERT_EQUAL(true, e_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, tree.has_module_key_ident(root_abs_ident.key_ident()));
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(t_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(c_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(d_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          VariableConstructor *constr = dynamic_cast<VariableConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          TypeVariableInfo *type_var_info = tree.type_var_info(u_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != type_var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, type_var_info->access_modifier());
          DatatypeVariable *datatype_var = dynamic_cast<DatatypeVariable *>(type_var_info->var().get());
          CPPUNIT_ASSERT(nullptr != datatype_var);
          NonUniqueDatatype *datatype = dynamic_cast<NonUniqueDatatype *>(datatype_var->datatype());
          CPPUNIT_ASSERT(nullptr != datatype);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), datatype->constrs().size());
        }
        {
          VariableInfo *var_info = tree.var_info(e_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          ConstructorVariable *constr_var = dynamic_cast<ConstructorVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != constr_var);
          NamedFieldConstructor *constr = dynamic_cast<NamedFieldConstructor *>(constr_var->constr().get());
          CPPUNIT_ASSERT(nullptr != constr);
          CPPUNIT_ASSERT_EQUAL(false, constr->has_datatype_fun());
          CPPUNIT_ASSERT(u_abs_ident.key_ident() == constr->datatype_key_ident());
          CPPUNIT_ASSERT(nullptr == constr->datatype_fun_inst());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), constr->field_types().size());
          auto field_type_iter = constr->field_types().begin();
          TypeVariableExpression *type_var_expr1 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr1);
          RelativeIdentifier *type_rel_ident1 = dynamic_cast<RelativeIdentifier *>(type_var_expr1->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident1->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());
          field_type_iter++;
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*field_type_iter)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(t_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(v_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          NamedFieldConstructorValue *named_field_constr_value1 = dynamic_cast<NamedFieldConstructorValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != named_field_constr_value1);
          RelativeIdentifier *rel_ident1 = dynamic_cast<RelativeIdentifier *>(named_field_constr_value1->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident1);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident1->has_key_ident());
          CPPUNIT_ASSERT(e_abs_ident.key_ident() == rel_ident1->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), named_field_constr_value1->fields().size());
          auto field_iter1 = named_field_constr_value1->fields().begin();
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), (*field_iter1)->index());
          VariableConstructorValue *var_constr_value2 = dynamic_cast<VariableConstructorValue *>((*field_iter1)->value());
          CPPUNIT_ASSERT(nullptr != var_constr_value2);
          RelativeIdentifier *rel_ident2 = dynamic_cast<RelativeIdentifier *>(var_constr_value2->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(c_abs_ident.key_ident() == rel_ident2->key_ident());
          field_iter1++;
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*field_iter1)->index());
          VariableLiteralValue *var_literal_value3 = dynamic_cast<VariableLiteralValue *>((*field_iter1)->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value3);
          field_iter1++;
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*field_iter1)->index());
          VariableConstructorValue *var_constr_value4 = dynamic_cast<VariableConstructorValue *>((*field_iter1)->value());
          CPPUNIT_ASSERT(nullptr != var_constr_value4);
          RelativeIdentifier *rel_ident4 = dynamic_cast<RelativeIdentifier *>(var_constr_value4->constr_ident());
          CPPUNIT_ASSERT(nullptr != rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(d_abs_ident.key_ident() == rel_ident4->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(c_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(d_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT(e_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[2]);
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[3]);
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_type_var_key_idents().size());
        CPPUNIT_ASSERT(t_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[0]);
        CPPUNIT_ASSERT(u_abs_ident.key_ident() == tree.uncompiled_type_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_lambda_value()
      {
        istringstream iss("\
v = 1\n\
\n\
f() = \\(x, y) -> #itoi8(#iadd(#iadd(x, y), v))\n\
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
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
          NonUniqueLambdaValue *lambda_value1 = dynamic_cast<NonUniqueLambdaValue *>(literal1->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lambda_value1->args().size());
          auto arg_iter1 = lambda_value1->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter1)->index());
          arg_iter1++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter1)->index());
          CPPUNIT_ASSERT(nullptr == lambda_value1->result_type_expr());
          BuiltinApplication *builtin_app2 = dynamic_cast<BuiltinApplication *>(lambda_value1->body());
          CPPUNIT_ASSERT(nullptr != builtin_app2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app2->args().size());
          auto arg_iter2 = builtin_app2->args().begin();
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          BuiltinApplication *builtin_app4 = dynamic_cast<BuiltinApplication *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != builtin_app4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app4->args().size());
          auto arg_iter4 = builtin_app4->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident5->index());
          arg_iter4++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          arg_iter3++;
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT(v_abs_ident.key_ident() == rel_ident7->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_lambda_value_with_types()
      {
        istringstream iss("\
import stdlib\n\
\n\
v = 1\n\
\n\
f() = \\(x, y: Int64): Int8 -> #itoi8(#iadd(#iadd(x, y), v))\n\
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
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
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
          NonUniqueLambdaValue *lambda_value1 = dynamic_cast<NonUniqueLambdaValue *>(literal1->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lambda_value1->args().size());
          auto arg_iter1 = lambda_value1->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter1)->index());
          arg_iter1++;
          CPPUNIT_ASSERT(nullptr != (*arg_iter1)->type_expr());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*arg_iter1)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter1)->index());
          CPPUNIT_ASSERT(nullptr != lambda_value1->result_type_expr());
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(lambda_value1->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident4->key_ident());
          BuiltinApplication *builtin_app2 = dynamic_cast<BuiltinApplication *>(lambda_value1->body());
          CPPUNIT_ASSERT(nullptr != builtin_app2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app2->args().size());
          auto arg_iter2 = builtin_app2->args().begin();
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          BuiltinApplication *builtin_app4 = dynamic_cast<BuiltinApplication *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != builtin_app4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app4->args().size());
          auto arg_iter4 = builtin_app4->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident5->index());
          arg_iter4++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          arg_iter3++;
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT(v_abs_ident.key_ident() == rel_ident7->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_unique_lambda_value()
      {
        istringstream iss("\
v = 1\n\
\n\
f() = unique \\(x, y) -> #itoi8(#iadd(#iadd(x, y), v))\n\
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
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
          UniqueLambdaValue *unique_lambda_value1 = dynamic_cast<UniqueLambdaValue *>(literal1->literal_value());
          CPPUNIT_ASSERT(nullptr != unique_lambda_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), unique_lambda_value1->args().size());
          auto arg_iter1 = unique_lambda_value1->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter1)->index());
          arg_iter1++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter1)->index());
          CPPUNIT_ASSERT(nullptr == unique_lambda_value1->result_type_expr());
          BuiltinApplication *builtin_app2 = dynamic_cast<BuiltinApplication *>(unique_lambda_value1->body());
          CPPUNIT_ASSERT(nullptr != builtin_app2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app2->args().size());
          auto arg_iter2 = builtin_app2->args().begin();
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          BuiltinApplication *builtin_app4 = dynamic_cast<BuiltinApplication *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != builtin_app4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app4->args().size());
          auto arg_iter4 = builtin_app4->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident5->index());
          arg_iter4++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          arg_iter3++;
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT(v_abs_ident.key_ident() == rel_ident7->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_unique_lambda_value_with_types()
      {
        istringstream iss("\
import stdlib\n\
\n\
v = 1\n\
\n\
f() = unique \\(x: Int64, y): Int8 -> #itoi8(#iadd(#iadd(x, y), v))\n\
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
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier f_abs_ident(list<string> { "f" });
        CPPUNIT_ASSERT_EQUAL(true, f_abs_ident.set_key_ident(*(tree.ident_table())));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(f_abs_ident.key_ident());
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
          UniqueLambdaValue *unique_lambda_value1 = dynamic_cast<UniqueLambdaValue *>(literal1->literal_value());
          CPPUNIT_ASSERT(nullptr != unique_lambda_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), unique_lambda_value1->args().size());
          auto arg_iter1 = unique_lambda_value1->args().begin();
          CPPUNIT_ASSERT(nullptr != (*arg_iter1)->type_expr());
          TypeVariableExpression *type_var_expr2 = dynamic_cast<TypeVariableExpression *>((*arg_iter1)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr2);
          RelativeIdentifier *type_rel_ident2 = dynamic_cast<RelativeIdentifier *>(type_var_expr2->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident2);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident2->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident2->key_ident());          
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter1)->index());
          arg_iter1++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter1)->index());
          CPPUNIT_ASSERT(nullptr != unique_lambda_value1->result_type_expr());
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(unique_lambda_value1->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident4->key_ident());
          BuiltinApplication *builtin_app2 = dynamic_cast<BuiltinApplication *>(unique_lambda_value1->body());
          CPPUNIT_ASSERT(nullptr != builtin_app2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app2->args().size());
          auto arg_iter2 = builtin_app2->args().begin();
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          BuiltinApplication *builtin_app4 = dynamic_cast<BuiltinApplication *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != builtin_app4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app4->args().size());
          auto arg_iter4 = builtin_app4->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident5->index());
          arg_iter4++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          arg_iter3++;
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT(v_abs_ident.key_ident() == rel_ident7->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(f_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_lambda_value_for_value()
      {
        istringstream iss("\
v = 1\n\
\n\
w = \\(x, y) -> #itoi8(#iadd(#iadd(x, y), v))\n\
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
        AbsoluteIdentifier w_abs_ident(list<string> { "w" });
        CPPUNIT_ASSERT_EQUAL(true, w_abs_ident.set_key_ident(*(tree.ident_table())));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(w_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value1 = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value1);
          NonUniqueLambdaValue *lambda_value1 = dynamic_cast<NonUniqueLambdaValue *>(var_literal_value1->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lambda_value1->args().size());
          auto arg_iter1 = lambda_value1->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter1)->index());
          arg_iter1++;
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter1)->index());
          CPPUNIT_ASSERT(nullptr == lambda_value1->result_type_expr());
          BuiltinApplication *builtin_app2 = dynamic_cast<BuiltinApplication *>(lambda_value1->body());
          CPPUNIT_ASSERT(nullptr != builtin_app2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app2->args().size());
          auto arg_iter2 = builtin_app2->args().begin();
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          BuiltinApplication *builtin_app4 = dynamic_cast<BuiltinApplication *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != builtin_app4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app4->args().size());
          auto arg_iter4 = builtin_app4->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident5->index());
          arg_iter4++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          arg_iter3++;
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT(v_abs_ident.key_ident() == rel_ident7->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(w_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
      
      void ResolverTests::test_resolver_resolves_identifiers_from_lambda_value_with_types_for_value()
      {
        istringstream iss("\
import stdlib\n\
\n\
v = 1\n\
\n\
w = \\(x, y: Int64): Int8 -> #itoi8(#iadd(#iadd(x, y), v))\n\
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
        AbsoluteIdentifier stdlib_int8_abs_ident(list<string> { "stdlib", "Int8" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int8_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier stdlib_int64_abs_ident(list<string> { "stdlib", "Int64" });
        CPPUNIT_ASSERT_EQUAL(true, stdlib_int64_abs_ident.set_key_ident(*(tree.ident_table())));
        CPPUNIT_ASSERT_EQUAL(true, root_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier v_abs_ident(list<string> { "v" });
        CPPUNIT_ASSERT_EQUAL(true, v_abs_ident.set_key_ident(*(tree.ident_table())));
        AbsoluteIdentifier w_abs_ident(list<string> { "w" });
        CPPUNIT_ASSERT_EQUAL(true, w_abs_ident.set_key_ident(*(tree.ident_table())));
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
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        {
          VariableInfo *var_info = tree.var_info(w_abs_ident.key_ident());
          CPPUNIT_ASSERT(nullptr != var_info);
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->access_modifier());
          UserDefinedVariable *user_defined_var = dynamic_cast<UserDefinedVariable *>(var_info->var().get());
          CPPUNIT_ASSERT(nullptr != user_defined_var);
          CPPUNIT_ASSERT(nullptr == user_defined_var->type_expr());
          VariableLiteralValue *var_literal_value1 = dynamic_cast<VariableLiteralValue *>(user_defined_var->value());
          CPPUNIT_ASSERT(nullptr != var_literal_value1);
          NonUniqueLambdaValue *lambda_value1 = dynamic_cast<NonUniqueLambdaValue *>(var_literal_value1->literal_value());
          CPPUNIT_ASSERT(nullptr != lambda_value1);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), lambda_value1->args().size());
          auto arg_iter1 = lambda_value1->args().begin();
          CPPUNIT_ASSERT(nullptr == (*arg_iter1)->type_expr());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), (*arg_iter1)->index());
          arg_iter1++;
          CPPUNIT_ASSERT(nullptr != (*arg_iter1)->type_expr());
          TypeVariableExpression *type_var_expr3 = dynamic_cast<TypeVariableExpression *>((*arg_iter1)->type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr3);
          RelativeIdentifier *type_rel_ident3 = dynamic_cast<RelativeIdentifier *>(type_var_expr3->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident3);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident3->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int64_abs_ident.key_ident() == type_rel_ident3->key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), (*arg_iter1)->index());
          CPPUNIT_ASSERT(nullptr != lambda_value1->result_type_expr());
          TypeVariableExpression *type_var_expr4 = dynamic_cast<TypeVariableExpression *>(lambda_value1->result_type_expr());
          CPPUNIT_ASSERT(nullptr != type_var_expr4);
          RelativeIdentifier *type_rel_ident4 = dynamic_cast<RelativeIdentifier *>(type_var_expr4->ident());
          CPPUNIT_ASSERT(nullptr != type_rel_ident4);
          CPPUNIT_ASSERT_EQUAL(true, type_rel_ident4->has_key_ident());
          CPPUNIT_ASSERT(stdlib_int8_abs_ident.key_ident() == type_rel_ident4->key_ident());
          BuiltinApplication *builtin_app2 = dynamic_cast<BuiltinApplication *>(lambda_value1->body());
          CPPUNIT_ASSERT(nullptr != builtin_app2);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), builtin_app2->args().size());
          auto arg_iter2 = builtin_app2->args().begin();
          BuiltinApplication *builtin_app3 = dynamic_cast<BuiltinApplication *>(arg_iter2->get());
          CPPUNIT_ASSERT(nullptr != builtin_app3);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app3->args().size());
          auto arg_iter3 = builtin_app3->args().begin();
          BuiltinApplication *builtin_app4 = dynamic_cast<BuiltinApplication *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != builtin_app4);
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), builtin_app4->args().size());
          auto arg_iter4 = builtin_app4->args().begin();
          VariableExpression *var_expr5 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr5);
          RelativeIdentifier *rel_ident5 = dynamic_cast<RelativeIdentifier *>(var_expr5->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident5);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident5->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), rel_ident5->index());
          arg_iter4++;
          VariableExpression *var_expr6 = dynamic_cast<VariableExpression *>(arg_iter4->get());
          CPPUNIT_ASSERT(nullptr != var_expr6);
          RelativeIdentifier *rel_ident6 = dynamic_cast<RelativeIdentifier *>(var_expr6->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident6);
          CPPUNIT_ASSERT_EQUAL(false, rel_ident6->has_key_ident());
          CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), rel_ident6->index());
          arg_iter3++;
          VariableExpression *var_expr7 = dynamic_cast<VariableExpression *>(arg_iter3->get());
          CPPUNIT_ASSERT(nullptr != var_expr7);
          RelativeIdentifier *rel_ident7 = dynamic_cast<RelativeIdentifier *>(var_expr7->ident());
          CPPUNIT_ASSERT(nullptr != rel_ident7);
          CPPUNIT_ASSERT_EQUAL(true, rel_ident7->has_key_ident());
          CPPUNIT_ASSERT(v_abs_ident.key_ident() == rel_ident7->key_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
          CPPUNIT_ASSERT(nullptr != var_info->insts().get());
          CPPUNIT_ASSERT_EQUAL(true, var_info->insts()->empty());
          CPPUNIT_ASSERT_EQUAL(AccessModifier::NONE, var_info->constr_access_modifier());
          CPPUNIT_ASSERT(nullptr == var_info->datatype_ident());
        }
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), tree.uncompiled_var_key_idents().size());
        CPPUNIT_ASSERT(v_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[0]);
        CPPUNIT_ASSERT(w_abs_ident.key_ident() == tree.uncompiled_var_key_idents()[1]);
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_var_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_key_idents().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_inst_pairs().empty());
        CPPUNIT_ASSERT_EQUAL(true, tree.uncompiled_type_fun_inst_pairs().empty());
      }
    }
  }
}
