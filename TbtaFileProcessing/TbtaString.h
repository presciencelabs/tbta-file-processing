#pragma once

#ifndef TbtaUseCString
#include <string>

#define TbtaStringAppend(x, y) x.append(y)
#define TbtaStringAppendBufferLength(dest, buffer, length) dest.append(buffer, length)
#define TbtaStringClear(x) x.clear()
#define TbtaStringIsEmpty(x) x.length() == 0
#define TbtaStringLength(x) x.length()
#define TbtaStringSetAt(str, position, newchar) str[position] = newchar
#define TbtaStringGetAt(str, position) str[position]
#define TbtaStringRemoveAt(str, position) str.erase(position, 1)
#define TbtaStringSubString(str, position, length) str.substr(position, length)
#endif

//Do I actually need afx.h?
#ifdef TbtaUseCString
#include <afx.h>

#define TbtaStringAppend(x, y) x+=y
#define TbtaStringAppendBufferLength(dest, buffer, length) dest+=CString(buffer, length)
#define TbtaStringClear(x) x.Empty()
#define TbtaStringIsEmpty(x) x.IsEmpty()
#define TbtaStringLength(x) x.GetLength()
#define TbtaStringSetAt(str, position, newchar) str.SetAt(position, newchar)
#define TbtaStringGetAt(str, position) str.GetAt(position)
#define TbtaStringRemoveAt(str, position) str.Delete(position, 1)
#define TbtaStringSubString(str, position, length) str.Mid(position, length)
#endif

#ifndef TbtaUseCString
//if cross-platform is a concern,
//it appears from what I can find in docs that most linux distributions eschew
//the use of wchar_t, preferring instead to treat all strings as "normal" char
//or std::string, with the implicit understanding that it is encoded in UTF8.
//Not 100% sure on this though.  Looking at the xercesc sources is probably a 
//good way to set up a cross-platform unicode pattern, since it is succesfully 
//used on multiple platforms.
//However, for the moment, we are assuming windows, which appears to want
//wchar_t (wstring) with the UTF16 encoding, which appears to also be
//what xercesc defaults to on Windows.	
typedef std::wstring TbtaString;
#endif

#ifdef TbtaUseCString
//it appears from what I can find in documentation that compiling with
//the UNICODE project settings will cause CSTring to use wchar_t with a UTF16 encoding.
//This appears to be what xerces also uses on the windows platform.
typedef CString TbtaString;
#endif