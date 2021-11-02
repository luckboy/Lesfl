/****************************************************************************
 *   Copyright (C) 2016, 2021 Łukasz Szpakowski.                            *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _LESFL_FRONTEND_TREE_H
#define _LESFL_FRONTEND_TREE_H

#include <cstddef>
#include <cstdint>
#include <vector>
#include <utility>
#include <lesfl/frontend/builtin.hpp>
#include <lesfl/frontend/ident.hpp>
#include <lesfl/comp.hpp>

namespace lesfl
{
  namespace frontend
  {
    class Definition;
    class Variable;
    class DefinableVariable;
    class VariableInstance;
    class Function;
    class DefinableFunction;
    class FunctionInstance;
    class Argument;
    class Annotation;
    class Expression;
    class ExpressionNamedFieldPair;
    class Binding;
    class TupleBindingVariable;
    class Case;
    class Pattern;
    class PatternNamedFieldPair;
    class LiteralValue;
    class SimpleLiteralValue;
    class Value;
    class ValueNamedFieldPair;
    class TypeVariable;
    class DefinableTypeVariable;
    class TypeFunction;
    class DefinableTypeFunction;
    class TypeFunctionInstance;
    class Datatype;
    class Constructor;
    class FunctionConstructor;
    class TypeArgument;
    class TypeParameter;
    class TypeNamedFieldPair;
    class TypeExpression;

    enum class IntType
    {
      INT8,
      INT16,
      INT32,
      INT64
    };

    enum class FloatType
    {
      SINGLE,
      DOUBLE
    };

    enum class AccessModifier
    {
      NONE,
      PRIVATE
    };

    enum class InlineModifier
    {
      NONE,
      INLINE
    };
    
    enum class FunctionModifier
    {
      NONE,
      PRIMITIVE
    };

    class Positional
    {
    protected:
      Position _M_pos;

      Positional(const Position &pos) : _M_pos(pos) {}
    public:
      virtual ~Positional();
      
      const Position &pos() const { return _M_pos; }      
    };

    class Accessible
    {
    protected:
      AccessModifier _M_access_modifier;

      Accessible(AccessModifier access_modifier) : _M_access_modifier(access_modifier) {}
    public:
      virtual ~Accessible();
      
      AccessModifier access_modifier() const { return _M_access_modifier; }
    };
    
    class Inlinable
    {
    protected:
      InlineModifier _M_inline_modifier;
      
      Inlinable(InlineModifier inline_modifier) : _M_inline_modifier(inline_modifier) {}
    public:
      virtual ~Inlinable();
      
      InlineModifier inline_modifier() const { return _M_inline_modifier; }
    };

    class Identifiable
    {
    protected:
      std::string _M_ident;

      Identifiable() {}

      Identifiable(const char *ident) : _M_ident(ident) {}

      Identifiable(const std::string &ident) : _M_ident(ident) {}
    public:
      virtual ~Identifiable();

      const std::string &ident() const { return _M_ident; }

      std::string to_ident_string() const;
    };

    class IdentifiableAndIndexable : public Identifiable, public Indexable
    {
    protected:
      IdentifiableAndIndexable(const char *ident) : Identifiable(ident) {}

      IdentifiableAndIndexable(const std::string &ident) : Identifiable(ident) {}
    public:
      ~IdentifiableAndIndexable();
    };

    class Instance
    {
    protected:
      Instance() {}
    public:
      virtual ~Instance();
    };

    class VariableInfo : public Accessible
    {
      std::shared_ptr<Variable> _M_var;
      std::shared_ptr<std::list<std::shared_ptr<Instance>>> _M_insts;
      AccessModifier _M_constr_access_modifier;
      const std::string *_M_datatype_ident;
    public:
      VariableInfo(AccessModifier access_modifier, const std::shared_ptr<Variable> &var, AccessModifier constr_access_modifier = AccessModifier::NONE, const std::string *datatype_ident = nullptr) :
        Accessible(constr_access_modifier == AccessModifier::PRIVATE ? constr_access_modifier : access_modifier), _M_var(var), _M_insts(std::shared_ptr<std::list<std::shared_ptr<Instance>>>(new std::list<std::shared_ptr<Instance>>())), _M_constr_access_modifier(constr_access_modifier), _M_datatype_ident(datatype_ident) {}

      ~VariableInfo();

      const std::shared_ptr<Variable> &var() const { return _M_var; }

      const std::shared_ptr<std::list<std::shared_ptr<Instance>>> &insts() const { return _M_insts; }

      void add_inst(const std::shared_ptr<Instance> &inst)
      { _M_insts->push_back(inst); }

      bool must_update_access_modifier() const { return _M_datatype_ident != nullptr; }

      AccessModifier constr_access_modifier() const { return _M_constr_access_modifier; }
      
      const std::string *datatype_ident() const { return _M_datatype_ident; }

      void update_access_modifier(AccessModifier access_modifier)
      {
        _M_access_modifier = (_M_constr_access_modifier == AccessModifier::PRIVATE ? _M_constr_access_modifier : access_modifier);
        _M_datatype_ident = nullptr;
      }
    };

    class TypeVariableInfo : public Accessible
    {
      std::shared_ptr<TypeVariable> _M_var;
    public:
      TypeVariableInfo(AccessModifier access_modifier, const std::shared_ptr<TypeVariable> &var) :
        Accessible(access_modifier), _M_var(var) {}

      ~TypeVariableInfo();

      const std::shared_ptr<TypeVariable> &var() const { return _M_var; }
    };

    class TypeFunctionInfo : public Accessible
    {
      std::shared_ptr<TypeFunction> _M_fun;
      std::shared_ptr<std::list<std::shared_ptr<TypeFunctionInstance>>> _M_insts;
    public:
      TypeFunctionInfo(AccessModifier access_modifier, const std::shared_ptr<TypeFunction> &fun) :
        Accessible(access_modifier), _M_fun(fun), _M_insts(std::shared_ptr<std::list<std::shared_ptr<TypeFunctionInstance>>>(new std::list<std::shared_ptr<TypeFunctionInstance>>())) {}

      virtual ~TypeFunctionInfo();

      const std::shared_ptr<TypeFunction> &fun() const { return _M_fun; }

      const std::shared_ptr<std::list<std::shared_ptr<TypeFunctionInstance>>> &fun_insts() const { return _M_insts; }
      
      void add_inst(const std::shared_ptr<TypeFunctionInstance> &inst)
      { _M_insts->push_back(inst); }
    };

    class Tree
    {
      std::list<std::unique_ptr<const std::list<std::unique_ptr<Definition>>>> _M_defs;
      std::shared_ptr<AbsoluteIdentifierTable> _M_ident_table;
      std::unordered_set<KeyIdentifier> _M_module_key_idents;
      std::unordered_map<KeyIdentifier, VariableInfo> _M_var_infos;
      std::unordered_map<KeyIdentifier, TypeVariableInfo> _M_type_var_infos;
      std::unordered_map<KeyIdentifier, TypeFunctionInfo> _M_type_fun_infos;
      std::vector<KeyIdentifier> _M_uncompiled_var_key_idents;
      std::vector<KeyIdentifier> _M_uncompiled_type_var_key_idents;
      std::vector<KeyIdentifier> _M_uncompiled_type_fun_key_idents;
    public:
      Tree() : _M_ident_table(std::shared_ptr<AbsoluteIdentifierTable>(new AbsoluteIdentifierTable())) {}

      Tree(const std::shared_ptr<AbsoluteIdentifierTable> &ident_table) : _M_ident_table(ident_table) {}

      virtual ~Tree();

      const std::list<std::unique_ptr<const std::list<std::unique_ptr<Definition>>>> &defs() const
      { return _M_defs; }

      void add_defs(const std::list<std::unique_ptr<Definition>> *defs)
      { _M_defs.push_back(std::unique_ptr<const std::list<std::unique_ptr<Definition>>>(defs)); }

      const std::shared_ptr<AbsoluteIdentifierTable> &ident_table() const
      { return _M_ident_table; }

      const std::unordered_set<KeyIdentifier> &module_key_idents() const
      { return _M_module_key_idents; }

      bool has_module_key_ident(KeyIdentifier key_ident) const
      { return _M_module_key_idents.find(key_ident) == _M_module_key_idents.end(); }

      bool has_module_key_ident(const Identifier &ident) const
      { return has_module_key_ident(ident.key_ident()); }

      bool add_module(KeyIdentifier key_ident)
      { return _M_module_key_idents.insert(key_ident).second; }

      const std::unordered_map<KeyIdentifier, VariableInfo> &var_infos() const
      { return _M_var_infos; }

      const VariableInfo *var_info(KeyIdentifier key_ident) const
      {
        auto iter = _M_var_infos.find(key_ident);
        return iter != _M_var_infos.end() ? &(iter->second) : nullptr;
      }

      const VariableInfo *var_info(const Identifier &ident) const
      { return var_info(ident.key_ident()); }

      VariableInfo *var_info(KeyIdentifier key_ident)
      {
        auto iter = _M_var_infos.find(key_ident);
        return iter != _M_var_infos.end() ? &(iter->second) : nullptr;
      }

      VariableInfo *var_info(const Identifier &ident)
      { return var_info(ident.key_ident()); }

      std::shared_ptr<Variable> var(KeyIdentifier key_ident) const
      {
        const VariableInfo *info = var_info(key_ident);
        return info != nullptr ? info->var() : std::shared_ptr<Variable>();
      }

      std::shared_ptr<Variable> var(const Identifier &ident) const
      { return var(ident.key_ident()); }

      bool add_var(KeyIdentifier key_ident, AccessModifier access_modifier, const std::shared_ptr<Variable> &var, AccessModifier constr_access_modifier = AccessModifier::NONE, const std::string *datatype_ident = nullptr)
      { return _M_var_infos.insert(std::make_pair(key_ident, VariableInfo(access_modifier, var, constr_access_modifier, datatype_ident))).second; }

      const std::unordered_map<KeyIdentifier, TypeVariableInfo> &type_var_infos() const
      { return _M_type_var_infos; }

      const TypeVariableInfo *type_var_info(KeyIdentifier key_ident) const
      {
        auto iter = _M_type_var_infos.find(key_ident);
        return iter != _M_type_var_infos.end() ? &(iter->second) : nullptr;
      }

      const TypeVariableInfo *type_var_info(const Identifier &ident) const
      { return type_var_info(ident.key_ident()); }

      TypeVariableInfo *type_var_info(KeyIdentifier key_ident)
      {
        auto iter = _M_type_var_infos.find(key_ident);
        return iter != _M_type_var_infos.end() ? &(iter->second) : nullptr;
      }

      TypeVariableInfo *type_var_info(const Identifier &ident)
      { return type_var_info(ident.key_ident()); }

      std::shared_ptr<TypeVariable> type_var(KeyIdentifier key_ident) const
      {
        const TypeVariableInfo *info = type_var_info(key_ident);
        return info != nullptr ? info->var() : std::shared_ptr<TypeVariable>();
      }

      std::shared_ptr<TypeVariable> type_var(const Identifier &ident) const
      { return type_var(ident.key_ident()); }

      bool add_type_var(KeyIdentifier key_ident, AccessModifier access_modifier, const std::shared_ptr<TypeVariable> &var)
      { return _M_type_var_infos.insert(std::make_pair(key_ident, TypeVariableInfo(access_modifier, var))).second; }

      const std::unordered_map<KeyIdentifier, TypeFunctionInfo> &type_fun_infos() const
      { return _M_type_fun_infos; }

      const TypeFunctionInfo *type_fun_info(KeyIdentifier key_ident) const
      {
        auto iter = _M_type_fun_infos.find(key_ident);
        return iter != _M_type_fun_infos.end() ? &(iter->second) : nullptr;
      }

      const TypeFunctionInfo *type_fun_info(const Identifier &ident) const
      { return type_fun_info(ident.key_ident()); }

      TypeFunctionInfo *type_fun_info(KeyIdentifier key_ident)
      {
        auto iter = _M_type_fun_infos.find(key_ident);
        return iter != _M_type_fun_infos.end() ? &(iter->second) : nullptr;
      }

      TypeFunctionInfo *type_fun_info(const Identifier &ident)
      { return type_fun_info(ident.key_ident()); }

      std::shared_ptr<TypeFunction> type_fun(KeyIdentifier key_ident) const
      {
        const TypeFunctionInfo *info = type_fun_info(key_ident);
        return info != nullptr ? info->fun() : std::shared_ptr<TypeFunction>();
      }

      std::shared_ptr<TypeFunction> type_fun(const Identifier &ident) const
      { return type_fun(ident.key_ident()); }

      bool add_type_fun(KeyIdentifier key_ident, AccessModifier access_modifier, const std::shared_ptr<TypeFunction> &fun)
      { return _M_type_fun_infos.insert(std::make_pair(key_ident, TypeFunctionInfo(access_modifier, fun))).second; }
      
      const std::vector<KeyIdentifier> &uncompiled_var_key_idents() const { return _M_uncompiled_var_key_idents; }

      std::vector<KeyIdentifier> &uncompiled_var_key_idents() { return _M_uncompiled_var_key_idents; }

      const std::vector<KeyIdentifier> &uncompiled_type_var_key_idents() const { return _M_uncompiled_type_var_key_idents; }

      std::vector<KeyIdentifier> &uncompiled_type_var_key_idents() { return _M_uncompiled_type_var_key_idents; }

      const std::vector<KeyIdentifier> &uncompiled_type_fun_key_idents() const { return _M_uncompiled_type_fun_key_idents; }

      std::vector<KeyIdentifier> &uncompiled_type_fun_key_idents() { return _M_uncompiled_type_fun_key_idents; }
    };

    class Definition : public Positional
    {
    protected:
      Definition(const Position &pos) : Positional(pos) {}
    public:
      ~Definition();
    };

    class Import : public Definition
    {
      std::unique_ptr<Identifier> _M_module_ident;
    public:
      Import(Identifier *module_ident, const Position &pos) :
        Definition(pos), _M_module_ident(module_ident) {}

      ~Import();

      Identifier *module_ident() const { return _M_module_ident.get(); }
    };

    class ModuleDefinition : public Definition
    {
      std::unique_ptr<Identifier> _M_ident;
      std::unique_ptr<const std::list<std::unique_ptr<Definition>>> _M_defs;
    public:
      ModuleDefinition(Identifier *ident, const std::list<std::unique_ptr<Definition>> *defs, const Position &pos) :
        Definition(pos), _M_ident(ident), _M_defs(defs) {}

      ~ModuleDefinition();

      Identifier *ident() const { return _M_ident.get(); }

      const std::list<std::unique_ptr<Definition>> &defs() const { return *_M_defs; }
    };

    class VariableDefinition : public Definition, public Accessible
    {
      std::string _M_ident;
      std::shared_ptr<DefinableVariable> _M_var;
    public:
      VariableDefinition(AccessModifier access_modifier, const std::string &ident, DefinableVariable *var, const Position &pos) :
        Definition(pos), Accessible(access_modifier), _M_ident(ident), _M_var(var) {}

      ~VariableDefinition();

      const std::string &ident() const { return _M_ident; }

      const std::shared_ptr<DefinableVariable> &var() const { return _M_var; }
    };

    class VariableInstanceDefinition : public Definition
    {
      std::string _M_ident;
      std::shared_ptr<VariableInstance> _M_var_inst;
    public:
      VariableInstanceDefinition(const std::string &ident, VariableInstance *var_inst, const Position &pos) :
        Definition(pos), _M_ident(ident), _M_var_inst(var_inst) {}

      ~VariableInstanceDefinition();

      const std::string &ident() const { return _M_ident; }

      const std::shared_ptr<VariableInstance> &var_inst() const { return _M_var_inst; }
    };

    class FunctionDefinition : public Definition, public Accessible
    {
      std::string _M_ident;
      std::shared_ptr<DefinableFunction> _M_fun;
    public:
      FunctionDefinition(AccessModifier access_modifier, const std::string &ident, DefinableFunction *fun, const Position &pos) :
        Definition(pos), Accessible(access_modifier), _M_ident(ident), _M_fun(fun) {}

      ~FunctionDefinition();

      const std::string &ident() const { return _M_ident; }

      const std::shared_ptr<DefinableFunction> &fun() const { return _M_fun; }
    };

    class FunctionInstanceDefinition : public Definition
    {
      std::string _M_ident;
      std::shared_ptr<FunctionInstance> _M_fun_inst;
    public:
      FunctionInstanceDefinition(const std::string &ident, FunctionInstance *fun_inst, const Position &pos) :
        Definition(pos), _M_ident(ident), _M_fun_inst(fun_inst) {}

      ~FunctionInstanceDefinition();

      const std::string &ident() const { return _M_ident; }

      const std::shared_ptr<FunctionInstance> &fun_inst() const { return _M_fun_inst; }
    };

    class TypeVariableDefinition : public Definition, public Accessible
    {
      std::string _M_ident;
      std::shared_ptr<DefinableTypeVariable> _M_var;
    public:
      TypeVariableDefinition(AccessModifier access_modifier, const std::string &ident, DefinableTypeVariable *var, const Position &pos) :
        Definition(pos), Accessible(access_modifier), _M_ident(ident), _M_var(var) {}

      ~TypeVariableDefinition();

      const std::string &ident() const { return _M_ident; }

      const std::shared_ptr<DefinableTypeVariable> &var() const { return _M_var; }
    };

    class TypeFunctionDefinition : public Definition, public Accessible
    {
      std::string _M_ident;
      std::shared_ptr<DefinableTypeFunction> _M_fun;
    public:
      TypeFunctionDefinition(AccessModifier access_modifier, const std::string &ident, DefinableTypeFunction *fun, const Position &pos) :
        Definition(pos), Accessible(access_modifier), _M_ident(ident), _M_fun(fun) {}

      ~TypeFunctionDefinition();

      const std::string &ident() const { return _M_ident; }

      const std::shared_ptr<DefinableTypeFunction> &fun() const { return _M_fun; }
    };

    class TypeFunctionInstanceDefinition : public Definition
    {
      std::string _M_ident;
      std::shared_ptr<TypeFunctionInstance> _M_fun_inst;
    public:
      TypeFunctionInstanceDefinition(const std::string &ident, TypeFunctionInstance *fun_inst, const Position &pos) :
        Definition(pos), _M_ident(ident), _M_fun_inst(fun_inst) {}

      ~TypeFunctionInstanceDefinition();

      const std::string &ident() const { return _M_ident; }

      const std::shared_ptr<TypeFunctionInstance> &fun_inst() const { return _M_fun_inst; }
    };

    class Variable
    {
    protected:
      Variable() {}
    public:
      virtual ~Variable();
    };

    class DefinableVariable : public Variable
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<TypeParameter>>> _M_inst_type_params;
      std::unique_ptr<TypeExpression> _M_type_expr;

      DefinableVariable() : _M_inst_type_params(nullptr), _M_type_expr(nullptr) {}

      DefinableVariable(TypeExpression *type_expr) :
        _M_inst_type_params(nullptr), _M_type_expr(type_expr) {}

      DefinableVariable(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params) :
        _M_inst_type_params(inst_type_params), _M_type_expr(nullptr) {}

      DefinableVariable(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, TypeExpression *type_expr) :
        _M_inst_type_params(inst_type_params), _M_type_expr(type_expr) {}
    public:
      ~DefinableVariable();

      bool is_template() const { return _M_inst_type_params.get() != nullptr; }

      const std::list<std::unique_ptr<TypeParameter>> &inst_type_params() const { return *_M_inst_type_params; }

      TypeExpression *type_expr() const { return _M_type_expr.get(); }
    };    

    class UserDefinedVariable : public DefinableVariable
    {
      std::unique_ptr<Value> _M_value;
    public:
      UserDefinedVariable(Value *value) : _M_value(value) {}

      UserDefinedVariable(TypeExpression *type_expr, Value *value) :
        DefinableVariable(type_expr), _M_value(value) {}

      UserDefinedVariable(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, Value *value) :
        DefinableVariable(inst_type_params), _M_value(value) {}

      UserDefinedVariable(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, TypeExpression *type_expr, Value *value) :
        DefinableVariable(inst_type_params, type_expr), _M_value(value) {}

      UserDefinedVariable(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, TypeExpression *type_expr) :
        DefinableVariable(inst_type_params, type_expr), _M_value(nullptr) {}

      ~UserDefinedVariable();

      Value *value() const { return _M_value.get(); }
    };

    class ExternalVariable : public DefinableVariable
    {
      std::string _M_external_var_ident;
    public:
      ExternalVariable(TypeExpression *type_expr, const std::string &external_var_ident) :
        DefinableVariable(type_expr), _M_external_var_ident(external_var_ident) {}

      ~ExternalVariable();

      const std::string &external_var_ident() const { return _M_external_var_ident; }
    };

    class AliasVariable : public DefinableVariable
    {
      std::unique_ptr<Identifier> _M_ident;
    public:
      AliasVariable(TypeExpression *type_expr, Identifier *ident) :
        DefinableVariable(type_expr), _M_ident(ident) {}

      AliasVariable(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, TypeExpression *type_expr, Identifier *ident) :
        DefinableVariable(inst_type_params, type_expr), _M_ident(ident) {}

      ~AliasVariable();

      Identifier *ident() const { return _M_ident.get(); }
    };

    class FunctionVariable : public Variable
    {
      std::shared_ptr<Function> _M_fun;
    public:
      FunctionVariable(const std::shared_ptr<Function> &fun) : _M_fun(fun) {}

      ~FunctionVariable();

      const std::shared_ptr<Function> &fun() const { return _M_fun; }
    };

    class ConstructorVariable : public Variable
    {
      std::shared_ptr<Constructor> _M_fun;
    public:
      ConstructorVariable(const std::shared_ptr<Constructor> &constr) : _M_fun(constr) {}

      ~ConstructorVariable();

      const std::shared_ptr<Constructor> &constr() const { return _M_fun; }
    };

    class LibraryVariable : public Variable
    {
    public:
      LibraryVariable();

      ~LibraryVariable();
    };

    class VariableInstance : public Instance
    {
      std::shared_ptr<Variable> _M_var;
    public:
      VariableInstance(Variable *var) : _M_var(var) {}

      ~VariableInstance();

      const std::shared_ptr<Variable> &var() const { return _M_var; }
    };

    class Function
    {
    protected:
      std::size_t _M_arg_count;

      Function(std::size_t arg_count) : _M_arg_count(arg_count) {}
    public:
      virtual ~Function();

      std::size_t arg_count() const { return _M_arg_count; }
    };

    class DefinableFunction : public Function
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<TypeParameter>>> _M_inst_type_params;
      std::unique_ptr<const std::list<std::unique_ptr<Annotation>>> _M_annotations;
      FunctionModifier _M_fun_modifier;
      std::unique_ptr<const std::list<std::unique_ptr<Argument>>> _M_args;
      std::unique_ptr<TypeExpression> _M_result_type_expr;

      DefinableFunction(const std::list<std::unique_ptr<Annotation>> *annotations, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args) :
        Function(args->size()), _M_inst_type_params(nullptr), _M_annotations(annotations), _M_fun_modifier(fun_modifier), _M_args(args), _M_result_type_expr(nullptr) {}

      DefinableFunction(const std::list<std::unique_ptr<Annotation>> *annotations, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr) :
        Function(args->size()), _M_inst_type_params(nullptr), _M_annotations(annotations), _M_fun_modifier(fun_modifier), _M_args(args), _M_result_type_expr(result_type_expr) {}

      DefinableFunction(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, const std::list<std::unique_ptr<Annotation>> *annotations, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args) :
        Function(args->size()), _M_inst_type_params(inst_type_params), _M_annotations(annotations), _M_fun_modifier(fun_modifier), _M_args(args), _M_result_type_expr(nullptr) {}

      DefinableFunction(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, const std::list<std::unique_ptr<Annotation>> *annotations, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr) :
        Function(args->size()), _M_inst_type_params(inst_type_params), _M_annotations(annotations), _M_fun_modifier(fun_modifier), _M_args(args), _M_result_type_expr(result_type_expr) {}
    public:
      ~DefinableFunction();

      bool is_template() const { return _M_inst_type_params.get() != nullptr; }

      const std::list<std::unique_ptr<TypeParameter>> &inst_type_params() const { return *_M_inst_type_params; }

      const std::list<std::unique_ptr<Annotation>> &annotations() const { return *_M_annotations; }

      FunctionModifier fun_modifier() const { return _M_fun_modifier; }

      const std::list<std::unique_ptr<Argument>> &args() const { return *_M_args; }

      std::size_t arg_count() const { return _M_arg_count; }
      
      TypeExpression *result_type_expr() const { return _M_result_type_expr.get(); }
    };

    class UserDefinedFunction : public DefinableFunction, public Inlinable
    {
      std::unique_ptr<Expression> _M_body;
    public:
      UserDefinedFunction(const std::list<std::unique_ptr<Annotation>> *annotations, InlineModifier inline_modifier, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, Expression *body) :
        DefinableFunction(annotations, fun_modifier, args), Inlinable(inline_modifier), _M_body(body) {}

      UserDefinedFunction(const std::list<std::unique_ptr<Annotation>> *annotations, InlineModifier inline_modifier, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr, Expression *body) :
        DefinableFunction(annotations, fun_modifier, args, result_type_expr), Inlinable(inline_modifier), _M_body(body) {}

      UserDefinedFunction(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, const std::list<std::unique_ptr<Annotation>> *annotations, InlineModifier inline_modifier, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, Expression *body) :
        DefinableFunction(inst_type_params, annotations, fun_modifier, args), Inlinable(inline_modifier), _M_body(body) {}

      UserDefinedFunction(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, const std::list<std::unique_ptr<Annotation>> *annotations, InlineModifier inline_modifier, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr, Expression *body) :
        DefinableFunction(inst_type_params, annotations, fun_modifier, args, result_type_expr), Inlinable(inline_modifier), _M_body(body) {}

      UserDefinedFunction(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr) :
        DefinableFunction(inst_type_params, new std::list<std::unique_ptr<Annotation>>(), fun_modifier, args, result_type_expr), Inlinable(InlineModifier::NONE), _M_body(nullptr) {}

      ~UserDefinedFunction();

      Expression *body() const { return _M_body.get(); }
    };

    class ExternalFunction : public DefinableFunction
    {
      std::string _M_external_fun_ident;
    public:
      ExternalFunction(FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr, const std::string &external_fun_ident) :
        DefinableFunction(new std::list<std::unique_ptr<Annotation>>(), fun_modifier, args, result_type_expr), _M_external_fun_ident(external_fun_ident) {}

      ~ExternalFunction();

      const std::string &external_fun_ident() const { return _M_external_fun_ident; }
    };

    class NativeFunction : public DefinableFunction, public Inlinable
    {
      std::string _M_native_fun_ident;
    public:
      NativeFunction(const std::list<std::unique_ptr<Annotation>> *annotations, InlineModifier inline_modifier, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr, const std::string &native_fun_ident) :
        DefinableFunction(annotations, fun_modifier, args, result_type_expr), Inlinable(inline_modifier), _M_native_fun_ident(native_fun_ident) {}

      ~NativeFunction();

      const std::string &native_fun_ident() const { return _M_native_fun_ident; }
    };

    class FunctionInstance : public Instance
    {
      std::shared_ptr<Function> _M_fun;
    public:
      FunctionInstance(Function *fun) : _M_fun(fun) {}

      ~FunctionInstance();

      const std::shared_ptr<Function> &fun() const { return _M_fun; }
    };

    class Argument : public Positional, public IdentifiableAndIndexable
    {
      std::unique_ptr<TypeExpression> _M_type_expr;
    public:
      Argument(const std::string &ident, const Position &pos) :
        Positional(pos), IdentifiableAndIndexable(ident), _M_type_expr(nullptr) {}

      Argument(const std::string &ident, TypeExpression *type_expr, const Position &pos) :
        Positional(pos), IdentifiableAndIndexable(ident), _M_type_expr(type_expr) {}

      ~Argument();

      TypeExpression *type_expr() const { return _M_type_expr.get(); }
    };

    class Annotation : public Positional, public Stringable
    {
      std::string _M_ident;
    public:
      Annotation(const std::string &ident, const Position &pos) :
        Positional(pos), _M_ident(ident) {}

      ~Annotation();

      const std::string &ident() const { return _M_ident; }

      std::string to_string() const;
    };

    class Expression : public Positional
    {
    protected:
      Expression(const Position &pos) : Positional(pos) {}
    public:
      ~Expression();
    };

    class Literal : public Expression
    {
      std::unique_ptr<LiteralValue> _M_literal_value;
    public:
      Literal(LiteralValue *value, const Position &pos) : Expression(pos), _M_literal_value(value) {}

      ~Literal();

      LiteralValue *literal_value() const { return _M_literal_value.get(); }
    };

    class Collection : public Expression
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<Expression>>> _M_elems;

      Collection(const std::list<std::unique_ptr<Expression>> *elems, const Position &pos) :
        Expression(pos), _M_elems(elems) {}
    public:
      ~Collection();

      const std::list<std::unique_ptr<Expression>> &elems() const { return *_M_elems; } 
    };

    class List : public Collection
    {
    public:
      List(const std::list<std::unique_ptr<Expression>> *elems, const Position &pos) : Collection(elems, pos) {}

      ~List();
    };

    class Array : public Collection
    {
    protected:
      Array(const std::list<std::unique_ptr<Expression>> *elems, const Position &pos) : Collection(elems, pos) {}
    public:
      ~Array();
    };

    class NonUniqueArray : public Array
    {
    public:
      NonUniqueArray(const std::list<std::unique_ptr<Expression>> *elems, const Position &pos) : Array(elems, pos) {}

      ~NonUniqueArray();
    };

    class UniqueArray : public Array
    {
    public:
      UniqueArray(const std::list<std::unique_ptr<Expression>> *elems, const Position &pos) : Array(elems, pos) {}

      ~UniqueArray();
    };

    class Tuple : public Expression
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<Expression>>> _M_fields;

      Tuple(const std::list<std::unique_ptr<Expression>> *fields, const Position &pos) :
        Expression(pos), _M_fields(fields) {}
    public:
      ~Tuple();

      const std::list<std::unique_ptr<Expression>> &fields() const { return *_M_fields; }
    };

    class NonUniqueTuple : public Tuple
    {
    public:
      NonUniqueTuple(const std::list<std::unique_ptr<Expression>> *fields, const Position &pos) : Tuple(fields, pos) {}

      ~NonUniqueTuple();
    };

    class UniqueTuple : public Tuple
    {
    public:
      UniqueTuple(const std::list<std::unique_ptr<Expression>> *fields, const Position &pos) : Tuple(fields, pos) {}

      ~UniqueTuple();
    };

    class VariableExpression : public Expression
    {
      std::unique_ptr<Identifier> _M_ident;
    public:
      VariableExpression(Identifier *ident, const Position &pos) : Expression(pos), _M_ident(ident) {}

      ~VariableExpression(); 

      Identifier *ident() const { return _M_ident.get(); }
    };

    class NamedFieldConstructorApplication : public Expression
    {
      std::unique_ptr<Identifier> _M_constr_ident;
      std::unique_ptr<const std::list<std::unique_ptr<ExpressionNamedFieldPair>>> _M_fields;
    public:
      NamedFieldConstructorApplication(Identifier *constr_ident, const std::list<std::unique_ptr<ExpressionNamedFieldPair>> *fields, const Position &pos) :
        Expression(pos), _M_constr_ident(constr_ident), _M_fields(fields) {}

      ~NamedFieldConstructorApplication();

      Identifier *constr_ident() const { return _M_constr_ident.get(); }

      const std::list<std::unique_ptr<ExpressionNamedFieldPair>> &fields() const { return *_M_fields; }
    };

    class Application : public Expression
    {
      std::unique_ptr<Expression> _M_fun;
      std::unique_ptr<const std::list<std::unique_ptr<Expression>>> _M_args;
    protected:
      Application(Expression *fun, const std::list<std::unique_ptr<Expression>> *args, const Position &pos) :
        Expression(pos), _M_fun(fun), _M_args(args) {}
    public:
      ~Application();

      Expression *fun() const { return _M_fun.get(); }

      const std::list<std::unique_ptr<Expression>> &args() const { return *_M_args; }
    };

    class NonUniqueApplication : public Application
    {
      FunctionModifier _M_fun_modifier;
    public:
      NonUniqueApplication(Expression *fun, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Expression>> *args, const Position &pos) :
        Application(fun, args, pos), _M_fun_modifier(fun_modifier) {}

      ~NonUniqueApplication();

      FunctionModifier fun_modifier() const { return _M_fun_modifier; }
    };

    class UniqueApplication : public Application
    {
    public:
      UniqueApplication(Expression *fun, const std::list<std::unique_ptr<Expression>> *args, const Position &pos) :
        Application(fun, args, pos) {}

      ~UniqueApplication();
    };

    class BuiltinApplication : public Expression
    {
      BuiltinFunction _M_fun;
      std::unique_ptr<const std::list<std::unique_ptr<Expression>>> _M_args;
    public:
      BuiltinApplication(BuiltinFunction fun, const std::list<std::unique_ptr<Expression>> *args, const Position &pos) :
        Expression(pos), _M_fun(fun), _M_args(args) {}

      ~BuiltinApplication();

      BuiltinFunction fun() const { return _M_fun; }

      const std::list<std::unique_ptr<Expression>> &args() const { return *_M_args; }
    };

    class FieldOperator : public Expression
    {
    protected:
      std::unique_ptr<Expression> _M_expr;
      std::int64_t _M_i;

      FieldOperator(Expression *expr, std::int64_t i, const Position &pos) :
        Expression(pos), _M_expr(expr), _M_i(i) {}
    public:
      ~FieldOperator();

      Expression *expr() const { return _M_expr.get(); }

      std::int64_t i() const { return _M_i; }
    };

    class Field : public FieldOperator
    {
    public:
      Field(Expression *expr, std::int64_t i, const Position &pos) :
        FieldOperator(expr, i, pos) {}

      ~Field();
    };

    class UniqueField : public FieldOperator
    {
    public:
      UniqueField(Expression *expr, std::int64_t i, const Position &pos) :
        FieldOperator(expr, i, pos) {}

      ~UniqueField();
    };

    class SetUniqueField : public FieldOperator
    {
      std::unique_ptr<Expression> _M_value_expr;
    public:
      SetUniqueField(Expression *expr, std::int64_t i, Expression *value_expr, const Position &pos) :
        FieldOperator(expr, i, pos), _M_value_expr(value_expr) {}

      ~SetUniqueField();

      Expression *value_expr() const { return _M_value_expr.get(); }
    };

    class NamedFieldOperator : public Expression, public Identifiable
    {
    protected:
      std::unique_ptr<Expression> _M_expr;

      NamedFieldOperator(Expression *expr, const std::string &ident, const Position &pos) :
        Expression(pos), Identifiable(ident), _M_expr(expr) {}
    public:
      ~NamedFieldOperator();

      Expression *expr() const { return _M_expr.get(); }
    };

    class NamedField : public NamedFieldOperator
    {
    public:
      NamedField(Expression *expr, const std::string &ident, const Position &pos) :
        NamedFieldOperator(expr, ident, pos) {}

      ~NamedField();
    };

    class UniqueNamedField : public NamedFieldOperator
    {
    public:
      UniqueNamedField(Expression *expr, const std::string &ident, const Position &pos) :
        NamedFieldOperator(expr, ident, pos) {}

      ~UniqueNamedField();
    };

    class SetUniqueNamedField : public NamedFieldOperator
    {
      std::unique_ptr<Expression> _M_value_expr;
    public:
      SetUniqueNamedField(Expression *expr, const std::string &ident, Expression *value_expr, const Position &pos) :
        NamedFieldOperator(expr, ident, pos), _M_value_expr(value_expr) {}

      ~SetUniqueNamedField();

      Expression *value_expr() const { return _M_value_expr.get(); }
    };

    class TypedExpression : public Expression
    {
      std::unique_ptr<Expression> _M_expr;
      std::unique_ptr<TypeExpression> _M_type_expr;
    public:
      TypedExpression(Expression *expr, TypeExpression *type_expr, const Position &pos) :
        Expression(pos), _M_expr(expr), _M_type_expr(type_expr) {}

      ~TypedExpression();
      
      Expression *expr() const { return _M_expr.get(); }

      TypeExpression *type_expr() const { return _M_type_expr.get(); }
    };

    class Let : public Expression
    {
      std::unique_ptr<const std::list<std::unique_ptr<Binding>>> _M_binds;
      std::unique_ptr<Expression> _M_expr;
    public:
      Let(const std::list<std::unique_ptr<Binding>> *binds, Expression *expr, const Position &pos) :
        Expression(pos), _M_binds(binds), _M_expr(expr) {}

      ~Let();

      const std::list<std::unique_ptr<Binding>> &binds() const { return *_M_binds; }

      Expression *expr() const { return _M_expr.get(); }
    };

    class Match : public Expression
    {
      std::unique_ptr<Expression> _M_expr;
      std::unique_ptr<const std::list<std::unique_ptr<Case>>> _M_cases;
    public:
      Match(Expression *expr, const std::list<std::unique_ptr<Case>> *cases, const Position &pos) :
        Expression(pos), _M_expr(expr), _M_cases(cases) {}

      ~Match();

      Expression *expr() const { return _M_expr.get(); }

      const std::list<std::unique_ptr<Case>> &cases() const { return *_M_cases; }      
    };

    class Throw : public Expression
    {
      std::unique_ptr<Expression> _M_expr;
    public:
      Throw(Expression *expr, const Position &pos) : Expression(pos), _M_expr(expr) {}

      ~Throw();

      Expression *expr() const { return _M_expr.get(); }
    };

    class ExpressionNamedFieldPair : public Positional, public IdentifiableAndIndexable
    {
      std::unique_ptr<Expression> _M_expr;
    public:
      ExpressionNamedFieldPair(const std::string &ident, Expression *expr, const Position &pos) :
        Positional(pos), IdentifiableAndIndexable(ident), _M_expr(expr) {}

      ~ExpressionNamedFieldPair();

      Expression *expr() const { return _M_expr.get(); }
    };

    class Binding
    {
    protected:
      Binding() {}
    public:
      virtual ~Binding();
    };

    class VariableBinding : public Binding, public Positional, public IdentifiableAndIndexable
    {
      Expression *_M_expr;
    public:
      VariableBinding(const std::string &ident, Expression *expr, const Position &pos) :
        Positional(pos), IdentifiableAndIndexable(ident), _M_expr(expr) {}

      ~VariableBinding();

      Expression *expr() const { return _M_expr; }
    };

    class TupleBinding : public Binding
    {
      std::unique_ptr<const std::list<std::unique_ptr<TupleBindingVariable>>> _M_vars;
      Expression *_M_expr;
    public:
      TupleBinding(const std::list<std::unique_ptr<TupleBindingVariable>> *vars, Expression *expr) :
        _M_vars(vars), _M_expr(expr) {}

      ~TupleBinding();

      const std::list<std::unique_ptr<TupleBindingVariable>> &vars() const { return *_M_vars; }

      Expression *expr() const { return _M_expr; }
    };

    class TupleBindingVariable : public Positional, public IdentifiableAndIndexable
    {
    public:
      TupleBindingVariable(const std::string &ident, const Position &pos) :
        Positional(pos), IdentifiableAndIndexable(ident) {}

      ~TupleBindingVariable();
    };

    class Case
    {
      std::unique_ptr<Pattern> _M_pattern;
      std::unique_ptr<Expression> _M_expr;
    public:
      Case(Pattern *pattern, Expression *expr) : _M_pattern(pattern), _M_expr(expr) {} 

      virtual ~Case();

      Pattern *pattern() const { return _M_pattern.get(); }

      Expression *expr() const { return _M_expr.get(); }
    };

    class Pattern : public Positional
    {
    protected:
      Pattern(const Position &pos) : Positional(pos) {}
    public:
      virtual ~Pattern();
    };

    class ConstructorPattern : public Pattern
    {
    protected:
      std::unique_ptr<Identifier> _M_constr_ident;
    
      ConstructorPattern(Identifier *constr_ident, const Position &pos) :
        Pattern(pos), _M_constr_ident(constr_ident) {}
    public:
      ~ConstructorPattern();

      Identifier *constr_ident() const { return _M_constr_ident.get(); }
    };

    class VariableConstructorPattern : public ConstructorPattern
    {
    public:
      VariableConstructorPattern(Identifier *constr_ident, const Position &pos) :
        ConstructorPattern(constr_ident, pos) {}

      ~VariableConstructorPattern();
    };

    class FunctionConstructorPattern : public ConstructorPattern
    {
    protected:
      FunctionConstructorPattern(Identifier *constr_ident, const Position &pos) :
        ConstructorPattern(constr_ident, pos) {}
    public:
      ~FunctionConstructorPattern();
    };

    class UnnamedFieldConstructorPattern : public FunctionConstructorPattern
    {
      std::unique_ptr<const std::list<std::unique_ptr<Pattern>>> _M_field_patterns;
    public:
      UnnamedFieldConstructorPattern(Identifier *constr_ident, const std::list<std::unique_ptr<Pattern>> *field_patterns, const Position &pos) :
        FunctionConstructorPattern(constr_ident, pos), _M_field_patterns(field_patterns) {}

      ~UnnamedFieldConstructorPattern();

      const std::list<std::unique_ptr<Pattern>> &field_patterns() const { return *_M_field_patterns; }
    };

    class NamedFieldConstructorPattern : public FunctionConstructorPattern
    {
      std::unique_ptr<const std::list<std::unique_ptr<PatternNamedFieldPair>>> _M_field_patterns;
    public:
      NamedFieldConstructorPattern(Identifier *constr_ident, const std::list<std::unique_ptr<PatternNamedFieldPair>> *field_patterns, const Position &pos) :
        FunctionConstructorPattern(constr_ident, pos), _M_field_patterns(field_patterns) {}

      ~NamedFieldConstructorPattern();

      const std::list<std::unique_ptr<PatternNamedFieldPair>> &field_patterns() const { return *_M_field_patterns; }
    };

    class CollectionPattern : public Pattern
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<Pattern>>> _M_elem_patterns;

      CollectionPattern(const std::list<std::unique_ptr<Pattern>> *elem_patterns, const Position &pos) :
        Pattern(pos), _M_elem_patterns(elem_patterns) {}
    public:
      ~CollectionPattern();

      const std::list<std::unique_ptr<Pattern>> &elem_patterns() const { return *_M_elem_patterns; }
    };

    class ListPattern : public CollectionPattern
    {
    public:
      ListPattern(const std::list<std::unique_ptr<Pattern>> *elem_patterns, const Position &pos) :
        CollectionPattern(elem_patterns, pos) {}

      ~ListPattern();
    };

    class ArrayPattern : public CollectionPattern
    {
    protected:
      ArrayPattern(const std::list<std::unique_ptr<Pattern>> *elem_patterns, const Position &pos) :
        CollectionPattern(elem_patterns, pos) {}
    public:
      ~ArrayPattern();
    };

    class NonUniqueArrayPattern : public ArrayPattern
    {
    public:
      NonUniqueArrayPattern(const std::list<std::unique_ptr<Pattern>> *elem_patterns, const Position &pos) :
        ArrayPattern(elem_patterns, pos) {}

      ~NonUniqueArrayPattern();
    };

    class UniqueArrayPattern : public ArrayPattern
    {
    public:
      UniqueArrayPattern(const std::list<std::unique_ptr<Pattern>> *elem_patterns, const Position &pos) :
        ArrayPattern(elem_patterns, pos) {}

      ~UniqueArrayPattern();
    };

    class TuplePattern : public Pattern
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<Pattern>>> _M_field_patterns;

      TuplePattern(const std::list<std::unique_ptr<Pattern>> *field_patterns, const Position &pos) :
        Pattern(pos), _M_field_patterns(field_patterns) {}
    public:
      ~TuplePattern();

      const std::list<std::unique_ptr<Pattern>> &field_patterns() const { return *_M_field_patterns; }
    };

    class NonUniqueTuplePattern : public TuplePattern
    {
    public:
      NonUniqueTuplePattern(const std::list<std::unique_ptr<Pattern>> *field_patterns, const Position &pos) :
        TuplePattern(field_patterns, pos) {}

      ~NonUniqueTuplePattern();
    };

    class UniqueTuplePattern : public TuplePattern
    {
    public:
      UniqueTuplePattern(const std::list<std::unique_ptr<Pattern>> *field_patterns, const Position &pos) :
        TuplePattern(field_patterns, pos) {}

      ~UniqueTuplePattern();
    };

    class LiteralPattern : public Pattern
    {
      std::unique_ptr<SimpleLiteralValue> _M_literal_value;
    public:
      LiteralPattern(SimpleLiteralValue *literal_value, const Position &pos) : Pattern(pos), _M_literal_value(literal_value) {}

      ~LiteralPattern();

      SimpleLiteralValue *literal_value() const { return _M_literal_value.get(); }
    };

    class VariablePattern : public Pattern, public IdentifiableAndIndexable
    {
    public:
      VariablePattern(const std::string &ident, const Position &pos) :
        Pattern(pos), IdentifiableAndIndexable(ident) {}

      ~VariablePattern();
    };

    class AsPattern : public Pattern, public IdentifiableAndIndexable
    {
      std::unique_ptr<Pattern> _M_pattern;
    public:
      AsPattern(const std::string &ident, Pattern *pattern, const Position &pos) :
        Pattern(pos), IdentifiableAndIndexable(ident), _M_pattern(pattern) {}

      ~AsPattern();

      Pattern *pattern() const { return _M_pattern.get(); }
    };

    class WildcardPattern : public Pattern
    {
    public:
      WildcardPattern(const Position &pos) : Pattern(pos) {}

      ~WildcardPattern();
    };

    class TypedPattern : public Pattern
    {
      std::unique_ptr<Pattern> _M_pattern;
      std::unique_ptr<TypeExpression> _M_type_expr;
    public:
      TypedPattern(Pattern *pattern, TypeExpression *type_expr, const Position &pos) :
        Pattern(pos), _M_pattern(pattern), _M_type_expr(type_expr) {} 

      ~TypedPattern();

      Pattern *pattern() const { return _M_pattern.get(); }

      TypeExpression *type_expr() const { return _M_type_expr.get(); }
    };

    class PatternNamedFieldPair : public Positional, public IdentifiableAndIndexable
    {
      std::unique_ptr<Pattern> _M_pattern;
    public:
      PatternNamedFieldPair(const std::string &ident, Pattern *pattern, const Position &pos) :
        Positional(pos), IdentifiableAndIndexable(ident), _M_pattern(pattern) {}

      ~PatternNamedFieldPair();

      Pattern *pattern() const { return _M_pattern.get(); }
    };

    class LiteralValue
    {
    protected:
      LiteralValue() {}
    public:
      virtual ~LiteralValue();
    };

    class NonUniqueLiteralValue : public LiteralValue
    {
    protected:
      NonUniqueLiteralValue() {}
    public:
      ~NonUniqueLiteralValue();
    };

    class SimpleLiteralValue : public NonUniqueLiteralValue
    {
    protected:
      SimpleLiteralValue() {}
    public:
      ~SimpleLiteralValue();
    };

    class CharValue : public SimpleLiteralValue
    {
      char _M_c;
    public:
      CharValue(char c) : _M_c(c) {}

      ~CharValue();

      char c() const { return _M_c; }
    };

    class WideCharValue : public SimpleLiteralValue
    {
      wchar_t _M_c;
    public:
      WideCharValue(wchar_t c) : _M_c(c) {}

      ~WideCharValue();

      wchar_t c() const { return _M_c; }
    };

    class IntValue : public SimpleLiteralValue
    {
      IntType _M_int_type;
      std::int64_t _M_i;
    public:
      IntValue(IntType int_type, std::int64_t i) : _M_int_type(int_type), _M_i(i) {}

      ~IntValue();

      IntType int_type() const { return _M_int_type; }

      std::int64_t i() const { return _M_i; }
    };

    class FloatValue : public SimpleLiteralValue
    {
      FloatType _M_float_type;
      double _M_f;
    public:
      FloatValue(FloatType float_type, double f) : _M_float_type(float_type), _M_f(f) {}

      ~FloatValue();

      FloatType float_type() const { return _M_float_type; }

      double f() const { return _M_f; }
    };

    class StringValue : public SimpleLiteralValue
    {
      std::string _M_string;
    public:
      StringValue(const std::string &str) : _M_string(str) {}

      ~StringValue();

      const std::string &string() const { return _M_string; }
    };

    class WideStringValue : public SimpleLiteralValue
    {
      std::wstring _M_string;
    public:
      WideStringValue(const std::wstring &str) : _M_string(str) {}

      ~WideStringValue();

      const std::wstring &string() const { return _M_string; }
    };

    class LambdaValue : public LiteralValue, public Inlinable
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<Argument>>> _M_args;
      std::unique_ptr<TypeExpression> _M_result_type_expr;
      std::unique_ptr<Expression> _M_body;

      LambdaValue(InlineModifier inline_modifier, const std::list<std::unique_ptr<Argument>> *args, Expression *body) :
        Inlinable(inline_modifier), _M_args(args), _M_result_type_expr(nullptr), _M_body(body) {}

      LambdaValue(InlineModifier inline_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr, Expression *body) :
        Inlinable(inline_modifier), _M_args(args), _M_result_type_expr(result_type_expr), _M_body(body) {}
    public:
      ~LambdaValue();

      const std::list<std::unique_ptr<Argument>> &args() const { return *_M_args; }

      TypeExpression *result_type_expr() const { return _M_result_type_expr.get(); }

      Expression *body() const { return _M_body.get(); }
    };

    class NonUniqueLambdaValue : public virtual LambdaValue, public virtual NonUniqueLiteralValue
    {
      FunctionModifier _M_fun_modifier;
    public:
      NonUniqueLambdaValue(InlineModifier inline_modifier, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, Expression *body) :
        LambdaValue(inline_modifier, args, body), _M_fun_modifier(fun_modifier) {}

      NonUniqueLambdaValue(InlineModifier inline_modifier, FunctionModifier fun_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr, Expression *body) :
        LambdaValue(inline_modifier, args, result_type_expr, body), _M_fun_modifier(fun_modifier) {}

      ~NonUniqueLambdaValue();

      FunctionModifier fun_modifier() const { return _M_fun_modifier; }
    };

    class UniqueLambdaValue : public LambdaValue
    {
    public:
      UniqueLambdaValue(InlineModifier inline_modifier, const std::list<std::unique_ptr<Argument>> *args, Expression *body) :
        LambdaValue(inline_modifier, args, body) {}

      UniqueLambdaValue(InlineModifier inline_modifier, const std::list<std::unique_ptr<Argument>> *args, TypeExpression *result_type_expr, Expression *body) :
        LambdaValue(inline_modifier, args, result_type_expr, body) {}

      ~UniqueLambdaValue();
    };

    class Value : public Positional
    {
    protected:
      Value(const Position &pos) : Positional(pos) {}
    public:
      virtual ~Value();
    };

    class VariableLiteralValue : public Value
    {
      std::unique_ptr<NonUniqueLiteralValue> _M_literal_value;
    public:
      VariableLiteralValue(NonUniqueLiteralValue *literal_value, const Position &pos) :
        Value(pos), _M_literal_value(literal_value) {}

      ~VariableLiteralValue();

      NonUniqueLiteralValue *literal_value() const { return _M_literal_value.get(); }
    };

    class CollectionValue : public Value
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<Value>>> _M_elems;

      CollectionValue(const std::list<std::unique_ptr<Value>> *elems, const Position &pos) :
        Value(pos), _M_elems(elems) {} 
    public:
      ~CollectionValue();

      const std::list<std::unique_ptr<Value>> &elems() const { return *_M_elems; }
    };

    class ListValue : public CollectionValue
    {
    public:
      ListValue(const std::list<std::unique_ptr<Value>> *elems, const Position &pos) :
        CollectionValue(elems, pos) {}

      ~ListValue();
    };

    class ArrayValue : public CollectionValue
    {
    public:
      ArrayValue(const std::list<std::unique_ptr<Value>> *elems, const Position &pos) :
        CollectionValue(elems, pos) {}

      ~ArrayValue();
    };

    class TupleValue : public Value
    {
      std::unique_ptr<const std::list<std::unique_ptr<Value>>> _M_fields;
    public:
      TupleValue(const std::list<std::unique_ptr<Value>> *fields, const Position &pos) :
        Value(pos), _M_fields(fields) {}

      ~TupleValue();

      const std::list<std::unique_ptr<Value>> &fields() const { return *_M_fields; }
    };

    class ConstructorValue : public Value
    {
    protected:
      std::unique_ptr<Identifier> _M_constr_ident;
    
      ConstructorValue(Identifier *constr_ident, const Position &pos) :
        Value(pos), _M_constr_ident(constr_ident) {}
    public:
      ~ConstructorValue();

      Identifier *constr_ident() const { return _M_constr_ident.get(); }
    };

    class VariableConstructorValue : public ConstructorValue
    {
    public:
      VariableConstructorValue(Identifier *constr_ident, const Position &pos) :
        ConstructorValue(constr_ident, pos) {}

      ~VariableConstructorValue();
    };

    class FunctionConstructorValue : public ConstructorValue
    {
    protected:
      FunctionConstructorValue(Identifier *constr_ident, const Position &pos) :
        ConstructorValue(constr_ident, pos) {}
    public:
      ~FunctionConstructorValue();
    };

    class UnnamedFieldConstructorValue : public FunctionConstructorValue
    {
      std::unique_ptr<const std::list<std::unique_ptr<Value>>> _M_fields;
    public:
      UnnamedFieldConstructorValue(Identifier *constr_ident, const std::list<std::unique_ptr<Value>> *fields, const Position &pos) :
        FunctionConstructorValue(constr_ident, pos), _M_fields(fields) {}

      ~UnnamedFieldConstructorValue();

      const std::list<std::unique_ptr<Value>> &fields() const { return *_M_fields; }
    };

    class NamedFieldConstructorValue : public FunctionConstructorValue
    {
      std::unique_ptr<const std::list<std::unique_ptr<ValueNamedFieldPair>>> _M_fields;
    public:
      NamedFieldConstructorValue(Identifier *constr_ident, const std::list<std::unique_ptr<ValueNamedFieldPair>> *fields, const Position &pos) :
        FunctionConstructorValue(constr_ident, pos), _M_fields(fields) {}

      ~NamedFieldConstructorValue();

      const std::list<std::unique_ptr<ValueNamedFieldPair>> &fields() const { return *_M_fields; }
    };

    class TypedValue : public Value
    {
      std::unique_ptr<Value> _M_value;
      std::unique_ptr<TypeExpression> _M_type_expr;
    public:
      TypedValue(Value *value, TypeExpression *type_expr, const Position &pos) :
        Value(pos), _M_value(value), _M_type_expr(type_expr) {}

      ~TypedValue();

      Value *value() const { return _M_value.get(); }

      TypeExpression *type_expr() const { return _M_type_expr.get(); }
    };

    class ValueNamedFieldPair : public Positional, public IdentifiableAndIndexable
    {
      std::unique_ptr<Value> _M_value;
    public:
      ValueNamedFieldPair(const std::string &ident, Value *value, const Position &pos) :
        Positional(pos), IdentifiableAndIndexable(ident), _M_value(value) {}

      ~ValueNamedFieldPair();

      Value *value() const { return _M_value.get(); }
    };

    class TypeVariable
    {
    protected:
      TypeVariable() {}
    public:
      virtual ~TypeVariable();
    };

    class DefinableTypeVariable : public TypeVariable
    {
    protected:
      DefinableTypeVariable() {}
    public:
      ~DefinableTypeVariable();
    };

    class TypeSynonymVariable : public DefinableTypeVariable
    {
      std::unique_ptr<TypeExpression> _M_expr;
    public:
      TypeSynonymVariable(TypeExpression *expr) : _M_expr(expr) {}

      ~TypeSynonymVariable();

      TypeExpression *expr() const { return _M_expr.get(); }
    };

    class DatatypeVariable : public DefinableTypeVariable
    {
      std::unique_ptr<Datatype> _M_datatype;
    public:
      DatatypeVariable(Datatype *datatype) : _M_datatype(datatype) {}

      ~DatatypeVariable();

      Datatype *datatype() const { return _M_datatype.get(); }
    };

    class BuiltinTypeVariable : public TypeVariable
    {
      BuiltinType _M_builtin_type;
    public:
      BuiltinTypeVariable(BuiltinType builtin_type) : _M_builtin_type(builtin_type) {}

      ~BuiltinTypeVariable();

      BuiltinType builtin_type() const { return _M_builtin_type; }
    };

    class TypeFunction
    {
    protected:
      std::size_t _M_arg_count;

      TypeFunction(std::size_t arg_count) : _M_arg_count(arg_count) {}
    public:
      virtual ~TypeFunction();

      std::size_t arg_count() const { return _M_arg_count; }
    };

    class DefinableTypeFunction : public TypeFunction
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<TypeParameter>>> _M_inst_type_params;
      std::unique_ptr<const std::list<std::unique_ptr<TypeArgument>>> _M_args;
    public:
      DefinableTypeFunction(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, const std::list<std::unique_ptr<TypeArgument>> *args) :
        TypeFunction(args->size()), _M_inst_type_params(inst_type_params), _M_args(args) {}

      ~DefinableTypeFunction();

      const std::list<std::unique_ptr<TypeParameter>> &inst_type_params() const { return *_M_inst_type_params; }

      const std::list<std::unique_ptr<TypeArgument>> &args() const { return *_M_args; }
    };

    class TypeSynonymFunction : public DefinableTypeFunction
    {
      std::unique_ptr<TypeExpression> _M_body;
    public:
      TypeSynonymFunction(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, const std::list<std::unique_ptr<TypeArgument>> *args, TypeExpression *body) :
        DefinableTypeFunction(inst_type_params, args), _M_body(body) {}

      TypeSynonymFunction(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, const std::list<std::unique_ptr<TypeArgument>> *args) :
        DefinableTypeFunction(inst_type_params, args), _M_body(nullptr) {}
        
      ~TypeSynonymFunction();

      TypeExpression *body() const { return _M_body.get(); }
    };

    class DatatypeFunction : public DefinableTypeFunction
    {
      std::unique_ptr<Datatype> _M_datatype;
    public:
      DatatypeFunction(const std::list<std::unique_ptr<TypeParameter>> *inst_type_params, const std::list<std::unique_ptr<TypeArgument>> *args, Datatype *datatype) :
        DefinableTypeFunction(inst_type_params, args), _M_datatype(datatype) {}

      ~DatatypeFunction();

      Datatype *datatype() const { return _M_datatype.get(); }
    };

    class BuiltinTypeFunction : public TypeFunction
    {
      BuiltinTypeTemplate _M_builtin_type_temlate;
    public:
      BuiltinTypeFunction(std::size_t arg_count, BuiltinTypeTemplate builtin_type_template) :
        TypeFunction(arg_count), _M_builtin_type_temlate(builtin_type_template) {}

      ~BuiltinTypeFunction();

      BuiltinTypeTemplate builtin_type_template() const { return _M_builtin_type_temlate; }
    };

    class TypeFunctionInstance
    {
    protected:
      bool _M_is_template;
      std::unique_ptr<const std::list<std::unique_ptr<TypeExpression>>> _M_args;

      TypeFunctionInstance(bool is_template, const std::list<std::unique_ptr<TypeExpression>> *args) :
        _M_is_template(is_template), _M_args(args) {}
    public:
      virtual ~TypeFunctionInstance();
      
      bool is_template() const { return _M_is_template; }

      const std::list<std::unique_ptr<TypeExpression>> &args() const { return *_M_args; }
    };

    class TypeSynonymFunctionInstance : public TypeFunctionInstance
    {
      std::unique_ptr<TypeExpression> _M_body;
    public:
      TypeSynonymFunctionInstance(bool is_template, const std::list<std::unique_ptr<TypeExpression>> *args, TypeExpression *body) :
        TypeFunctionInstance(is_template, args), _M_body(body) {}

      ~TypeSynonymFunctionInstance();

      TypeExpression *body() const { return _M_body.get(); }
    };

    class DatatypeFunctionInstance : public TypeFunctionInstance
    {
      std::unique_ptr<Datatype> _M_datatype;
    public:
      DatatypeFunctionInstance(bool is_template, const std::list<std::unique_ptr<TypeExpression>> *args, Datatype *datatype) :
        TypeFunctionInstance(is_template, args), _M_datatype(datatype) {}

      ~DatatypeFunctionInstance();

      Datatype *datatype() const { return _M_datatype.get(); }
    };

    class Datatype
    {
    protected:
      Datatype() {}
    public:
      virtual ~Datatype();
    };

    class NonUniqueDatatype : public Datatype
    {
      std::unique_ptr<const std::list<std::shared_ptr<Constructor>>> _M_constrs;
    public:
      NonUniqueDatatype(const std::list<std::shared_ptr<Constructor>> *constrs) :
        _M_constrs(constrs) {}

      ~NonUniqueDatatype();

      const std::list<std::shared_ptr<Constructor>> &constrs() const { return *_M_constrs; }
    };

    class UniqueDatatype : public Datatype
    {
      std::unique_ptr<const std::list<std::shared_ptr<FunctionConstructor>>> _M_constrs;
    public:
      UniqueDatatype(const std::list<std::shared_ptr<FunctionConstructor>> *constrs) :
        _M_constrs(constrs) {}

      ~UniqueDatatype();

      const std::list<std::shared_ptr<FunctionConstructor>> &constrs() const { return *_M_constrs; }
    };

    class Constructor : public Accessible, public Positional
    {
    protected:
      std::string _M_ident;
      bool _M_has_datatype_fun;
      KeyIdentifier _M_datatype_key_ident;
      DatatypeFunctionInstance *_M_datatype_fun_inst;

      Constructor(AccessModifier access_modifier, const std::string &ident, const Position &pos) :
        Accessible(access_modifier), Positional(pos), _M_ident(ident), _M_has_datatype_fun(false), _M_datatype_fun_inst(nullptr) {}
    public:
      ~Constructor();

      const std::string &ident() const { return _M_ident; }
      
      bool has_datatype_fun() const { return _M_has_datatype_fun; }

      void set_datatype_fun_flag(bool has_datatype_fun) { _M_has_datatype_fun = has_datatype_fun; }

      KeyIdentifier datatype_key_ident() const { return _M_datatype_key_ident; }
      
      void set_datatype_key_ident(KeyIdentifier datatype_key_ident) { _M_datatype_key_ident = datatype_key_ident; }
      
      DatatypeFunctionInstance *datatype_fun_inst() const { return _M_datatype_fun_inst; }

      void set_datatype_fun_inst(DatatypeFunctionInstance *datatype_fun_inst) { _M_datatype_fun_inst = datatype_fun_inst; }
    };

    class VariableConstructor : public Constructor
    {
    public:
      VariableConstructor(AccessModifier access_modifier, const std::string &ident, const Position &pos) :
        Constructor(access_modifier, ident, pos) {}

      ~VariableConstructor();
    };

    class FunctionConstructor : public Constructor, public Inlinable
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<Annotation>>> _M_annotations;

      FunctionConstructor(const std::list<std::unique_ptr<Annotation>> *annotations, AccessModifier access_modifier, InlineModifier inline_modifier, const std::string &ident, const Position &pos) :
        Constructor(access_modifier, ident, pos), Inlinable(inline_modifier), _M_annotations(annotations) {}
    public:
      ~FunctionConstructor();

      const std::list<std::unique_ptr<Annotation>> &annotations() const { return *_M_annotations; }

      virtual std::size_t field_count() = 0;
    };

    class UnnamedFieldConstructor : public FunctionConstructor
    {
      std::unique_ptr<const std::list<std::unique_ptr<TypeExpression>>> _M_field_types;
    public:
      UnnamedFieldConstructor(const std::list<std::unique_ptr<Annotation>> *annotations, AccessModifier access_modifier, InlineModifier inline_modifier, const std::string &ident, const std::list<std::unique_ptr<TypeExpression>> *field_types, const Position &pos) :
        FunctionConstructor(annotations, access_modifier, inline_modifier, ident, pos), _M_field_types(field_types) {}

      ~UnnamedFieldConstructor();

      std::size_t field_count();

      const std::list<std::unique_ptr<TypeExpression>> &field_types() const { return *_M_field_types; }
    };

    class NamedFieldConstructor : public FunctionConstructor
    {
      std::unique_ptr<const std::list<std::unique_ptr<TypeNamedFieldPair>>> _M_field_types;
      std::unordered_map<std::string, std::size_t> _M_field_indices;
    public:
      NamedFieldConstructor(const std::list<std::unique_ptr<Annotation>> *annotations, AccessModifier access_modifier, InlineModifier inline_modifier, const std::string &ident, const std::list<std::unique_ptr<TypeNamedFieldPair>> *field_types, const Position &pos) :
        FunctionConstructor(annotations, access_modifier, inline_modifier, ident, pos), _M_field_types(field_types) {}

      ~NamedFieldConstructor();

      std::size_t field_count();

      const std::list<std::unique_ptr<TypeNamedFieldPair>> &field_types() const { return *_M_field_types; }

      const std::unordered_map<std::string, std::size_t> &field_indices() const { return _M_field_indices; }

      void set_field_indices(const std::unordered_map<std::string, std::size_t> &field_indices) { _M_field_indices = field_indices; }
    };

    class TypeArgument : public Positional, public IdentifiableAndIndexable
    {
    public:
      TypeArgument(const std::string &ident, const Position &pos) :
        Positional(pos), IdentifiableAndIndexable(ident) {}

      ~TypeArgument();
    };

    class TypeParameter : public Positional, public IdentifiableAndIndexable
    {
    public:
      TypeParameter(const std::string &ident, const Position &pos) :
        Positional(pos), IdentifiableAndIndexable(ident) {}

      ~TypeParameter();
    };

    class TypeNamedFieldPair : public Positional, public Identifiable
    {
      std::unique_ptr<TypeExpression> _M_type_expr;
    public:
      TypeNamedFieldPair(const std::string &ident, TypeExpression *type_expr, const Position &pos) :
        Positional(pos), Identifiable(ident), _M_type_expr(type_expr) {}

      ~TypeNamedFieldPair();

      TypeExpression *type_expr() const { return _M_type_expr.get(); }
    };

    class TypeExpression : public Positional
    {
    protected:
      TypeExpression(const Position &pos) : Positional(pos) {}
    public:
      ~TypeExpression();
    };
    
    class With : public TypeExpression
    {
      std::unique_ptr<TypeExpression> _M_type1;
      std::unique_ptr<TypeExpression> _M_type2;
    public:
      With(TypeExpression *type1, TypeExpression *type2, const Position &pos) :
        TypeExpression(pos), _M_type1(type1), _M_type2(type2) {}
        
      ~With();
        
      TypeExpression *type1() const { return _M_type1.get(); }

      TypeExpression *type2() const { return _M_type2.get(); }
    };

    class TypeVariableExpression : public TypeExpression
    {
      std::unique_ptr<Identifier> _M_ident;
    public:
      TypeVariableExpression(Identifier *ident, const Position &pos) :
        TypeExpression(pos), _M_ident(ident) {}

      ~TypeVariableExpression();

      Identifier *ident() const { return _M_ident.get(); }
    };

    class TypeParameterExpression : public TypeExpression, public IdentifiableAndIndexable
    {
    public:
      TypeParameterExpression(const std::string &ident, const Position &pos) :
        TypeExpression(pos), IdentifiableAndIndexable(ident) {}

      ~TypeParameterExpression();
    };
    
    class TupleType : public TypeExpression
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<TypeExpression>>> _M_field_types;

      TupleType(const std::list<std::unique_ptr<TypeExpression>> *field_types, const Position &pos) :
        TypeExpression(pos), _M_field_types(field_types) {}
    public:
      ~TupleType();

      const std::list<std::unique_ptr<TypeExpression>> &field_types() const { return *_M_field_types; }
    };

    class NonUniqueTupleType : public TupleType
    {
    public:
      NonUniqueTupleType(const std::list<std::unique_ptr<TypeExpression>> *field_types, const Position &pos) :
        TupleType(field_types, pos) {}

      ~NonUniqueTupleType();
    };

    class UniqueTupleType : public TupleType
    {
    public:
      UniqueTupleType(const std::list<std::unique_ptr<TypeExpression>> *field_types, const Position &pos) :
        TupleType(field_types, pos) {}

      ~UniqueTupleType();
    };

    class FunctionType : public TypeExpression
    {
    protected:
      std::unique_ptr<const std::list<std::unique_ptr<TypeExpression>>> _M_arg_types;
      std::unique_ptr<TypeExpression> _M_result_type;

      FunctionType(const std::list<std::unique_ptr<TypeExpression>> *arg_types, TypeExpression *result_type, const Position &pos) :
        TypeExpression(pos), _M_arg_types(arg_types), _M_result_type(result_type) {}
    public:
      ~FunctionType();

      const std::list<std::unique_ptr<TypeExpression>> &arg_types() const { return *_M_arg_types; }

      TypeExpression *result_type() const { return _M_result_type.get(); }
    };

    class NonUniqueFunctionType : public FunctionType
    {
      FunctionModifier _M_fun_modifier;
    public:
      NonUniqueFunctionType(const std::list<std::unique_ptr<TypeExpression>> *arg_types, FunctionModifier fun_modifier, TypeExpression *result_type, const Position &pos) :
        FunctionType(arg_types, result_type, pos), _M_fun_modifier(fun_modifier) {}

      ~NonUniqueFunctionType();

      FunctionModifier fun_modifier() const { return _M_fun_modifier; }
    };

    class UniqueFunctionType : public FunctionType
    {
    public:
      UniqueFunctionType(const std::list<std::unique_ptr<TypeExpression>> *arg_types, TypeExpression *result_type, const Position &pos) :
        FunctionType(arg_types, result_type, pos) {}

      ~UniqueFunctionType();
    };

    class TypeApplication : public TypeExpression
    {
      std::unique_ptr<Identifier> _M_fun_ident;
      std::unique_ptr<const std::list<std::unique_ptr<TypeExpression>>> _M_args;
    public:
      TypeApplication(Identifier *fun_ident, const std::list<std::unique_ptr<TypeExpression>> *args, const Position &pos) :
        TypeExpression(pos), _M_fun_ident(fun_ident), _M_args(args) {}

      ~TypeApplication();

      Identifier *fun_ident() const { return _M_fun_ident.get(); }

      const std::list<std::unique_ptr<TypeExpression>> &args() const { return *_M_args; }
    };
  }
}

#endif
