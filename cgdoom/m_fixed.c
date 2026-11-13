// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// $Log:$
//
// DESCRIPTION:
//	Fixed point implementation.
//
//-----------------------------------------------------------------------------


//static const char 

#include "os.h"

#include "doomtype.h"
#include "i_system.h"

#ifdef __GNUG__
#pragma implementation "m_fixed.h"
#endif
#include "m_fixed.h"


//CGD: Bastl impl of int*int=>int64 , (int64 >>16) => int
fixed_t FixedMul( fixed_t	a, fixed_t	b )
{
  int64_t p = (int64_t)a * b;
  return (int32_t)(p >> 16);
}


//
// FixedDiv, C version.
//

unsigned int FixedDivU(unsigned int ua, unsigned int ub )
{
  unsigned int uRes = ua / ub;
  unsigned int uRem = ua - uRes * ub;
  int i;
  for(i=0;i<FRACBITS;i++)
  {
    uRes <<= 1;
    uRem <<= 1;
    if (uRem >= ub)
    {
      uRem -= ub;
      uRes++;
    }
  }
  return uRes;
}


fixed_t FixedDiv( fixed_t	a, fixed_t	b )
{
  int iResult;
  int iNegative = (a < 0) != (b<0);
  if ( (abs(a)>>14) >= abs(b))
    return iNegative ? MININT : MAXINT;
  iResult = FixedDivU(abs(a),abs(b));
  if(iNegative) iResult = -iResult;
  return iResult;
}
