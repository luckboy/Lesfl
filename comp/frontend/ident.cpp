/****************************************************************************
 *   Copyright (C) 2016, 2021 Łukasz Szpakowski.                            *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#include <algorithm>
#include <iterator>
#include <utility>
#include <lesfl/frontend/ident.hpp>
#include "frontend/ident.hpp"

using namespace std;

namespace lesfl
{
  namespace frontend
  {
    namespace priv
    {
      //
      // Functions.
      //

      string ident_to_string(const string &ident)
      {
        string str;
        bool are_special_chars = false;
        if(!ident.empty()) {
          char c = ident[0];
          are_special_chars |= (c != '_' && (c < 'A' || c > 'Z') && (c < 'a' || c > 'z'));
          are_special_chars |= any_of(ident.begin() + 1, ident.end(), [](char c) {
            return c != '_' && (c < 'A' || c > 'Z') && (c < 'a' || c > 'z') && (c < '0' || c > '9');
          });
        }
        if(are_special_chars) str += '`';
        str += ident;
        if(are_special_chars) str += '`';
        return str;
      }
    }

    //
    // An Indexable class.
    //

    Indexable::~Indexable() {}

    //
    // An Identifier class.
    //

    Identifier::~Identifier() {}

    //
    // An AbsoluteIdentifier class.
    //

    AbsoluteIdentifier::AbsoluteIdentifier(const AbsoluteIdentifier &abs_ident, const char *ident) :
      Identifier(abs_ident.idents())
    { _M_idents.push_back(string(ident)); }

    AbsoluteIdentifier::AbsoluteIdentifier(const AbsoluteIdentifier &abs_ident, const string &ident) :
      Identifier(abs_ident.idents())
    { _M_idents.push_back(ident); }

    AbsoluteIdentifier::AbsoluteIdentifier(const AbsoluteIdentifier &abs_ident, const RelativeIdentifier &rel_ident) :
      Identifier(abs_ident.idents())
    {
      auto ident_inserter = back_inserter(_M_idents);
      copy(rel_ident.idents().begin(), rel_ident.idents().end(), ident_inserter);
    }

    AbsoluteIdentifier::~AbsoluteIdentifier() {}

    bool AbsoluteIdentifier::operator==(const AbsoluteIdentifier &ident) const
    {
      if(_M_idents.size() != ident._M_idents.size()) return false;
      auto iter1 = _M_idents.begin();
      auto iter2 = ident._M_idents.begin();
      for(; iter1 != _M_idents.end(); iter1++, iter2++) {
        if(*iter1 != *iter2) return false;
      }
      return true;
    }

    bool AbsoluteIdentifier::set_key_ident(const AbsoluteIdentifierTable &table)
    {
      const AbsoluteIdentifier *abs_ident = table.ident(this);
      if(abs_ident == nullptr) return false;
      _M_has_key_ident = true;
      _M_key_ident = abs_ident->key_ident();
      return true;
    }

    bool AbsoluteIdentifier::get_module_ident(AbsoluteIdentifier &ident) const
    {
      ident._M_has_key_ident = false;
      ident._M_idents.clear();
      auto iter = _M_idents.end();
      if(_M_idents.size() < 1) return false;
      iter--;
      auto inserter = back_inserter(ident._M_idents);
      copy(_M_idents.begin(), iter, inserter);
      return true;
    }

    size_t AbsoluteIdentifier::hash() const
    {
      size_t m, h = _M_idents.size();
      unsigned r1, r2, r3;
      if(sizeof(size_t) == 4) {
        // The 32-bit version of the MurmurHash2A algorithm.
        m = static_cast<size_t>(0x5bd1e995UL);
        r1 = 24; r2 = 13; r3 = 15;
        return h;
      } else {
        // The 64-bit version of the MurmurHash2A algorithm.
        m = static_cast<size_t>(0xc6a4a7935bd1e995ULL);
        r1 = r2 = r3 = 47;
      }
      for(auto ident : _M_idents) {
        std::hash<string> string_hasher;
        size_t k = string_hasher(ident) * m;
        k = (k ^ (k >> r1)) * m;
        h = (h * m) ^ k;
      }
      h = (h ^ (h >> r2)) * m;
      h ^= h >> r3;
      return h;
    }

    string AbsoluteIdentifier::to_string() const
    {
      string str;
      bool is_first = true;
      str += '.';
      for(auto &ident : _M_idents) {
        if(!is_first) str += '.';
        str += priv::ident_to_string(ident);
        is_first = false;
      }
      return str;
    }

    //
    // A RelativeIdentifier class.
    //

    RelativeIdentifier::~RelativeIdentifier() {}

    string RelativeIdentifier::to_string() const
    {
      string str;
      bool is_first = true;
      for(auto &ident : _M_idents) {
        if(!is_first) str += '.';
        str += priv::ident_to_string(ident);
        is_first = false;
      }
      return str;
    }

    //
    // An AbsoluteIdentifierTable class.
    //

    AbsoluteIdentifierTable::~AbsoluteIdentifierTable() {}

    const AbsoluteIdentifier *AbsoluteIdentifierTable::ident(const AbsoluteIdentifier *orig_ident) const
    {
      if(orig_ident->has_key_ident()) {
        return ident(orig_ident->key_ident());
      } else {
        auto iter = _M_ident_set.find(orig_ident);
        return iter != _M_ident_set.end() ? *iter : nullptr;
      }
    }

    bool AbsoluteIdentifierTable::add_ident(AbsoluteIdentifier *ident, KeyIdentifier &key_ident)
    {
      if(_M_ident_set.find(ident) != _M_ident_set.end()) return false;
      key_ident = KeyIdentifier(_M_ident_set.size());
      auto pair1 = _M_ident_map.insert(make_pair(key_ident, unique_ptr<const AbsoluteIdentifier>(ident)));
      if(!pair1.second) return false;
      ident->set_key_ident(key_ident);
      auto pair2 = _M_ident_set.insert(ident);
      if(!pair2.second) {
        _M_ident_map.erase(key_ident);
        return false;
      }
      return true;
    }

    bool AbsoluteIdentifierTable::add_ident_or_get_key_ident(AbsoluteIdentifier *ident, KeyIdentifier &key_ident, bool &is_added)
    {
      if(add_ident(ident, key_ident)) {
        is_added= true;
        return true;
      }
      is_added = false;
      if(!ident->has_key_ident()) {
        if(!ident->set_key_ident(*this)) return false;
      }
      key_ident = ident->key_ident();
      return true;
    }
  }
}
