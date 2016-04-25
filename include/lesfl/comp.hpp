/****************************************************************************
 *   Copyright (C) 2016 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _LESFL_COMP_HPP
#define _LESFL_COMP_HPP

#include <memory>
#include <letin/comp.hpp>

namespace lesfl
{
  typedef letin::comp::SourceStream SourceStream;
  typedef letin::comp::Source Source;
  typedef letin::comp::Position Position;
  typedef letin::comp::Error Error;
  typedef letin::comp::Program LetinProgram;
  typedef letin::comp::Compiler LetinCompiler;

  class Interface
  {
    int _M_x;
  public:
    Interface() {}

    virtual ~Interface();
  };

  class Program
  {
    std::unique_ptr<LetinProgram> _M_letin_prog;
    std::unique_ptr<Interface> _M_iface;
  public:
    Program(LetinProgram *letin_prog, Interface *iface) :
      _M_letin_prog(letin_prog), _M_iface(iface) {}

    virtual ~Program();

    LetinProgram *letin_prog() const { return _M_letin_prog.get(); }

    Interface *iface() const { return _M_iface.get(); }
  };
    
  class Compiler
  {
    LetinCompiler *_M_letin_comp;
    std::vector<std::string> _M_lib_dirs;
  public:
    Compiler(LetinCompiler *letin_comp) : _M_letin_comp(letin_comp) {}

    virtual ~Compiler();

    Program *compile(const std::vector<Source> &sources, std::list<Error> &errors, bool is_iface = true);

    Program *compile(const char *file_name, std::list<Error> &errors, bool is_iface = true);

    void add_lib_dir(const std::string &dir_name);

    void add_lib_dir(const char *dir_name) { add_lib_dir(std::string(dir_name)); }
  };
}

#endif
