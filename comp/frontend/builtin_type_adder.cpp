/****************************************************************************
 *   Copyright (C) 2021 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#include <vector>
#include <lesfl/frontend.hpp>

using namespace std;

namespace lesfl
{
  namespace frontend
  {
    BuiltinTypeAdder::~BuiltinTypeAdder() {}

    bool BuiltinTypeAdder::add_builtin_types(Tree &tree)
    {
      static vector<pair<string, BuiltinType>> builtin_types {
        make_pair("Char", BuiltinType::CHAR),
        make_pair("WChar", BuiltinType::WCHAR),
        make_pair("Int8", BuiltinType::INT8),
        make_pair("Int16", BuiltinType::INT16),
        make_pair("Int32", BuiltinType::INT32),
        make_pair("Int64", BuiltinType::INT64),
        make_pair("Float", BuiltinType::FLOAT),
        make_pair("Double", BuiltinType::DOUBLE)
      };
      static vector<pair<string, BuiltinTypeTemplate>> builtin_type_templates {
        make_pair("Array", BuiltinTypeTemplate::ARRAY),
        make_pair("UniqueArray", BuiltinTypeTemplate::UNIQUE_ARRAY)
      };
      unique_ptr<AbsoluteIdentifier> abs_ident(new AbsoluteIdentifier());
      bool is_added_abs_ident;
      KeyIdentifier key_ident;
      if(!tree.ident_table()->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added_abs_ident)) return false;
      if(is_added_abs_ident) abs_ident.release();
      if(!tree.add_module(key_ident))  return false;
      abs_ident.reset(new AbsoluteIdentifier("stdlib"));
      if(!tree.ident_table()->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added_abs_ident)) return false;
      if(is_added_abs_ident) abs_ident.release();
      if(!tree.add_module(key_ident)) return false;
      for(auto &tmp_pair : builtin_types) {
        abs_ident.reset(new AbsoluteIdentifier("stdlib"));
        abs_ident->idents().push_back(tmp_pair.first);
        if(!tree.ident_table()->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added_abs_ident)) return false;
        if(is_added_abs_ident) abs_ident.release();
        if(!tree.add_type_var(key_ident, AccessModifier::NONE, shared_ptr<TypeVariable>(new BuiltinTypeVariable(tmp_pair.second)))) return false;
      }
      for(auto &tmp_pair : builtin_type_templates) {
        abs_ident.reset(new AbsoluteIdentifier("stdlib"));
        abs_ident->idents().push_back(tmp_pair.first);
        if(!tree.ident_table()->add_ident_or_get_key_ident(abs_ident.get(), key_ident, is_added_abs_ident)) return false;
        if(is_added_abs_ident) abs_ident.release();
        if(!tree.add_type_fun(key_ident, AccessModifier::NONE, shared_ptr<TypeFunction>(new BuiltinTypeFunction(1, tmp_pair.second)))) return false;
      }
      return true;
    }
  }
}
