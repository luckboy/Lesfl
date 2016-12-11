/****************************************************************************
 *   Copyright (C) 2016 Łukasz Szpakowski.                                  *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _UTIL_HPP
#define _UTIL_HPP

namespace lesfl
{
  namespace util
  {
    namespace priv
    {
      template<typename _Fun>
      struct ArgumentTypeBase { typedef void Type; };

      template<typename _T, typename _U, typename _V>
      struct ArgumentTypeBase<_T (_U::*)(_V) const> { typedef _V Type; };

      template<typename _Fun>
      class ArgumentType : public ArgumentTypeBase<decltype(&_Fun::operator())> {};

      template<typename _Fun>
      struct ResultTypeBase { typedef void Type; };

      template<typename _T, typename _U, typename _V>
      struct ResultTypeBase<_T (_U::*)(_V) const> { typedef _T Type; };

      template<typename _Fun>
      class ResultType : public ResultTypeBase<decltype(&_Fun::operator())> {};
    }

    template<typename _T, typename _DefaultFun>
    inline typename priv::ResultType<_DefaultFun>::Type dynamic_match(_T *x, _DefaultFun default_fun)
    { return default_fun(x); }

    template<typename _T, typename _DefaultFun, typename _Fun, typename... _Funs>
    inline typename priv::ResultType<_DefaultFun>::Type dynamic_match(_T *x, _DefaultFun default_fun, _Fun fun, _Funs... funs)
    {
      typename priv::ArgumentType<_Fun>::Type x2 = dynamic_cast<typename priv::ArgumentType<_Fun>::Type>(x);
      if(x2 != nullptr)
        return fun(x2);
      else
        return dynamic_match(x, default_fun, funs...);
    }
  }
}

#endif
