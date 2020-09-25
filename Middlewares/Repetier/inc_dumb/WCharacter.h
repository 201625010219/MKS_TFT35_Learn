/*
 WCharacter.h - Character utility functions for Wiring & Arduino
 Copyright (c) 2010 Hernando Barragan.  All right reserved.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Character_h
#define Character_h

#include <ctype.h>

// WCharacter.h prototypes
inline boolean isAlphaNumeric(int c) ;
inline boolean isAlpha(int c) ;
inline boolean isAscii(int c) ;
inline boolean isWhitespace(int c) ;
inline boolean isControl(int c) ;
inline boolean isDigit(int c) ;
inline boolean isGraph(int c) ;
inline boolean isLowerCase(int c) ;
inline boolean isPrintable(int c) ;
inline boolean isPunct(int c) ;
inline boolean isSpace(int c) ;
inline boolean isUpperCase(int c) ;
inline boolean isHexadecimalDigit(int c) ;
inline int toAscii(int c) ;
inline int toLowerCase(int c) ;
inline int toUpperCase(int c);


// Checks for an alphanumeric character. 
// It is equivalent to (isalpha(c) || isdigit(c)).
inline boolean isAlphaNumeric(int c) 
{
  //dump
  
  //return ( isalnum(c) == 0 ? false : true);
}


// Checks for an alphabetic character. 
// It is equivalent to (isupper(c) || islower(c)).
inline boolean isAlpha(int c)
{
  //dump
  
  //return ( isalpha(c) == 0 ? false : true);
   return true;
}


// Checks whether c is a 7-bit unsigned char value 
// that fits into the ASCII character set.
inline boolean isAscii(int c)
{
  //dump
  
  //return ( isascii (c) == 0 ? false : true);
  
  return true;
}


// Checks for a blank character, that is, a space or a tab.
inline boolean isWhitespace(int c)
{
 //dump
  
  // return ( isblank (c) == 0 ? false : true);
   return true;
}


// Checks for a control character.
inline boolean isControl(int c)
{
  //dump
  
  //return ( iscntrl (c) == 0 ? false : true);
   return true;
}


// Checks for a digit (0 through 9).
inline boolean isDigit(int c)
{
 //dump
  
  // return ( isdigit (c) == 0 ? false : true);
   return true;
}


// Checks for any printable character except space.
inline boolean isGraph(int c)
{
  //dump
  
  //return ( isgraph (c) == 0 ? false : true); 
  return true;
}


// Checks for a lower-case character.
inline boolean isLowerCase(int c)
{
 //dump
  
  // return (islower (c) == 0 ? false : true);
   return true;
}


// Checks for any printable character including space.
inline boolean isPrintable(int c)
{
 //dump
  
  // return ( isprint (c) == 0 ? false : true);
   return true;
}


// Checks for any printable character which is not a space 
// or an alphanumeric character.
inline boolean isPunct(int c)
{
  //dump
  
  // return ( ispunct (c) == 0 ? false : true);
   return true;
}


// Checks for white-space characters. For the avr-libc library, 
// these are: space, formfeed ('\f'), newline ('\n'), carriage 
// return ('\r'), horizontal tab ('\t'), and vertical tab ('\v').
inline boolean isSpace(int c)
{
 //dump
  
  //  return ( isspace (c) == 0 ? false : true);
   return true;
}


// Checks for an uppercase letter.
inline boolean isUpperCase(int c)
{
 //dump
  
  //  return ( isupper (c) == 0 ? false : true);
   return true;
}


// Checks for a hexadecimal digits, i.e. one of 0 1 2 3 4 5 6 7 
// 8 9 a b c d e f A B C D E F.
inline boolean isHexadecimalDigit(int c)
{
  //dump
  
  // return ( isxdigit (c) == 0 ? false : true);
   return true;
}


// Converts c to a 7-bit unsigned char value that fits into the 
// ASCII character set, by clearing the high-order bits.
inline int toAscii(int c)
{
  //dump
  
  // return toascii (c);
   return true;
}


// Warning:
// Many people will be unhappy if you use this function. 
// This function will convert accented letters into random 
// characters.

// Converts the letter c to lower case, if possible.
inline int toLowerCase(int c)
{
  //dump
  
  // return tolower (c);
   return 0;
}


// Converts the letter c to upper case, if possible.
inline int toUpperCase(int c)
{
  //dump
  
  // return toupper (c);
   return 0;
}

#endif