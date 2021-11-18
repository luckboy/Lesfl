/****************************************************************************
 *   Copyright (C) 2016-2018, 2021 Łukasz Szpakowski.                       *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#include <algorithm>
#include <iterator>
#include <set>
#include <lesfl/frontend.hpp>
#include "util.hpp"

using namespace std;
using namespace lesfl::util;

namespace lesfl
{
  namespace frontend
  {
    namespace
    {
      //
      // Structures.
      //

      struct LocalVariablePair
      {
        size_t ref_count;
        list<size_t> indices;
        
        LocalVariablePair() : ref_count(0) {}
      };

      struct ResolverContext
      {
        Tree &tree;
        AbsoluteIdentifier current_module_ident;
        AbsoluteIdentifier predef_module_ident;
        list<vector<AbsoluteIdentifier>> imported_module_ident_stack;
        list<vector<string>> local_var_ident_stack;
        size_t local_var_count;
        unordered_map<string, LocalVariablePair> local_var_pairs;
        unordered_set<string> top_local_var_idents;
        vector<size_t> closure_limit_stack;
        unordered_map<string, size_t> type_param_indices;
        size_t type_param_count;
        bool template_flag;

        ResolverContext(Tree &tree) :
          tree(tree), predef_module_ident("predef"), local_var_count(0), type_param_count(0) {}
      };
    }

    //
    // Static inline functions and static functions.
    //

    static inline void push_imported_module_vector(ResolverContext &context)
    { context.imported_module_ident_stack.push_back(vector<AbsoluteIdentifier>()); }
    
    static void push_imported_module(ResolverContext &context, const AbsoluteIdentifier &ident)
    { context.imported_module_ident_stack.back().push_back(ident); }

    static inline void pop_imported_modules(ResolverContext &context)
    { context.imported_module_ident_stack.pop_back(); }

    static inline void clear_imported_module_ident_stack(ResolverContext &context)
    { context.imported_module_ident_stack.clear(); }
    
    static bool set_local_var_index(ResolverContext &context, RelativeIdentifier &ident)
    {
      auto iter = context.local_var_pairs.find(ident.idents().back());
      if(iter == context.local_var_pairs.end()) return false;
      size_t closure_limit = 0;
      if(!context.closure_limit_stack.empty()) closure_limit = context.closure_limit_stack.back();
      size_t i = iter->second.indices.back();
      if(i >= closure_limit) {
        ident.set_index(i);
        return true;
      } else
        return false;
    }

    static inline void push_local_var_vector(ResolverContext &context)
    { context.local_var_ident_stack.push_back(vector<string>()); }

    static bool push_local_var(ResolverContext &context, IdentifiableAndIndexable &identifiable)
    {
      if(!context.top_local_var_idents.insert(identifiable.ident()).second) return false;
      auto pair = context.local_var_pairs.insert(make_pair(identifiable.ident(), LocalVariablePair()));
      pair.first->second.ref_count++;
      pair.first->second.indices.push_back(context.local_var_count);
      context.local_var_ident_stack.back().push_back(pair.first->first);
      identifiable.set_index(context.local_var_count);
      context.local_var_count++;
      return true;
    }

    static inline void clear_top_local_var_idents(ResolverContext &context)
    { context.top_local_var_idents.clear(); }

    static bool pop_local_vars(ResolverContext &context)
    {
      if(!context.local_var_ident_stack.empty()) {
        for(const string &ident : context.local_var_ident_stack.back()) {
          auto iter = context.local_var_pairs.find(ident);
          if(iter != context.local_var_pairs.end()) {
            LocalVariablePair &pair = iter->second;
            if(pair.ref_count != 0) {
              pair.ref_count--;
              pair.indices.pop_back();
            }
            if(pair.ref_count == 0) context.local_var_pairs.erase(ident);
            context.local_var_count--;
          }
        }
        context.local_var_ident_stack.pop_back();
        return true;
      } else
        return false;
    }

    static bool check_and_clear_local_var_ident_stack(ResolverContext &context, const Position &pos, list<Error> &errors)
    {
      bool is_success = true;
      if(!context.local_var_ident_stack.empty()) {
        errors.push_back(Error(pos, "internal error: local_var_ident_stack isn't empty"));
        is_success = false;
      }
      context.local_var_ident_stack.clear();
      return is_success;
    }

    static void push_closure_limit(ResolverContext &context, size_t local_var_count)
    { context.closure_limit_stack.push_back(local_var_count); }
    
    static bool pop_closure_limit(ResolverContext &context)
    {
      if(!context.closure_limit_stack.empty()) {
        context.closure_limit_stack.pop_back();
        return true;
      } else
        return false;
    }
    
    static bool check_and_clear_closure_limit_stack(ResolverContext &context, const Position &pos, list<Error> &errors)
    {
      bool is_success = true;
      if(!context.closure_limit_stack.empty()) {
        errors.push_back(Error(pos, "internal error: closure_limit_stack isn't empty"));
        is_success = false;
      }
      context.closure_limit_stack.clear();
      return is_success;
    }
    
    static bool set_type_param_index(ResolverContext &context, IdentifiableAndIndexable &identifiable)
    {
      auto iter = context.type_param_indices.find(identifiable.ident());
      if(iter == context.type_param_indices.end()) return false;
      identifiable.set_index(iter->second);
      return true;
    }

    static bool add_type_param(ResolverContext &context, IdentifiableAndIndexable &identifiable)
    {
      if(!context.type_param_indices.insert(make_pair(identifiable.ident(), context.type_param_count)).second) return false;
      identifiable.set_index(context.type_param_count);
      context.type_param_count++;
      return true;
    }

    static void clear_type_params(ResolverContext &context)
    { context.type_param_indices.clear(); }

    static bool check_and_clear_type_param_indices(ResolverContext &context, const Position &pos, list<Error> &errors)
    {
      bool is_success = true;
      if(!context.type_param_indices.empty()) {
        errors.push_back(Error(pos, "internal error: type_param_indices isn't empty"));
        is_success = false;
      }
      context.type_param_indices.clear();
      return is_success;
    }

    static bool add_ident_or_get_key_ident(ResolverContext &context, AbsoluteIdentifier *ident, KeyIdentifier &key_ident, bool &is_added, const Position &pos, list<Error> &errors)
    {
      if(!context.tree.ident_table()->add_ident_or_get_key_ident(ident, key_ident, is_added)) {
        errors.push_back(Error(pos, "internal error: can't add identifier to identifier table or get key identifier from identifier table"));
        return false;
      }
      return true;
    }

    static bool get_module_abs_ident(ResolverContext &context, const Identifier &ident, AbsoluteIdentifier &abs_ident, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(&ident,
      [&pos, &errors](const Identifier *ident) -> bool {
        errors.push_back(Error(pos, "internal error: unknown identifier class"));
        return false;
      },
      [&](const AbsoluteIdentifier *ident) -> bool {
        abs_ident = *ident;
        return true;
      },
      [&](const RelativeIdentifier *ident) -> bool {
        abs_ident = AbsoluteIdentifier(context.current_module_ident, *ident);
        return true;
      });
    }

    static bool add_root_module(ResolverContext &context, list<Error> &errors)
    {
      unique_ptr<AbsoluteIdentifier> abs_ident(new AbsoluteIdentifier());
      bool is_added_abs_ident;
      KeyIdentifier key_ident;
      if(!add_ident_or_get_key_ident(context, abs_ident.get(), key_ident, is_added_abs_ident, Position(Source(), 0, 0), errors)) return false;
      if(is_added_abs_ident) abs_ident.release();
      context.tree.add_module(key_ident);
      return true;
    }

    static bool add_constr(ResolverContext &context, const shared_ptr<Constructor> &constr, AccessModifier access_modifier, bool has_datatype_fun, KeyIdentifier *datatype_key_ident, DatatypeFunctionInstance *datatype_fun_inst, list<Error> &errors, const string *datatype_ident = nullptr)
    {
      unique_ptr<AbsoluteIdentifier> abs_ident(new AbsoluteIdentifier(context.current_module_ident, constr->ident()));
      bool is_added_abs_ident;
      KeyIdentifier key_ident;
      if(!add_ident_or_get_key_ident(context, abs_ident.get(), key_ident, is_added_abs_ident, constr->pos(), errors)) return false;
      if(is_added_abs_ident) abs_ident.release();
      bool is_success = true;
      constr->set_datatype_fun_flag(has_datatype_fun);
      if(datatype_key_ident != nullptr) constr->set_datatype_key_ident(*datatype_key_ident);
      constr->set_datatype_fun_inst(datatype_fun_inst);
      shared_ptr<Variable> constr_var(new ConstructorVariable(constr));
      if(!context.tree.add_var(key_ident, access_modifier, constr_var, constr->access_modifier(), datatype_ident)) {
        if(context.template_flag)
          errors.push_back(Error(constr->pos(), "constructor template " + abs_ident->to_string() + " is already defined"));
        else
          errors.push_back(Error(constr->pos(), "constructor " + abs_ident->to_string() + " is already defined"));
        is_success = false;
      }
      context.tree.uncompiled_var_key_idents().push_back(key_ident);
      return is_success;
    }

    static bool add_constrs_from_datatype(ResolverContext &context, Datatype *datatype, AccessModifier access_modifier, bool has_datatype_fun, KeyIdentifier *datatype_key_ident, DatatypeFunctionInstance *datatype_fun_inst, const Position &pos, list<Error> &errors, const string *datatype_ident = nullptr)
    {
      return dynamic_match(datatype,
      [&pos, &errors](Datatype *datatype) -> bool {
        errors.push_back(Error(pos, "interal error: unknown datatype class"));
        return false;
      },
      [&](NonUniqueDatatype *datatype) -> bool {
        bool is_success = true;
        for(auto &constr : datatype->constrs()) {
          is_success &= add_constr(context, constr, access_modifier, has_datatype_fun, datatype_key_ident, datatype_fun_inst, errors, datatype_ident);
        }
        return is_success;
      },
      [&](UniqueDatatype *datatype) -> bool {
        bool is_success = true;
        for(auto &constr : datatype->constrs()) {
          is_success &= add_constr(context, constr, access_modifier, has_datatype_fun, datatype_key_ident, datatype_fun_inst, errors, datatype_ident);
        }
        return is_success;
      });
    }

    static bool add_constrs_from_type_var(ResolverContext &context, const shared_ptr<DefinableTypeVariable> &var, AccessModifier access_modifier, KeyIdentifier datatype_key_ident, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(var.get(),
      [&pos, &errors](TypeVariable *var) -> bool {
        errors.push_back(Error(pos, "interal error: unknown type variable class"));
        return false;
      },
      [](TypeSynonymVariable *var) -> bool {
        return true;
      },
      [&](DatatypeVariable *var) -> bool {
        return add_constrs_from_datatype(context, var->datatype(), access_modifier, false, &datatype_key_ident, nullptr, pos, errors);
      },
      [](BuiltinTypeVariable *var) -> bool {
        return true;
      });
    }

    static bool add_constrs_from_type_fun(ResolverContext &context, const shared_ptr<DefinableTypeFunction> &fun, AccessModifier access_modifier, KeyIdentifier datatype_key_ident, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(fun.get(),
      [&pos, &errors](TypeFunction *fun) -> bool {
        errors.push_back(Error(pos, "interal error: unknown type function class"));
        return false;
      },
      [](TypeSynonymFunction *fun) -> bool {
        return true;
      },
      [&](DatatypeFunction *fun) -> bool {
        return add_constrs_from_datatype(context, fun->datatype(), access_modifier, true, &datatype_key_ident, nullptr, pos, errors);
      },
      [](BuiltinTypeFunction *fun) -> bool {
        return true;
      });
    }

    static bool add_constrs_from_type_fun_inst(ResolverContext &context, const shared_ptr<TypeFunctionInstance> &inst, const string *datatype_ident, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(inst.get(),
      [&pos, &errors](TypeFunctionInstance *fun) -> bool {
        errors.push_back(Error(pos, "interal error: unknown type function instance class"));
        return false;
      },
      [](TypeSynonymFunctionInstance *inst) -> bool {
        return true;
      },
      [&](DatatypeFunctionInstance *inst) -> bool {
        context.template_flag = inst->is_template();
        bool is_success = add_constrs_from_datatype(context, inst->datatype(), AccessModifier::NONE, true, nullptr, inst, pos, errors, datatype_ident);
        context.template_flag = false;
        return is_success;
      });
    }

    static bool add_defs(ResolverContext &context, const list<unique_ptr<Definition>> &defs, list<Error> &errors)
    {
      bool is_success = true;
      for(auto &def : defs) {
        is_success &= dynamic_match(def.get(), 
        [&errors](Definition *def) -> bool {
          errors.push_back(Error(def->pos(), "interal error: unknown definition class"));
          return false;
        },
        [](Import *import) -> bool {
          return true;
        },
        [&](ModuleDefinition *module_def) -> bool {
          AbsoluteIdentifier *abs_ident;
          unique_ptr<AbsoluteIdentifier> tmp_abs_ident;
          KeyIdentifier key_ident;
          {
            AbsoluteIdentifier module_abs_ident;
            bool is_added_abs_ident;
            if(!get_module_abs_ident(context, *(module_def->ident()), module_abs_ident, module_def->pos(), errors)) return false;
            auto iter = module_abs_ident.idents().begin();
            while(true) {
              tmp_abs_ident.reset(new AbsoluteIdentifier());
              auto inserter = back_inserter(tmp_abs_ident->idents());
              copy(module_abs_ident.idents().begin(), iter, inserter);
              if(!add_ident_or_get_key_ident(context, tmp_abs_ident.get(), key_ident, is_added_abs_ident, module_def->pos(), errors)) return false;
              abs_ident = tmp_abs_ident.get();
              if(is_added_abs_ident) tmp_abs_ident.release();
              context.tree.add_module(key_ident);
              if(iter == module_abs_ident.idents().end()) break;
              iter++;
              tmp_abs_ident.release();
            }
          }
          module_def->ident()->set_key_ident(key_ident);
          AbsoluteIdentifier saved_current_module = context.current_module_ident;
          context.current_module_ident = *abs_ident;
          bool tmp_is_success = add_defs(context, module_def->defs(), errors);
          context.current_module_ident = saved_current_module;
          return tmp_is_success;
        },
        [&](VariableDefinition *var_def) -> bool {
          unique_ptr<AbsoluteIdentifier> abs_ident(new AbsoluteIdentifier(context.current_module_ident, var_def->ident()));
          bool is_added_abs_ident;
          KeyIdentifier key_ident;
          if(!add_ident_or_get_key_ident(context, abs_ident.get(), key_ident, is_added_abs_ident, var_def->pos(), errors)) return false;
          if(is_added_abs_ident) abs_ident.release();
          bool tmp_is_success = true;
          if(!context.tree.add_var(key_ident, var_def->access_modifier(), var_def->var())) {
            if(var_def->var()->is_template())
              errors.push_back(Error(var_def->pos(), "variable template " + abs_ident->to_string() + " is already defined"));
            else
              errors.push_back(Error(var_def->pos(), "variable " + abs_ident->to_string() + " is already defined"));
            tmp_is_success = false;
          }
          context.tree.uncompiled_var_key_idents().push_back(key_ident);
          return tmp_is_success;
        },
        [](VariableInstanceDefinition *var_inst_def) -> bool {
          return true;
        },
        [&](FunctionDefinition *fun_def) -> bool {
          unique_ptr<AbsoluteIdentifier> abs_ident(new AbsoluteIdentifier(context.current_module_ident, fun_def->ident()));
          bool is_added_abs_ident;
          KeyIdentifier key_ident;
          if(!add_ident_or_get_key_ident(context, abs_ident.get(), key_ident, is_added_abs_ident, fun_def->pos(), errors)) return false;
          if(is_added_abs_ident) abs_ident.release();
          bool tmp_is_success = true;
          shared_ptr<Variable> fun_var(new FunctionVariable(fun_def->fun()));
          if(!context.tree.add_var(key_ident, fun_def->access_modifier(), fun_var)) {
            if(fun_def->fun()->is_template())
              errors.push_back(Error(fun_def->pos(), "function template " + abs_ident->to_string() + " is already defined"));
            else
              errors.push_back(Error(fun_def->pos(), "function " + abs_ident->to_string() + " is already defined"));
            tmp_is_success = false;
          }
          context.tree.uncompiled_var_key_idents().push_back(key_ident);
          return tmp_is_success;
        },
        [](FunctionInstanceDefinition *fun_inst_def) -> bool {
          return true;
        },
        [&](TypeVariableDefinition *type_var_def) -> bool {
          unique_ptr<AbsoluteIdentifier> abs_ident(new AbsoluteIdentifier(context.current_module_ident, type_var_def->ident()));
          bool is_added_abs_ident;
          KeyIdentifier key_ident;
          if(!add_ident_or_get_key_ident(context, abs_ident.get(), key_ident, is_added_abs_ident, type_var_def->pos(), errors)) return false;
          if(is_added_abs_ident) abs_ident.release();
          bool tmp_is_success = true;
          if(!context.tree.add_type_var(key_ident, type_var_def->access_modifier(), type_var_def->var())) {
            errors.push_back(Error(type_var_def->pos(), "type " + abs_ident->to_string() + " is already defined"));
            tmp_is_success = false;
          }
          context.template_flag = false;
          tmp_is_success &= add_constrs_from_type_var(context, type_var_def->var(), type_var_def->access_modifier(), key_ident, type_var_def->pos(), errors);
          context.template_flag = false;
          context.tree.uncompiled_type_var_key_idents().push_back(key_ident);
          return tmp_is_success;
        },
        [&](TypeFunctionDefinition *type_fun_def) -> bool {
          unique_ptr<AbsoluteIdentifier> abs_ident(new AbsoluteIdentifier(context.current_module_ident, type_fun_def->ident()));
          bool is_added_abs_ident;
          KeyIdentifier key_ident;
          if(!add_ident_or_get_key_ident(context, abs_ident.get(), key_ident, is_added_abs_ident, type_fun_def->pos(), errors)) return false;
          if(is_added_abs_ident) abs_ident.release();
          bool tmp_is_success = true;
          if(!context.tree.add_type_fun(key_ident, type_fun_def->access_modifier(), type_fun_def->fun())) {
            errors.push_back(Error(def->pos(), "type template" + abs_ident->to_string() + " is already defined"));
            tmp_is_success = false;
          }
          context.template_flag = true;
          tmp_is_success &= add_constrs_from_type_fun(context, type_fun_def->fun(), type_fun_def->access_modifier(), key_ident, type_fun_def->pos(), errors);
          context.template_flag = false;
          context.tree.uncompiled_type_fun_key_idents().push_back(key_ident);
          return tmp_is_success;
        },
        [&](TypeFunctionInstanceDefinition *type_fun_inst_def) -> bool {
          return add_constrs_from_type_fun_inst(context, type_fun_inst_def->fun_inst(), &(type_fun_inst_def->ident()), type_fun_inst_def->pos(), errors);
        });
      }
      return is_success;
    }

    static bool get_non_alias_var(ResolverContext &context, const Identifier &ident, shared_ptr<Variable> &var, const Position &pos, list<Error> &errors)
    {
      unordered_set<KeyIdentifier> marked_key_idents;
      KeyIdentifier key_ident = ident.key_ident();
      var = context.tree.var(ident);
      marked_key_idents.insert(key_ident);
      while(true) {
        if(var.get() == nullptr) {
          errors.push_back(Error(pos, "internal error: variable isn't found"));
          return false;
        }
        AliasVariable *alias_var = dynamic_cast<AliasVariable *>(var.get());
        if(alias_var == nullptr) break;
        if(alias_var->is_template() && !alias_var->inst_type_params().empty()) break;
        if(alias_var->ident()->has_key_ident()) {
          key_ident = alias_var->ident()->key_ident();
          if(marked_key_idents.find(key_ident) != marked_key_idents.end()) {
            errors.push_back(Error(pos, "alias variable " + ident.to_abs_ident_string(*(context.tree.ident_table())) + " refers to alias cycle"));
            var.reset();
            return false;
          }
          var = context.tree.var(*(alias_var->ident()));
          marked_key_idents.insert(key_ident);
        } else {
          errors.push_back(Error(pos, "alias variable " + ident.to_abs_ident_string(*(context.tree.ident_table())) + " refers to undefined variable"));
          var.reset();
          return false;
        }
      }
      return true;
    }

    static bool set_key_ident(ResolverContext &context, AbsoluteIdentifier &ident, function<bool (const AbsoluteIdentifier &, AccessModifier &, bool &)> get_access_modifier_fun, function<void (const AbsoluteIdentifier &)> *add_private_error_fun = nullptr, function<void ()> *add_undefined_error_fun = nullptr)
    {
      if(!ident.set_key_ident(*(context.tree.ident_table()))) {
        if(add_undefined_error_fun != nullptr) (*add_undefined_error_fun)();
        return false;
      }
      AccessModifier access_modifier = AccessModifier::NONE;
      bool is_added;
      bool is_access_modifier = get_access_modifier_fun(ident, access_modifier, is_added);
      if(!is_added) {
        if(add_undefined_error_fun != nullptr) (*add_undefined_error_fun)();
        return false;
      }
      if(is_access_modifier) {
        AbsoluteIdentifier tmp_module_ident;
        bool has_tmp_module_ident = ident.get_module_ident(tmp_module_ident);
        if(!has_tmp_module_ident || tmp_module_ident != context.current_module_ident) {
          if(access_modifier == AccessModifier::PRIVATE) {
            ident.unset_key_ident();
            if(add_private_error_fun != nullptr) (*add_private_error_fun)(ident);
            return false;
          }
        }
      }
      return true;
    }

    static bool set_key_ident(ResolverContext &context, RelativeIdentifier &ident, const AbsoluteIdentifier &module_ident, function<bool (const AbsoluteIdentifier &, AccessModifier &, bool &)> get_access_modifier_fun, bool *is_private = nullptr, function<void (const AbsoluteIdentifier &)> *add_private_error_fun = nullptr)
    {
      AbsoluteIdentifier abs_ident(module_ident.idents(), ident);
      if(!abs_ident.set_key_ident(*(context.tree.ident_table()))) {
        if(is_private != nullptr) *is_private = false;
        return false;
      }
      AccessModifier access_modifier = AccessModifier::NONE;
      bool is_added;
      bool is_access_modifier = get_access_modifier_fun(abs_ident, access_modifier, is_added);
      if(!is_added) {
        if(is_private != nullptr) *is_private = false;
        return false;
      }
      if(is_access_modifier) {
        AbsoluteIdentifier tmp_module_ident;
        bool has_tmp_module_ident = abs_ident.get_module_ident(tmp_module_ident);
        if(!has_tmp_module_ident || tmp_module_ident != context.current_module_ident) {
          if(access_modifier == AccessModifier::PRIVATE) {
            abs_ident.unset_key_ident();
            if(add_private_error_fun != nullptr) (*add_private_error_fun)(abs_ident);
            if(is_private != nullptr) *is_private = true;
            return false;
          }
        }
      }
      ident.set_key_ident(abs_ident.key_ident());
      return true;
    }

    static bool resolve_ident(ResolverContext &context, Identifier *ident, const Position &pos, list<Error> &errors, function<bool (const AbsoluteIdentifier &, AccessModifier &, bool &)> get_access_modifier_fun, function<void (const AbsoluteIdentifier &)> add_private_error_fun, function<void ()> add_undefined_error_fun, bool are_local_vars = true)
    {
      return dynamic_match(ident,
      [&](Identifier *ident) -> bool {
        errors.push_back(Error(pos, "internal error: unknown identifier class"));
        return false;
      },
      [&](AbsoluteIdentifier *ident) -> bool {
        return set_key_ident(context, *ident, get_access_modifier_fun, &add_private_error_fun, &add_undefined_error_fun);
      },
      [&](RelativeIdentifier *ident) -> bool {
        if(are_local_vars && ident->idents().size() == 1) {
          if(set_local_var_index(context, *ident)) return true;
        }
        if(set_key_ident(context, *ident, context.current_module_ident, get_access_modifier_fun))
          return true;
        auto iter = context.imported_module_ident_stack.rbegin();
        for(; iter != context.imported_module_ident_stack.rend(); iter++) {
          auto iter2 = iter->rbegin();
          for(; iter2 != iter->rend(); iter2++) {
            if(set_key_ident(context, *ident, *iter2, get_access_modifier_fun))
              return true;
          }
        }
        bool is_private;
        if(set_key_ident(context, *ident, context.predef_module_ident, get_access_modifier_fun, &is_private, &add_private_error_fun)) {
          return true;
        } else {
          if(is_private) return false;
          add_undefined_error_fun();
          return false;
        }
      });
    }

    static bool resolve_module_ident(ResolverContext &context, Identifier *ident, const Position &pos, list<Error> &errors)
    {
      return resolve_ident(context, ident, pos, errors,
      [&context](const AbsoluteIdentifier &abs_ident, AccessModifier &access_modifier, bool &is_added_module) {
        is_added_module = context.tree.has_module_key_ident(abs_ident);
        return false;
      },
      [&pos, &errors](const AbsoluteIdentifier &abs_ident) {
        errors.push_back(Error(pos, "module " + abs_ident.to_string() + " is private"));
      },
      [&ident, &pos, &errors]() {
        errors.push_back(Error(pos, "module " + ident->to_string() + " is undefined"));
      }, false);
    }

    static bool resolve_var_ident(ResolverContext &context, Identifier *ident, const Position &pos, list<Error> &errors)
    {
      return resolve_ident(context, ident, pos, errors,
      [&context](const AbsoluteIdentifier &abs_ident, AccessModifier &access_modifier, bool &is_added_var) {
        VariableInfo *info = context.tree.var_info(abs_ident);
        is_added_var = (info != nullptr);
        if(is_added_var && info->must_update_access_modifier()) {
          AbsoluteIdentifier datatype_abs_ident;
          if(!abs_ident.get_module_ident(datatype_abs_ident)) {
            is_added_var = false;
            return false;
          }
          datatype_abs_ident.idents().push_back(*(info->datatype_ident()));
          if(!datatype_abs_ident.set_key_ident(*(context.tree.ident_table()))) {
            is_added_var = false;
            return false;
          }
          TypeFunctionInfo *datatype_info = context.tree.type_fun_info(datatype_abs_ident);
          if(datatype_info == nullptr) {
            is_added_var = false;
            return false;
          }
          info->update_access_modifier(datatype_info->access_modifier());
        }
        if(is_added_var) access_modifier = info->access_modifier();
        return is_added_var;
      },
      [&pos, &errors](const AbsoluteIdentifier &abs_ident) {
        errors.push_back(Error(pos, "variable " + abs_ident.to_string() + " is private"));
      },
      [&ident, &pos, &errors]() {
        errors.push_back(Error(pos, "variable " + ident->to_string() + " is undefined"));
      });
    }

    static bool resolve_type_var_ident(ResolverContext &context, Identifier *ident, const Position &pos, list<Error> &errors)
    {
      return resolve_ident(context, ident, pos, errors,
      [&context](const AbsoluteIdentifier &abs_ident, AccessModifier &access_modifier, bool &is_added_type_var) {
        TypeVariableInfo *info = context.tree.type_var_info(abs_ident);
        is_added_type_var = (info != nullptr);
        if(is_added_type_var) access_modifier = info->access_modifier();
        return is_added_type_var;
      },
      [&pos, &errors](const AbsoluteIdentifier &abs_ident) {
        errors.push_back(Error(pos, "type " + abs_ident.to_string() + " is private"));
      },
      [&ident, &pos, &errors]() {
        errors.push_back(Error(pos, "type " + ident->to_string() +" is undefined"));
      }, false);
    }

    static bool resolve_type_fun_ident(ResolverContext &context, Identifier *ident, const Position &pos, list<Error> &errors)
    {
      return resolve_ident(context, ident, pos, errors,
      [&context](const AbsoluteIdentifier &abs_ident, AccessModifier &access_modifier, bool &is_added_type_fun) {
        TypeFunctionInfo *info = context.tree.type_fun_info(abs_ident);
        is_added_type_fun = (info != nullptr);
        if(is_added_type_fun) access_modifier = info->access_modifier();
        return is_added_type_fun;
      },
      [&pos, &errors](const AbsoluteIdentifier &abs_ident) {
        errors.push_back(Error(pos, "type template " + abs_ident.to_string() + " is private"));
      },
      [&ident, &pos, &errors]() {
        errors.push_back(Error(pos, "type template " + ident->to_string() + " is undefined"));
      }, false);
    }

    static bool resolve_idents_from_expr(ResolverContext &context, Expression *expr, list<Error> &errors);

    static bool resolve_idents_from_pattern(ResolverContext &context, Pattern *pattern, list<Error> &errors);

    static bool resolve_idents_from_value(ResolverContext &context, Value *value, list<Error> &errors);

    static bool resolve_idents_from_type_expr(ResolverContext &context, TypeExpression *expr, list<Error> &errors, bool can_add_param_types = false);

    static bool resolve_idents_from_args(ResolverContext &context, const list<unique_ptr<Argument>> &args, list<Error> &errors, bool can_add_param_types = false);

    static bool resolve_idents_from_literal_value(ResolverContext &context, LiteralValue *value, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(value,
      [&pos, &errors](LiteralValue *value) -> bool {
        errors.push_back(Error(pos, "internal error: unknown literal value class"));
        return false;
      },
      [](SimpleLiteralValue *value) -> bool {
        return true;
      },
      [&](LambdaValue *value) -> bool {
        NonUniqueLambdaValue *non_unique_lambda_value = dynamic_cast<NonUniqueLambdaValue *>(value);
        if(non_unique_lambda_value != nullptr && non_unique_lambda_value->fun_modifier() == FunctionModifier::PRIMITIVE)
          push_closure_limit(context, context.local_var_count);
        bool is_success = resolve_idents_from_args(context, value->args(), errors);
        is_success &= resolve_idents_from_type_expr(context, value->result_type_expr(), errors);
        is_success &= resolve_idents_from_expr(context, value->body(), errors);
        if(non_unique_lambda_value != nullptr && non_unique_lambda_value->fun_modifier() == FunctionModifier::PRIMITIVE)
          pop_closure_limit(context);
        pop_local_vars(context);
        return is_success;
      });
    }

    static bool resolve_idents_from_expr_named_field_pairs(ResolverContext &context, const list<unique_ptr<ExpressionNamedFieldPair>> &pairs, const unordered_map<string, size_t> &indices, function<string ()> *constr_abs_ident_string_fun, list<Error> &errors)
    {
      bool is_success = true;
      set<size_t> used_indices;
      for(auto &pair : pairs) {
        auto iter = indices.find(pair->ident());
        if(iter != indices.end()) {
          pair->set_index(iter->second);
          auto iter = used_indices.find(pair->index());
          if(iter == used_indices.end()) {
            used_indices.insert(pair->index());
          } else {
            errors.push_back(Error(pair->pos(), "field " + pair->to_ident_string() + " is already specified"));
            is_success = false;
          }
        } else {
          if(constr_abs_ident_string_fun != nullptr)
            errors.push_back(Error(pair->pos(), "field " + pair->to_ident_string() + " is undefined at constructor " + (*constr_abs_ident_string_fun)()));
          is_success = false;
        }
        is_success &= resolve_idents_from_expr(context, pair->expr(), errors);
      }
      return is_success;
    }

    static bool resolve_idents_from_binds(ResolverContext &context, const list<unique_ptr<Binding>> &binds, const Position &pos, list<Error> &errors)
    {
      bool is_success = true;
      unordered_set<KeyIdentifier> used_key_idents;
      push_local_var_vector(context);
      for(auto &bind : binds) {
        is_success &= dynamic_match(bind.get(),
        [&](Binding *bind) -> bool {
          errors.push_back(Error(pos, "internal error: unknown binding class"));
          return false;
        },
        [&](VariableBinding *bind) -> bool {
          bool tmp_is_success = true;
          if(!push_local_var(context, *bind)) {
            errors.push_back(Error(bind->pos(), "variable " + bind->to_ident_string() + " is already defined"));
            tmp_is_success = false;
          }
          return tmp_is_success;
        },
        [&](TupleBinding *bind) -> bool {
          bool tmp_is_success = true;
          for(auto &var : bind->vars()) {
            if(var.get() != nullptr) {
              if(!push_local_var(context, *var)) {
                errors.push_back(Error(var->pos(), "variable " + var->to_ident_string() + " is already defined"));
                tmp_is_success = false;
              }
            }
          }
          return tmp_is_success;
        });
      }
      clear_top_local_var_idents(context);
      for(auto &bind : binds) {
        is_success &= dynamic_match(bind.get(),
        [&](Binding *bind) -> bool {
          errors.push_back(Error(pos, "internal error: unknown binding class"));
          return false;
        },
        [&](VariableBinding *bind) -> bool {
          return resolve_idents_from_expr(context, bind->expr(), errors);
        },
        [&](TupleBinding *bind) -> bool {
          return resolve_idents_from_expr(context, bind->expr(), errors);
        });
      }
      return is_success;
    }

    static bool resolve_idents_from_expr(ResolverContext &context, Expression *expr, list<Error> &errors)
    {
      return dynamic_match(expr,
      [&errors](Expression *expr) -> bool {
        errors.push_back(Error(expr->pos(), "internal error: unknown expression class"));
        return false;
      },
      [&](Literal *literal) -> bool {
        return resolve_idents_from_literal_value(context, literal->literal_value(), literal->pos(), errors);
      },
      [&](Collection *collection) -> bool {
        bool is_success = true;
        for(auto &elem : collection->elems()) {
          is_success &= resolve_idents_from_expr(context, elem.get(), errors);
        }
        return is_success;
      },
      [&](Tuple *tuple) -> bool {
        bool is_success = true;
        for(auto &field : tuple->fields()) {
          is_success &= resolve_idents_from_expr(context, field.get(), errors);
        }
        return is_success;
      },
      [&](VariableExpression *var_expr) -> bool {
        return resolve_var_ident(context, var_expr->ident(), var_expr->pos(), errors);
      },
      [&](NamedFieldConstructorApplication *app) -> bool {
        bool is_success = resolve_var_ident(context, app->constr_ident(), app->pos(), errors);
        shared_ptr<Variable> constr_var;
        if(is_success) is_success &= get_non_alias_var(context, *(app->constr_ident()), constr_var, app->pos(), errors);
        unordered_map<string, size_t> indices;
        function<string ()> constr_abs_ident_string_fun = [&context, app]() {
          return app->constr_ident()->to_abs_ident_string(*(context.tree.ident_table()));
        };
        function<string ()> *constr_abs_ident_string_fun_ptr = nullptr;
        if(constr_var.get() != nullptr) {
          is_success &= dynamic_match(constr_var.get(),
          [&](Variable *var) -> bool {
            errors.push_back(Error(app->pos(), "variable " + constr_abs_ident_string_fun() + " isn't constructor"));
            return false;
          },
          [&](ConstructorVariable *constr_var) -> bool {
            return dynamic_match(constr_var->constr().get(),
            [&](Constructor *constr) -> bool {
              errors.push_back(Error(app->pos(), "constructor " + constr_abs_ident_string_fun() + " isn't function constructor with named fields"));
              return false;
            },
            [&](NamedFieldConstructor *constr) -> bool {
              size_t index = 0;
              for(auto &field_type : constr->field_types()) {
                indices.insert(make_pair(field_type->ident(), index));
                index++;
              }
              constr_abs_ident_string_fun_ptr = &constr_abs_ident_string_fun;
              if(app->fields().size() < constr->field_count()) {
                errors.push_back(Error(app->pos(), "too few fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              if(app->fields().size() > constr->field_count()) {
                errors.push_back(Error(app->pos(), "too many fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              return true;
            });
          });
        } else
          is_success = false;
        is_success &= resolve_idents_from_expr_named_field_pairs(context, app->fields(), indices, constr_abs_ident_string_fun_ptr, errors);
        return is_success;
      },
      [&](Application *app) -> bool {
        bool is_success = resolve_idents_from_expr(context, app->fun(), errors);
        for(auto &arg : app->args()) {
          is_success &= resolve_idents_from_expr(context, arg.get(), errors);
        }
        return is_success;
      },
      [&](BuiltinApplication *app) -> bool {
        bool is_success = true;
        for(auto &arg : app->args()) {
          is_success &= resolve_idents_from_expr(context, arg.get(), errors);
        }
        return is_success;
      },
      [&](Field *field) -> bool {
        return resolve_idents_from_expr(context, field->expr(), errors);
      },
      [&](UniqueField *field) -> bool {
        return resolve_idents_from_expr(context, field->expr(), errors);
      },
      [&](SetUniqueField *set_field) -> bool {
        bool is_success = resolve_idents_from_expr(context, set_field->expr(), errors);
        is_success &= resolve_idents_from_expr(context, set_field->value_expr(), errors);
        return is_success;
      },
      [&](NamedField *field) -> bool {
        return resolve_idents_from_expr(context, field->expr(), errors);
      },
      [&](UniqueNamedField *field) -> bool {
        return resolve_idents_from_expr(context, field->expr(), errors);
      },
      [&](SetUniqueNamedField *set_field) -> bool {
        return resolve_idents_from_expr(context, set_field->value_expr(), errors);
      },
      [&](TypedExpression *typed_expr) -> bool {
        bool is_success = resolve_idents_from_expr(context, typed_expr->expr(), errors);
        is_success &= resolve_idents_from_type_expr(context, typed_expr->type_expr(), errors);
        return is_success;
      },
      [&](Let *let) -> bool {
        bool is_success = resolve_idents_from_binds(context, let->binds(), let->pos(), errors);
        is_success &= resolve_idents_from_expr(context, let->expr(), errors);
        pop_local_vars(context);
        return is_success;
      },
      [&](Match *match) -> bool {
        bool is_success = resolve_idents_from_expr(context, match->expr(), errors);
        for(auto &caze : match->cases()) {
          push_local_var_vector(context);
          is_success &= resolve_idents_from_pattern(context, caze->pattern(), errors);
          clear_top_local_var_idents(context);
          is_success &= resolve_idents_from_expr(context, caze->expr(), errors);
          pop_local_vars(context);
        }
        return is_success;
      },
      [&](Throw *throv) {
        return resolve_idents_from_expr(context, throv->expr(), errors);
      });
    }

    static bool resolve_idents_from_pattern_named_field_pairs(ResolverContext &context, const list<unique_ptr<PatternNamedFieldPair>> &pairs, const unordered_map<string, size_t> &indices, function<string ()> *constr_abs_ident_string_fun, list<Error> &errors)
    {
      bool is_success = true;
      set<size_t> used_indices;
      for(auto &pair : pairs) {
        auto iter = indices.find(pair->ident());
        if(iter != indices.end()) {
          pair->set_index(iter->second);
          auto iter = used_indices.find(pair->index());
          if(iter == used_indices.end()) {
            used_indices.insert(pair->index());
          } else {
            errors.push_back(Error(pair->pos(), "field " + pair->to_ident_string() + " is already specified"));
            is_success = false;
          }
        } else {
          if(constr_abs_ident_string_fun != nullptr)
            errors.push_back(Error(pair->pos(), "field " + pair->to_ident_string() + " is undefined at constructor " + (*constr_abs_ident_string_fun)()));
          is_success = false;
        }
        is_success &= resolve_idents_from_pattern(context, pair->pattern(), errors);
      }
      return is_success;
    }

    static bool resolve_idents_from_pattern(ResolverContext &context, Pattern *pattern, list<Error> &errors)
    {
      return dynamic_match(pattern,
      [&errors](Pattern *pattern) -> bool {
        errors.push_back(Error(pattern->pos(), "internal error: unknown pattern class"));
        return false;
      },
      [&](VariableConstructorPattern *pattern) -> bool {
        bool is_success = resolve_var_ident(context, pattern->constr_ident(), pattern->pos(), errors);
        shared_ptr<Variable> constr_var;
        if(is_success) is_success &= get_non_alias_var(context, *(pattern->constr_ident()), constr_var, pattern->pos(), errors);
        unordered_set<KeyIdentifier> key_idents;
        function<string ()> constr_abs_ident_string_fun = [&context, pattern]() {
          return pattern->constr_ident()->to_abs_ident_string(*(context.tree.ident_table()));
        };
        if(constr_var.get() != nullptr) {
          is_success &= dynamic_match(constr_var.get(),
          [&](Variable *var) -> bool {
            errors.push_back(Error(pattern->pos(), "variable " + constr_abs_ident_string_fun() + " isn't constructor"));
            return false;
          },
          [&](ConstructorVariable *constr_var) -> bool {
            return dynamic_match(constr_var->constr().get(),
            [&](Constructor *constr) -> bool {
              errors.push_back(Error(pattern->pos(), "constructor " + constr_abs_ident_string_fun() + " isn't veriable constructor"));
              return false;
            },
            [&](VariableConstructor *constr) -> bool {
              return true;
            });
          });
        } else
          is_success = false;
        return is_success;
      },
      [&](UnnamedFieldConstructorPattern *pattern) -> bool {
        bool is_success = resolve_var_ident(context, pattern->constr_ident(), pattern->pos(), errors);
        shared_ptr<Variable> constr_var;
        if(is_success) is_success &= get_non_alias_var(context, *(pattern->constr_ident()), constr_var, pattern->pos(), errors);
        function<string ()> constr_abs_ident_string_fun = [&context, pattern]() {
          return pattern->constr_ident()->to_abs_ident_string(*(context.tree.ident_table()));
        };
        if(constr_var.get() != nullptr) {
          is_success &= dynamic_match(constr_var.get(),
          [&](Variable *var) -> bool {
            errors.push_back(Error(pattern->pos(), "variable " + constr_abs_ident_string_fun() + " isn't constructor"));
            return false;
          },
          [&](ConstructorVariable *constr_var) -> bool {
            return dynamic_match(constr_var->constr().get(),
            [&](Constructor *constr) -> bool {
              errors.push_back(Error(pattern->pos(), "constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
              return false;
            },
            [&](FunctionConstructor *constr) -> bool {
              if(pattern->field_patterns().size() < constr->field_count()) {
                errors.push_back(Error(pattern->pos(), "too few fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              if(pattern->field_patterns().size() > constr->field_count()) {
                errors.push_back(Error(pattern->pos(), "too many fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              return true;
            });
          });
        } else 
          is_success = false;
        for(auto &field_pattern : pattern->field_patterns()) {
          is_success &= resolve_idents_from_pattern(context, field_pattern.get(), errors);
        }
        return is_success;
      },
      [&](NamedFieldConstructorPattern *pattern) -> bool {
        bool is_success = resolve_var_ident(context, pattern->constr_ident(), pattern->pos(), errors);
        shared_ptr<Variable> constr_var;
        if(is_success) is_success &= get_non_alias_var(context, *(pattern->constr_ident()), constr_var, pattern->pos(), errors);
        unordered_map<string, size_t> indices;
        function<string ()> constr_abs_ident_string_fun = [&context, pattern]() {
          return pattern->constr_ident()->to_abs_ident_string(*(context.tree.ident_table()));
        };
        function<string ()> *constr_abs_ident_string_fun_ptr = nullptr;
        if(constr_var.get() != nullptr) {
          is_success &= dynamic_match(constr_var.get(),
          [&](Variable *var) -> bool {
            errors.push_back(Error(pattern->pos(), "variable " + constr_abs_ident_string_fun() + " isn't constructor"));
            return false;
          },
          [&](ConstructorVariable *constr_var) -> bool {
            return dynamic_match(constr_var->constr().get(),
            [&](Constructor *constr) -> bool {
              errors.push_back(Error(pattern->pos(), "constructor " + constr_abs_ident_string_fun() + " isn't function constructor with named fields"));
              return false;
            },
            [&](NamedFieldConstructor *constr) -> bool {
              size_t index = 0;
              for(auto &field_type : constr->field_types()) {
                indices.insert(make_pair(field_type->ident(), index));
                index++;
              }
              if(pattern->field_patterns().size() < constr->field_count()) {
                errors.push_back(Error(pattern->pos(), "too few fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              if(pattern->field_patterns().size() > constr->field_count()) {
                errors.push_back(Error(pattern->pos(), "too many fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              return true;
            });
          });
        } else
          is_success = false;
        is_success &= resolve_idents_from_pattern_named_field_pairs(context, pattern->field_patterns(), indices, constr_abs_ident_string_fun_ptr, errors);
        return is_success;
      },
      [&](CollectionPattern *pattern) -> bool {
        bool is_success = true;
        for(auto &elem_pattern : pattern->elem_patterns()) {
          is_success &= resolve_idents_from_pattern(context, elem_pattern.get(), errors);
        }
        return is_success;
      },
      [&](TuplePattern *pattern) -> bool {
        bool is_success = true;
        for(auto &field_pattern : pattern->field_patterns()) {
          is_success &= resolve_idents_from_pattern(context, field_pattern.get(), errors);
        }
        return is_success;
      },
      [](LiteralPattern *pattern) -> bool {
        return true;
      },
      [&](VariablePattern *pattern) -> bool {
        bool is_success = true;
        if(!push_local_var(context, *pattern)) {
          errors.push_back(Error(pattern->pos(), "variable " + pattern->to_ident_string() + " is already defined"));
          is_success = false;
        }
        return is_success;
      },
      [&](AsPattern *pattern) -> bool {
        bool is_success = true;
        if(!push_local_var(context, *pattern)) {
          errors.push_back(Error(pattern->pos(), "variable " + pattern->to_ident_string() + " is already defined"));
          is_success = false;
        }
        is_success &= resolve_idents_from_pattern(context, pattern->pattern(), errors);
        return is_success;
      },
      [](WildcardPattern *pattern) -> bool {
        return true;
      },
      [&](TypedPattern *pattern) -> bool {
        bool is_success = resolve_idents_from_pattern(context, pattern->pattern(), errors);
        is_success = resolve_idents_from_type_expr(context, pattern->type_expr(), errors);
        return is_success;
      });
    }

    static bool resolve_idents_from_value_named_field_pairs(ResolverContext &context, const list<unique_ptr<ValueNamedFieldPair>> &pairs, const unordered_map<string, size_t> &indices, function<string ()> *constr_abs_ident_string_fun, list<Error> &errors)
    {
      bool is_success = true;
      set<size_t> used_indices;
      for(auto &pair : pairs) {
        auto iter = indices.find(pair->ident());
        if(iter != indices.end()) {
          pair->set_index(iter->second);
          auto iter = used_indices.find(pair->index());
          if(iter == used_indices.end()) {
            used_indices.insert(pair->index());
          } else {
            errors.push_back(Error(pair->pos(), "field " + pair->to_ident_string() + " is already specified"));
            is_success = false;
          }
        } else {
          if(constr_abs_ident_string_fun != nullptr)
            errors.push_back(Error(pair->pos(), "field " + pair->to_ident_string() + " is undefined at constructor " + (*constr_abs_ident_string_fun)()));
          is_success = false;
        }
        is_success &= resolve_idents_from_value(context, pair->value(), errors);
      }
      return is_success;
    }

    static bool resolve_idents_from_value(ResolverContext &context, Value *value, list<Error> &errors)
    {
      return dynamic_match(value,
      [&errors](Value *value) -> bool {
        errors.push_back(Error(value->pos(), "internal error: unknown value class"));
        return false;
      },
      [&](VariableLiteralValue *value) -> bool {
        bool is_success = check_and_clear_local_var_ident_stack(context, value->pos(), errors);
        is_success &= check_and_clear_closure_limit_stack(context, value->pos(), errors);
        is_success &= resolve_idents_from_literal_value(context, value->literal_value(), value->pos(), errors);
        return is_success;
      },
      [&](CollectionValue *value) -> bool {
        bool is_success = true;
        for(auto &elem : value->elems()) {
          is_success &= resolve_idents_from_value(context, elem.get(), errors);
        }
        return is_success;
      },
      [&](TupleValue *value) -> bool {
        bool is_success = true;
        for(auto &field : value->fields()) {
          is_success &= resolve_idents_from_value(context, field.get(), errors);
        }
        return is_success;
      },
      [&](VariableConstructorValue *value) -> bool {
        bool is_success = resolve_var_ident(context, value->constr_ident(), value->pos(), errors);
        shared_ptr<Variable> constr_var;
        if(is_success) is_success &= get_non_alias_var(context, *(value->constr_ident()), constr_var, value->pos(), errors);
        function<string ()> constr_abs_ident_string_fun = [&context, value]() {
          return value->constr_ident()->to_abs_ident_string(*(context.tree.ident_table()));
        };
        if(constr_var.get() != nullptr) {
          is_success &= dynamic_match(constr_var.get(),
          [&](Variable *var) -> bool {
            errors.push_back(Error(value->pos(), "variable " + constr_abs_ident_string_fun() + " isn't constructor"));
            return false;
          },
          [&](ConstructorVariable *constr_var) -> bool {
            return dynamic_match(constr_var->constr().get(),
            [&](Constructor *constr) -> bool {
              errors.push_back(Error(value->pos(), "constructor " + constr_abs_ident_string_fun() + " isn't veriable constructor"));
              return false;
            },
            [&](VariableConstructor *constr) -> bool {
              return true;
            });
          });
        } else
          is_success = false;
        return is_success;
      },
      [&](UnnamedFieldConstructorValue *value) -> bool {
        bool is_success = resolve_var_ident(context, value->constr_ident(), value->pos(), errors);
        shared_ptr<Variable> constr_var;
        if(is_success) is_success &= get_non_alias_var(context, *(value->constr_ident()), constr_var, value->pos(), errors);
        function<string ()> constr_abs_ident_string_fun = [&context, value]() {
          return value->constr_ident()->to_abs_ident_string(*(context.tree.ident_table()));
        };
        if(constr_var.get() != nullptr) {
          is_success &= dynamic_match(constr_var.get(),
          [&](Variable *var) -> bool {
            errors.push_back(Error(value->pos(), "variable " + constr_abs_ident_string_fun() + " isn't constructor"));
            return false;
          },
          [&](ConstructorVariable *constr_var) -> bool {
            return dynamic_match(constr_var->constr().get(),
            [&](Constructor *constr) -> bool {
              errors.push_back(Error(value->pos(), "constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
              return false;
            },
            [&](FunctionConstructor *constr) -> bool {
              if(value->fields().size() < constr->field_count()) {
                errors.push_back(Error(value->pos(), "too few fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              if(value->fields().size() > constr->field_count()) {
                errors.push_back(Error(value->pos(), "too many fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              return true;
            });
          });
        } else
          is_success = false;
        for(auto &field_value : value->fields()) {
          is_success &= resolve_idents_from_value(context, field_value.get(), errors);
        }
        return is_success;
      },
      [&](NamedFieldConstructorValue *value) -> bool {
        bool is_success = resolve_var_ident(context, value->constr_ident(), value->pos(), errors);
        shared_ptr<Variable> constr_var;
        if(is_success) is_success &= get_non_alias_var(context, *(value->constr_ident()), constr_var, value->pos(), errors);
        unordered_map<string, size_t> indices;
        function<string ()> constr_abs_ident_string_fun = [&context, value]() {
          return value->constr_ident()->to_abs_ident_string(*(context.tree.ident_table()));
        };
        function<string ()> *constr_abs_ident_string_fun_ptr = nullptr;
        if(constr_var.get() != nullptr) {
          is_success &= dynamic_match(constr_var.get(),
          [&](Variable *var) -> bool {
            errors.push_back(Error(value->pos(), "variable " + constr_abs_ident_string_fun() + " isn't constructor"));
            return false;
          },
          [&](ConstructorVariable *constr_var) -> bool {
            return dynamic_match(constr_var->constr().get(),
            [&](Constructor *constr) -> bool {
              errors.push_back(Error(value->pos(), "constructor " + constr_abs_ident_string_fun() + " isn't function constructor with named fields"));
              return false;
            },
            [&](NamedFieldConstructor *constr) -> bool {
              size_t index = 0;
              for(auto &field_type : constr->field_types()) {
                indices.insert(make_pair(field_type->ident(), index));
                index++;
              }
              constr_abs_ident_string_fun_ptr = &constr_abs_ident_string_fun;
              if(value->fields().size() < constr->field_count()) {
                errors.push_back(Error(value->pos(), "too few fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              if(value->fields().size() > constr->field_count()) {
                errors.push_back(Error(value->pos(), "too many fields of constructor " + constr_abs_ident_string_fun() + " isn't function constructor"));
                return false;
              }
              return true;
            });
          });
        } else
          is_success = false;
        is_success &= resolve_idents_from_value_named_field_pairs(context, value->fields(), indices, constr_abs_ident_string_fun_ptr, errors);
        return is_success;
      },
      [&](TypedValue *typed_value) -> bool {
        bool is_success = resolve_idents_from_value(context, typed_value->value(), errors);
        is_success &= resolve_idents_from_type_expr(context, typed_value->type_expr(), errors);
        return is_success;
      });
    }

    static bool resolve_idents_from_type_expr(ResolverContext &context, TypeExpression *expr, list<Error> &errors, bool can_add_type_params)
    {
      return dynamic_match(expr,
      [&errors](TypeExpression *expr) -> bool {
        errors.push_back(Error(expr->pos(), "internal error: unknown type expression class"));
        return false;
      },
      [&](With *with) -> bool {
        bool is_success = true;
        is_success &= resolve_idents_from_type_expr(context, with->type1(), errors, can_add_type_params);
        is_success &= resolve_idents_from_type_expr(context, with->type2(), errors, can_add_type_params);
        return is_success;
      },
      [&](TypeVariableExpression *type_var_expr) -> bool {
        return resolve_type_var_ident(context, type_var_expr->ident(), type_var_expr->pos(), errors);
      },
      [&](TypeParameterExpression *type_param_expr) -> bool {
        if(context.template_flag) {
          if(!set_type_param_index(context, *type_param_expr)) {
            if(can_add_type_params) {
              if(!add_type_param(context, *type_param_expr)) {
                errors.push_back(Error(type_param_expr->pos(), "type parameter " + type_param_expr->to_ident_string() + " is already defined"));
                return false;
              }
            } else {
              errors.push_back(Error(type_param_expr->pos(), "type parameter " + type_param_expr->to_ident_string() + " is undefined"));
              return false;
            }
          }
          return true;
        } else {
          errors.push_back(Error(type_param_expr->pos(), "type parameters only can used in templates"));
          return false;
        }
      },
      [&](TupleType *tuple_type) -> bool {
        bool is_success = true;
        for(auto &field_type : tuple_type->field_types()) {
          is_success &=  resolve_idents_from_type_expr(context, field_type.get(), errors, can_add_type_params);
        }
        return is_success;
      },
      [&](FunctionType *fun_type) -> bool {
        bool is_success = true;
        for(auto &arg_type : fun_type->arg_types()) {
          is_success &=  resolve_idents_from_type_expr(context, arg_type.get(), errors, can_add_type_params);
        }
        is_success &= resolve_idents_from_type_expr(context, fun_type->result_type(), errors, can_add_type_params);
        return is_success;
      },
      [&](TypeApplication *type_app) -> bool {
        bool is_success = resolve_type_fun_ident(context, type_app->fun_ident(), type_app->pos(), errors);
        for(auto &arg : type_app->args()) {
          is_success &=  resolve_idents_from_type_expr(context, arg.get(), errors, can_add_type_params);
        }
        return is_success;
      });
    }

    static bool resolve_idents_from_args(ResolverContext &context, const list<unique_ptr<Argument>> &args, list<Error> &errors, bool can_add_type_params)
    {
      bool is_success = true;
      push_local_var_vector(context);
      for(auto &arg : args) {
        if(!push_local_var(context, *arg)) {
          errors.push_back(Error(arg->pos(), "argument " + arg->to_ident_string() + " is already defined"));
          is_success = false;
        }
        if(arg->type_expr() != nullptr)
          is_success &= resolve_idents_from_type_expr(context, arg->type_expr(), errors, can_add_type_params);
      }
      clear_top_local_var_idents(context);
      return is_success;
    }

    static bool check_annotations(const list<unique_ptr<Annotation>> &annotations, list<Error> &errors)
    {
      bool is_success = true;
      bool is_eager = false;
      bool is_lazy = false;
      bool is_unmemoized = false;
      bool is_memoized = false;
      for(auto &annotation : annotations) {
        bool tmp_is_eager = false;
        bool tmp_is_lazy = false;
        bool tmp_is_unmemoized = false;
        bool tmp_is_memoized = false;
        if(annotation->ident() == "eager") tmp_is_eager = true;
        else if(annotation->ident() == "lazy") tmp_is_lazy = true;
        else if(annotation->ident() == "memoized") tmp_is_memoized = true;
        else if(annotation->ident() == "unmemoized") tmp_is_unmemoized = true;
        else if(annotation->ident() == "onlyeager") {
          tmp_is_eager = true;
          tmp_is_unmemoized = true;
        } else if(annotation->ident() == "onlylazy") {
          tmp_is_lazy = true;
          tmp_is_unmemoized = true;
        } else if(annotation->ident() == "onlymemoized") {
          tmp_is_eager = true;
          tmp_is_memoized = true;
        } else {
          errors.push_back(Error(annotation->pos(), "annotation " + annotation->to_string() + " is unsupported"));
          is_success = false;
          continue;
        }
        is_eager |= tmp_is_eager;
        is_lazy |= tmp_is_lazy;
        is_unmemoized |= tmp_is_unmemoized;
        is_memoized |= tmp_is_memoized;
        if((tmp_is_eager || tmp_is_lazy) && is_eager && is_lazy) {
          errors.push_back(Error(annotation->pos(), "function can't be eager and lazy"));
          is_success = false;
        }
        if((tmp_is_unmemoized || tmp_is_memoized) && is_unmemoized && is_memoized) {
          errors.push_back(Error(annotation->pos(), "function can't be unmemoized and memoized"));
          is_success = false;
        }
      }
      return is_success;
    }

    static bool resolve_idents_from_type_params(ResolverContext &context, const list<unique_ptr<TypeParameter>> &params, list<Error> &errors, bool can_add_type_params = false)
    {
      bool is_success = true;
      for(auto &param : params) {
        if(can_add_type_params) {
          if(!add_type_param(context, *param)) {
            errors.push_back(Error(param->pos(), "type parameter " + param->to_ident_string() + " is already defined"));
            is_success = false;
          }
        } else {
          if(!set_type_param_index(context, *param)) {
            errors.push_back(Error(param->pos(), "type parameter " + param->to_ident_string() + " is undefined"));
            is_success = false;
          }
        }
      }
      return is_success;
    }

    static bool resolve_idents_from_type_args(ResolverContext &context, const list<unique_ptr<TypeArgument>> &args, list<Error> &errors)
    {
      bool is_success = true;
      for(auto &arg : args) {
        if(!add_type_param(context, *arg)) {
          errors.push_back(Error(arg->pos(), "type argument " + arg->to_ident_string() + " is already defined"));
          is_success = false;
        }
      }
      return is_success;
    }

    static bool resolve_idents_from_type_named_field_pairs(ResolverContext &context, const list<unique_ptr<TypeNamedFieldPair>> &pairs, unordered_map<string, size_t> &indices, list<Error> &errors, function<string ()> constr_abs_ident_string_fun)
    {
      bool is_success = true;
      indices.clear();
      size_t index = 0;
      for(auto &pair : pairs) {
        if(!indices.insert(make_pair(pair->ident(), index)).second) {
          errors.push_back(Error(pair->pos(), "field " + pair->to_ident_string() + " is already defined at constructor " + constr_abs_ident_string_fun()));
          is_success = false;
        }
        is_success &= resolve_idents_from_type_expr(context, pair->type_expr(), errors);
      }
      return is_success;
    }

    static bool resolve_idents_from_constr(ResolverContext &context, const shared_ptr<Constructor> &constr, KeyIdentifier *datatype_key_ident, list<Error> &errors)
    {
      if(datatype_key_ident != nullptr) constr->set_datatype_key_ident(*datatype_key_ident);
      return dynamic_match(constr.get(),
      [&errors](Constructor *constr) -> bool {
        errors.push_back(Error(constr->pos(), "internal error: unknown constructor class"));
        return false;
      },
      [](VariableConstructor *constr) -> bool {
        return true;
      },
      [&](UnnamedFieldConstructor *constr) -> bool {
        bool is_success = check_annotations(constr->annotations(), errors);
        for(auto &field_type : constr->field_types()) {
          is_success &= resolve_idents_from_type_expr(context, field_type.get(), errors);
        }
        return is_success;
      },
      [&](NamedFieldConstructor *constr) -> bool {
        bool is_success = check_annotations(constr->annotations(), errors);
        unordered_map<string, size_t> indices;
        is_success &= resolve_idents_from_type_named_field_pairs(context, constr->field_types(), indices, errors,
        [constr, &context]() {
          AbsoluteIdentifier constr_abs_ident(context.current_module_ident, constr->ident());
          return constr_abs_ident.to_string();
        });
        constr->set_field_indices(indices);
        return is_success;
      });
    }

    static bool resolve_idents_from_datatype(ResolverContext &context, Datatype *datatype, const Position &pos, list<Error> &errors, KeyIdentifier *datatype_key_ident = nullptr)
    {
      return dynamic_match(datatype,
      [&pos, &errors](Datatype *datatype) -> bool {
        errors.push_back(Error(pos, "internal error: unknown datatype class"));
        return false;
      },
      [&](NonUniqueDatatype *datatype) -> bool {
        bool is_success = true;
        for(auto &constr : datatype->constrs()) {
          is_success &= resolve_idents_from_constr(context, constr, datatype_key_ident, errors);
        }
        return is_success;
      },
      [&](UniqueDatatype *datatype) -> bool {
        bool is_success = true;
        for(auto &constr : datatype->constrs()) {
          is_success &= resolve_idents_from_constr(context, constr, datatype_key_ident, errors);
        }
        return is_success;
      });
    }
    
    static bool resolve_idents_from_var(ResolverContext &context, const shared_ptr<Variable> &var, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(var.get(),
      [&pos, &errors](Variable *var) -> bool {
        errors.push_back(Error(pos, "internal error: unknown variable class"));
        return false;
      },
      [&](UserDefinedVariable *var) -> bool {
        bool is_success = check_and_clear_type_param_indices(context, pos, errors);
        if(var->is_template())
          is_success &= resolve_idents_from_type_params(context, var->inst_type_params(), errors, true);
        context.template_flag = var->is_template();
        if(var->type_expr() != nullptr)
          is_success &= resolve_idents_from_type_expr(context, var->type_expr(), errors, true);
        if(var->value() != nullptr)
          is_success &= resolve_idents_from_value(context, var->value(), errors);
        context.template_flag = false;
        clear_type_params(context);
        return is_success;
      },
      [&](ExternalVariable *var) -> bool {
        bool is_success = check_and_clear_type_param_indices(context, pos, errors);
        context.template_flag = false;
        is_success &= resolve_idents_from_type_expr(context, var->type_expr(), errors);
        context.template_flag = false;
        return is_success;
      },
      [](AliasVariable *var) -> bool {
        return true;
      });
    }

    static bool resolve_idents_from_var_inst(ResolverContext &context, const shared_ptr<VariableInstance> &inst, const Position &pos, list<Error> &errors)
    { return resolve_idents_from_var(context, inst->var(), pos, errors); }

    static bool resolve_idents_from_fun(ResolverContext &context, const shared_ptr<Function> &fun, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(fun.get(),
      [&pos, &errors](Function *fun) -> bool {
        errors.push_back(Error(pos, "internal error: unknown function class"));
        return false;
      },
      [&](UserDefinedFunction *fun) -> bool {
        bool is_success = check_and_clear_type_param_indices(context, pos, errors);
        if(fun->is_template())
          is_success &= resolve_idents_from_type_params(context, fun->inst_type_params(), errors, true);
        is_success &= check_annotations(fun->annotations(), errors);
        context.template_flag = fun->is_template();
        is_success &= check_and_clear_local_var_ident_stack(context, pos, errors);
        is_success &= check_and_clear_closure_limit_stack(context, pos, errors);
        is_success &= resolve_idents_from_args(context, fun->args(), errors, true);
        if(fun->result_type_expr() != nullptr)
          is_success &= resolve_idents_from_type_expr(context, fun->result_type_expr(), errors, true);
        if(fun->body() != nullptr)
          is_success &= resolve_idents_from_expr(context, fun->body(), errors);
        pop_local_vars(context);
        context.template_flag = false;
        clear_type_params(context);
        return is_success;
      },
      [&](ExternalFunction *fun) -> bool {
        bool is_success = check_and_clear_type_param_indices(context, pos, errors);
        context.template_flag = false;
        is_success &= check_and_clear_local_var_ident_stack(context, pos, errors);
        is_success &= resolve_idents_from_args(context, fun->args(), errors, true);
        if(fun->result_type_expr() != nullptr)
          is_success &= resolve_idents_from_type_expr(context, fun->result_type_expr(), errors, true);
        pop_local_vars(context);
        context.template_flag = false;
        return is_success;
      },
      [&](NativeFunction *fun) -> bool {
        bool is_success = check_annotations(fun->annotations(), errors);
        is_success &= check_and_clear_type_param_indices(context, pos, errors);
        context.template_flag = false;
        is_success &= check_and_clear_local_var_ident_stack(context, pos, errors);
        is_success &= resolve_idents_from_args(context, fun->args(), errors, true);
        if(fun->result_type_expr() != nullptr)
          is_success &= resolve_idents_from_type_expr(context, fun->result_type_expr(), errors, true);
        pop_local_vars(context);
        context.template_flag = false;
        return is_success;
      });
    }

    static bool resolve_idents_from_fun_inst(ResolverContext &context, const shared_ptr<FunctionInstance> &inst, const Position &pos, list<Error> &errors)
    { return resolve_idents_from_fun(context, inst->fun(), pos, errors); }

    static bool resolve_idents_from_type_var(ResolverContext &context, const shared_ptr<TypeVariable> &var, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(var.get(),
      [&pos, &errors](TypeVariable *var) -> bool {
        errors.push_back(Error(pos, "internal error: unknown type variable class"));
        return false;
      },
      [&](TypeSynonymVariable *var) -> bool {
        context.template_flag = false;
        bool is_success = resolve_idents_from_type_expr(context, var->expr(), errors);
        context.template_flag = false;
        return is_success;
      },
      [&](DatatypeVariable *var) -> bool {
        context.template_flag = false;
        bool is_success = resolve_idents_from_datatype(context, var->datatype(), pos, errors);
        context.template_flag = false;
        return is_success;
      },
      [&](BuiltinTypeVariable *var) -> bool {
        return true;
      });
    }

    static bool resolve_idents_from_type_fun(ResolverContext &context, const shared_ptr<TypeFunction> &fun, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(fun.get(),
      [&pos, &errors](TypeFunction *fun) -> bool {
        errors.push_back(Error(pos, "internal error: unknown type function class"));
        return false;
      },
      [&](TypeSynonymFunction *fun) -> bool {
        bool is_success = check_and_clear_type_param_indices(context, pos, errors);
        is_success &= resolve_idents_from_type_args(context, fun->args(), errors);
        is_success &= resolve_idents_from_type_params(context, fun->inst_type_params(), errors);
        context.template_flag = true;
        if(fun->body() != nullptr)
          is_success &= resolve_idents_from_type_expr(context, fun->body(), errors);
        context.template_flag = false;
        clear_type_params(context);
        return is_success;
      },
      [&](DatatypeFunction *fun) -> bool {
        bool is_success = check_and_clear_type_param_indices(context, pos, errors);
        is_success &= resolve_idents_from_type_args(context, fun->args(), errors);
        is_success &= resolve_idents_from_type_params(context, fun->inst_type_params(), errors);
        context.template_flag = true;
        is_success &= resolve_idents_from_datatype(context, fun->datatype(), pos, errors);
        context.template_flag = false;
        clear_type_params(context);
        return is_success;
      });
    }

    static bool resolve_idents_from_type_fun_inst(ResolverContext &context, const shared_ptr<TypeFunctionInstance> &inst, KeyIdentifier datatype_key_ident, const Position &pos, list<Error> &errors)
    { 
      return dynamic_match(inst.get(),
       [&pos, &errors](TypeFunctionInstance *inst) -> bool {
        errors.push_back(Error(pos, "internal error: unknown type function instance class"));
        return false;
      },
      [&](TypeSynonymFunctionInstance *inst) -> bool {
        bool is_success = check_and_clear_type_param_indices(context, pos, errors);
        context.template_flag = inst->is_template();
        for(auto &arg : inst->args()) {
          is_success &= resolve_idents_from_type_expr(context, arg.get(), errors, true);
        }
        is_success &= resolve_idents_from_type_expr(context, inst->body(), errors);
        clear_type_params(context);
        context.template_flag = false;
        return is_success;
      },
      [&](DatatypeFunctionInstance *inst) -> bool {
        bool is_success = check_and_clear_type_param_indices(context, pos, errors);
        context.template_flag = inst->is_template();
        for(auto &arg : inst->args()) {
          is_success &= resolve_idents_from_type_expr(context, arg.get(), errors, true);
        }
        is_success &= resolve_idents_from_datatype(context, inst->datatype(), pos, errors, &datatype_key_ident);
        clear_type_params(context);
        context.template_flag = false;
        return is_success;
      });
    }
    
    static bool resolve_idents_from_alias_var(ResolverContext &context, const shared_ptr<DefinableVariable> &var, const Position &pos, list<Error> &errors)
    {
      return dynamic_match(var.get(),
      [&pos, &errors](DefinableVariable *var) -> bool {
        errors.push_back(Error(pos, "internal error: unknown definable variable class"));
        return false;
      },
      [](UserDefinedVariable *var) -> bool {
        return true;
      },
      [](ExternalVariable *var) -> bool {
        return true;
      },
      [&](AliasVariable *var) -> bool {
        bool is_success = check_and_clear_type_param_indices(context, pos, errors);
        if(var->is_template())
          is_success &= resolve_idents_from_type_params(context, var->inst_type_params(), errors, true);
        context.template_flag = var->is_template();
        if(var->type_expr() != nullptr)
          is_success &= resolve_idents_from_type_expr(context, var->type_expr(), errors, true);
        is_success &= resolve_var_ident(context, var->ident(), pos, errors);
        context.template_flag = false;
        clear_type_params(context);
        return is_success;
      });
    }

    static bool resolve_idents_from_alias_defs(ResolverContext &context, const list<unique_ptr<Definition>> &defs, list<Error> &errors)
    {
      bool is_success = true;
      for(auto &def : defs) {
        is_success &= dynamic_match(def.get(), 
        [](const Definition *def) -> bool {
          return true;
        },
        [&](Import *import) -> bool {
          bool tmp_is_success = resolve_module_ident(context, import->module_ident(), import->pos(), errors);
          if(tmp_is_success) push_imported_module(context, *(import->module_ident()->abs_ident(*(context.tree.ident_table()))));
          return tmp_is_success;
        },
        [&](ModuleDefinition *module_def) -> bool {
          AbsoluteIdentifier saved_current_module = context.current_module_ident;
          context.current_module_ident = *(module_def->ident()->abs_ident(*(context.tree.ident_table())));
          push_imported_module_vector(context);
          bool tmp_is_success = resolve_idents_from_alias_defs(context, module_def->defs(), errors);
          pop_imported_modules(context);
          context.current_module_ident = saved_current_module;
          return tmp_is_success;
        },
        [&](VariableDefinition *var_def) -> bool {
          return resolve_idents_from_alias_var(context, var_def->var(), var_def->pos(), errors);
        });
      }
      return is_success;
    }

    static bool resolve_idents_from_defs(ResolverContext &context, const list<unique_ptr<Definition>> &defs, list<Error> &errors)
    {
      bool is_success = true;
      for(auto &def : defs) {
        is_success &= dynamic_match(def.get(), 
        [](const Definition *def) -> bool {
          return true;
        },
        [&](Import *import) -> bool {
          bool tmp_is_success = resolve_module_ident(context, import->module_ident(), import->pos(), errors);
          if(tmp_is_success) push_imported_module(context, *(import->module_ident()->abs_ident(*(context.tree.ident_table()))));
          return tmp_is_success;
        },
        [&](ModuleDefinition *module_def) -> bool {
          AbsoluteIdentifier saved_current_module = context.current_module_ident;
          context.current_module_ident = *(module_def->ident()->abs_ident(*(context.tree.ident_table())));
          push_imported_module_vector(context);
          bool tmp_is_success = resolve_idents_from_defs(context, module_def->defs(), errors);
          pop_imported_modules(context);
          context.current_module_ident = saved_current_module;
          return tmp_is_success;
        },
        [&](VariableDefinition *var_def) -> bool {
          return resolve_idents_from_var(context, var_def->var(), var_def->pos(), errors);
        },
        [&](VariableInstanceDefinition *var_inst_def) -> bool {
          AbsoluteIdentifier abs_ident(context.current_module_ident, var_inst_def->ident());
          bool tmp_is_success = resolve_var_ident(context, &abs_ident, var_inst_def->pos(), errors);
          VariableInfo *var_info = context.tree.var_info(abs_ident);
          if(var_info != nullptr)
            var_info->add_inst(var_inst_def->var_inst());
          tmp_is_success &= resolve_idents_from_var_inst(context, var_inst_def->var_inst(), var_inst_def->pos(), errors);
          return tmp_is_success;
        },
        [&](FunctionDefinition *fun_def) -> bool {
          return resolve_idents_from_fun(context, fun_def->fun(), fun_def->pos(), errors);
        },
        [&](FunctionInstanceDefinition *fun_inst_def) -> bool {
          AbsoluteIdentifier abs_ident(context.current_module_ident, fun_inst_def->ident());
          bool tmp_is_success = resolve_var_ident(context, &abs_ident, fun_inst_def->pos(), errors);
          VariableInfo *var_info = context.tree.var_info(abs_ident);
          if(var_info != nullptr)
            var_info->add_inst(fun_inst_def->fun_inst());
          tmp_is_success &= resolve_idents_from_fun_inst(context, fun_inst_def->fun_inst(), fun_inst_def->pos(), errors);
          return tmp_is_success;
        },
        [&](TypeVariableDefinition *type_var_def) -> bool {
          return resolve_idents_from_type_var(context, type_var_def->var(), type_var_def->pos(), errors);
        },
        [&](TypeFunctionDefinition *type_fun_def) -> bool {
          return resolve_idents_from_type_fun(context, type_fun_def->fun(), type_fun_def->pos(), errors);
        },
        [&](TypeFunctionInstanceDefinition *type_fun_inst_def) -> bool {
          AbsoluteIdentifier abs_ident(context.current_module_ident, type_fun_inst_def->ident());
          bool tmp_is_success = resolve_type_fun_ident(context, &abs_ident, type_fun_inst_def->pos(), errors);
          TypeFunctionInfo *type_fun_info = context.tree.type_fun_info(abs_ident);
          if(type_fun_info != nullptr)
            type_fun_info->add_inst(type_fun_inst_def->fun_inst());
          tmp_is_success &= resolve_idents_from_type_fun_inst(context, type_fun_inst_def->fun_inst(), abs_ident.key_ident(), type_fun_inst_def->pos(), errors);
          return tmp_is_success;
        });
      }
      return is_success;
    }

    //
    // A Resolver class.
    //

    Resolver::~Resolver() {}

    bool Resolver::resolve(Tree &tree, list<Error> &errors)
    {
      ResolverContext context(tree);
      bool is_success = true;
      is_success &= add_root_module(context, errors);
      for(auto &defs : tree.defs()) {
        is_success &= add_defs(context, *defs, errors);
      }
      for(auto &defs : tree.defs()) {
        clear_imported_module_ident_stack(context);
        push_imported_module_vector(context);
        is_success &= resolve_idents_from_alias_defs(context, *defs, errors);
      }
      for(auto &defs : tree.defs()) {
        clear_imported_module_ident_stack(context);
        push_imported_module_vector(context);
        is_success &= resolve_idents_from_defs(context, *defs, errors);
      }
      return is_success;
    }
  }
}
