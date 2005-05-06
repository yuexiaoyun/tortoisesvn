// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003 - Tim Kemp and Stefan Kueng

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
#pragma once
#include <string>
#include <atlstr.h>

#pragma warning (push,1)
typedef std::basic_string<wchar_t> wide_string;
#ifdef UNICODE
#define stdstring wide_string
#else
#define stdstring std::string
#endif
#pragma warning (pop)

class CUnicodeUtils
{
public:
	CUnicodeUtils(void);
	~CUnicodeUtils(void);
#ifdef _MFC_VER
#ifdef UNICODE
	static CStringA GetUTF8(CString string);
	static CStringW GetUnicode(CStringA string);
#else
	static CStringA GetUTF8(CString string);
	static CStringA GetUnicode(CStringA string);
#endif
#endif
#ifdef UNICODE
	static std::string StdGetUTF8(wide_string wide);
	static wide_string StdGetUnicode(std::string multibyte);
#else
	static std::string StdGetUTF8(std::string str) {return str;}
	static std::string StdGetUnicode(std::string multibyte) {return multibyte;}
#endif
};
