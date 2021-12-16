/****************************************************************************
 *   Copyright (C) 2016, 2021 Łukasz Szpakowski.                            *
 *                                                                          *
 *   This software is licensed under the GNU Lesser General Public          *
 *   License v3 or later. See the LICENSE file and the GPL file for         *
 *   the full licensing terms.                                              *
 ****************************************************************************/
#ifndef _LESFL_FRONTEND_BUILTIN_HPP
#define _LESFL_FRONTEND_BUILTIN_HPP

namespace lesfl
{
  namespace frontend
  {
    enum class BuiltinFunction
    {
      CEQ,
      CNE,
      CLT,
      CGE,
      CGT,
      CLE,
      WCEQ,
      WCNE,
      WCLT,
      WCGE,
      WCGT,
      WCLE,
      INEG,
      IADD,
      ISUB,
      IMUL,
      IDIV,
      IMOD,
      INOT,
      IAND,
      IOR,
      IXOR,
      ISHL,
      ISHR,
      ISHRU,
      IEQ,
      INE,
      ILT,
      IGE,
      IGT,
      ILE,
      FNEG,
      FADD,
      FSUB,
      FMUL,
      FDIV,
      FEQ,
      FNE,
      FLT,
      FGE,
      FGT,
      FLE,
      FPOW,
      FSQRT,
      FEXP,
      FLOG,
      FCOS,
      FSIN,
      FTAN,
      FACOS,
      FASIN,
      FATAN,
      FCEIL,
      FFLOOR,
      FROUND,
      FTRUNC,
      ANTH,
      ALEN,
      ACAT,
      UAFILL,
      UANTH,
      UASNTH,
      UALEN,
      UATOA,
      CTOI,
      ITOC,
      WCTOI,
      ITOWC,
      ITOF,
      FTOI,
      TRY,
      UTRY,
      FORCE,
      RETHROW,
      STACKTRACE,
      I8TOI,
      I16TOI,
      I32TOI,
      ITOI8,
      ITOI16,
      ITOI32,
      SFTOF,
      FTOSF,
      AFILL,
      ATOUA,
      AMAP,
      UACAT,
      UAMAP
    };

    enum class BuiltinType
    {
      CHAR,
      WCHAR,
      INT8,
      INT16,
      INT32,
      INT64,
      FLOAT,
      DOUBLE
    };

    enum class BuiltinTypeTemplate
    {
      ARRAY,
      UNIQUE_ARRAY
    };
  }
}

#endif
