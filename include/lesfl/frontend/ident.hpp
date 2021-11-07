/****************************************************************************
 *   Copyright (C) 2016, 2021 Łukasz Szpakowski.                            *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _LESFL_FRONTEND_IDENT_HPP
#define _LESFL_FRONTEND_IDENT_HPP

#include <cstddef>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace lesfl
{
  namespace frontend
  {
    class AbsoluteIdentifier;
    class RelativeIdentifier;
    class AbsoluteIdentifierTable;

    class KeyIdentifier
    {
      std::size_t _M_key;
    public:
      KeyIdentifier() : _M_key(0) {}

      KeyIdentifier(std::size_t key): _M_key(key) {}

      bool operator==(const KeyIdentifier &key_ident) const
      { return _M_key == key_ident._M_key; }

      bool operator!=(const KeyIdentifier &key_ident) const
      { return _M_key != key_ident._M_key; }

      std::size_t key() const { return _M_key; }

      std::size_t hash() const
      {
        std::hash<std::size_t> hasher;
        return hasher(_M_key);
      }
    };

    class Stringable
    {
    protected:
      Stringable() {}
    public:
      virtual ~Stringable();

      virtual std::string to_string() const = 0;
    };

    class Indexable
    {
    protected:
      bool _M_has_index;
      std::size_t _M_index;

      Indexable() : _M_has_index(false) {}
    public:
      virtual ~Indexable();

      std::size_t index() const { return _M_index; }

      void set_index(std::size_t index) { _M_index = index; }
    };

    class Identifier : public Stringable
    {
    protected:
      std::list<std::string> _M_idents;
      bool _M_has_key_ident;
      KeyIdentifier _M_key_ident;

      Identifier() : _M_has_key_ident(false) {}

      Identifier(const char *ident) :
        _M_idents(std::list<std::string> { ident }), _M_has_key_ident(false) {}

      Identifier(const std::string &ident) :
        _M_idents(std::list<std::string> { ident }), _M_has_key_ident(false) {}

      Identifier(const std::list<std::string> &idents) :
        _M_idents(idents), _M_has_key_ident(false) {}
    public:
      virtual ~Identifier();

      const std::list<std::string> &idents() const { return _M_idents; }

      std::list<std::string> &idents() { return _M_idents; }

      bool has_key_ident() const { return _M_has_key_ident; }

      KeyIdentifier key_ident() const { return _M_key_ident; }

      void set_key_ident(KeyIdentifier key_ident)
      {
        _M_has_key_ident = true;
        _M_key_ident = key_ident;
      }

      void unset_key_ident() { _M_has_key_ident = false; }

      const AbsoluteIdentifier *abs_ident(const AbsoluteIdentifierTable &table) const;

      std::string to_abs_ident_string(const AbsoluteIdentifierTable &table) const;
    };
    
    class AbsoluteIdentifier : public Identifier
    {
    public:
      AbsoluteIdentifier() {}

      AbsoluteIdentifier(const char *ident) : Identifier(ident) {}

      AbsoluteIdentifier(const std::string &ident) : Identifier(ident) {}

      AbsoluteIdentifier(const std::list<std::string> &idents) : Identifier(idents) {}

      AbsoluteIdentifier(const AbsoluteIdentifier &abs_ident, const char *ident);

      AbsoluteIdentifier(const AbsoluteIdentifier &abs_ident, const std::string &ident);

      AbsoluteIdentifier(const AbsoluteIdentifier &abs_ident, const RelativeIdentifier &rel_ident);

      ~AbsoluteIdentifier();

      bool operator==(const AbsoluteIdentifier &ident) const;

      bool operator!=(const AbsoluteIdentifier &ident) const
      { return !(*this == ident); }

      void set_key_ident(KeyIdentifier key_ident)
      { return this->Identifier::set_key_ident(key_ident); }

      bool set_key_ident(const AbsoluteIdentifierTable &table);

      AbsoluteIdentifier *module_ident(const AbsoluteIdentifier &ident) const
      {
        std::unique_ptr<AbsoluteIdentifier> abs_ident(new AbsoluteIdentifier());
        if(!get_module_ident(*abs_ident)) return nullptr;
        return abs_ident.release();
      }

      bool get_module_ident(AbsoluteIdentifier &ident) const;

      std::size_t hash() const;

      std::string to_string() const;
    };

    class RelativeIdentifier : public Identifier, public Indexable
    {
    public:
      RelativeIdentifier() {}

      RelativeIdentifier(const char *ident) : Identifier(ident) {}

      RelativeIdentifier(const std::string &ident) : Identifier(ident) {}

      RelativeIdentifier(const std::list<std::string> &idents) : Identifier(idents) {}

      ~RelativeIdentifier();

      std::string to_string() const;
    };
  }
}

namespace std
{
  template<>
  struct hash<lesfl::frontend::KeyIdentifier>
  {
    std::size_t operator()(const lesfl::frontend::KeyIdentifier &key_ident) const
    { return key_ident.hash(); }
  };

  template<>
  struct hash<lesfl::frontend::AbsoluteIdentifier>
  {
    std::size_t operator()(const lesfl::frontend::AbsoluteIdentifier &ident) const
    { return ident.hash(); }
  };
}

namespace lesfl
{
  namespace frontend
  {
    class AbsoluteIdentifierTable
    {
      struct EqualTo
      {
        bool operator()(const AbsoluteIdentifier *ident1, const AbsoluteIdentifier *ident2) const
        { return ident1 == ident2 || *ident1 == *ident2; }
      };

      struct Hash
      {
        std::size_t operator()(const AbsoluteIdentifier *ident) const
        {
          std::hash<AbsoluteIdentifier> hasher;
          return hasher(*ident);
        }
      };

      std::unordered_map<KeyIdentifier, std::unique_ptr<const AbsoluteIdentifier>> _M_ident_map;
      std::unordered_set<const AbsoluteIdentifier *, Hash, EqualTo> _M_ident_set;
    public:
      AbsoluteIdentifierTable() {}

      virtual ~AbsoluteIdentifierTable();

      const AbsoluteIdentifier *ident(KeyIdentifier key_ident) const
      {
        auto iter = _M_ident_map.find(key_ident);
        return iter != _M_ident_map.end() ? iter->second.get() : nullptr;
      }

      const AbsoluteIdentifier *ident(const AbsoluteIdentifier *orig_ident) const; 

      bool add_ident(AbsoluteIdentifier *ident, KeyIdentifier &key_ident);

      bool add_ident_or_get_key_ident(AbsoluteIdentifier *ident, KeyIdentifier &key_ident, bool &is_added);
    };

    inline const AbsoluteIdentifier *Identifier::abs_ident(const AbsoluteIdentifierTable &table) const
    { return _M_has_key_ident ? table.ident(_M_key_ident) : nullptr; }

    inline std::string Identifier::to_abs_ident_string(const AbsoluteIdentifierTable &table) const
    { return abs_ident(table)->to_string(); }
  }
}

#endif
