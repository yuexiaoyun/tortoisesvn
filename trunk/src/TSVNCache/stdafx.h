// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#include <tchar.h>
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#include <atlbase.h>
#include <atlstr.h>

#include <conio.h>

// TODO: reference additional headers your program requires here

#define CSTRING_AVAILABLE


using namespace ATL;

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <deque>
#include <numeric>

#include "svn_wc.h"
#include "svn_client.h"
#include "svn_path.h"
#include "svn_pools.h"
#include "svn_utf.h"


typedef CComCritSecLock<CComAutoCriticalSection> AutoLocker;
