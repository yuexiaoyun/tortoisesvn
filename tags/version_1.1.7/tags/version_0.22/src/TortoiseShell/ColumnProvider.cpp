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

#include "ShellExt.h"
#include "guids.h"
#include "svnstatus.h"
#include "UnicodeStrings.h"
#include "PreserveChdir.h"
#include <string>
#include <Shlwapi.h>
#include <wininet.h>

// If this is set the SVN columns will always show if the current directory contains SVN content
// Otherwise the user will have to add the columns via the "more..." button
#define COLUMN_ALWAYS_SHOW 1

#ifdef COLUMN_ALWAYS_SHOW
const static int ColumnFlags = SHCOLSTATE_TYPE_STR|SHCOLSTATE_ONBYDEFAULT;
#else
const static int ColumnFlags = SHCOLSTATE_TYPE_STR|SHCOLSTATE_SECONDARYUI;
#endif

// IColumnProvider members
STDMETHODIMP CShellExt::GetColumnInfo(DWORD dwIndex, SHCOLUMNINFO *psci)
{
	if (dwIndex > 4)
		return S_FALSE;
	LoadLangDll();
	wide_string ws;
	switch (dwIndex)
	{
		case 0:
			psci->scid.fmtid = CLSID_TortoiseSVN_UPTODATE;
			psci->scid.pid = dwIndex;
			psci->vt = VT_BSTR;
			psci->fmt = LVCFMT_LEFT;
			psci->cChars = 15;
			psci->csFlags = ColumnFlags;

			MAKESTRING(IDS_COLTITLESTATUS);
#ifdef UNICODE
			lstrcpyW(psci->wszTitle, stringtablebuffer);
#else
			lstrcpyW(psci->wszTitle, MultibyteToWide(stringtablebuffer).c_str());
#endif
			MAKESTRING(IDS_COLDESCSTATUS);
#ifdef UNICODE
			lstrcpyW(psci->wszDescription, stringtablebuffer);
#else
			lstrcpyW(psci->wszDescription, MultibyteToWide(stringtablebuffer).c_str());
#endif
			break;
		case 1:
			psci->scid.fmtid = CLSID_TortoiseSVN_UPTODATE;
			psci->scid.pid = dwIndex;
			psci->vt = VT_BSTR;
			psci->fmt = LVCFMT_LEFT;
			psci->cChars = 15;
			psci->csFlags = ColumnFlags;

			MAKESTRING(IDS_COLTITLEREV);
#ifdef UNICODE
			lstrcpyW(psci->wszTitle, stringtablebuffer);
#else
			lstrcpyW(psci->wszTitle, MultibyteToWide(stringtablebuffer).c_str());
#endif
			MAKESTRING(IDS_COLDESCREV);
#ifdef UNICODE
			lstrcpyW(psci->wszDescription, stringtablebuffer);
#else
			lstrcpyW(psci->wszDescription, MultibyteToWide(stringtablebuffer).c_str());
#endif
			break;
		case 2:
			psci->scid.fmtid = CLSID_TortoiseSVN_UPTODATE;
			psci->scid.pid = dwIndex;
			psci->vt = VT_BSTR;
			psci->fmt = LVCFMT_LEFT;
			psci->cChars = 30;
			psci->csFlags = ColumnFlags;

			MAKESTRING(IDS_COLTITLEURL);
#ifdef UNICODE
			lstrcpyW(psci->wszTitle, stringtablebuffer);
#else
			lstrcpyW(psci->wszTitle, MultibyteToWide(stringtablebuffer).c_str());
#endif
			MAKESTRING(IDS_COLDESCURL);
#ifdef UNICODE
			lstrcpyW(psci->wszDescription, stringtablebuffer);
#else
			lstrcpyW(psci->wszDescription, MultibyteToWide(stringtablebuffer).c_str());
#endif
			break;
		case 3:
			psci->scid.fmtid = CLSID_TortoiseSVN_UPTODATE;
			psci->scid.pid = dwIndex;
			psci->vt = VT_BSTR;
			psci->fmt = LVCFMT_LEFT;
			psci->cChars = 30;
			psci->csFlags = ColumnFlags;

			MAKESTRING(IDS_COLTITLESHORTURL);
#ifdef UNICODE
			lstrcpyW(psci->wszTitle, stringtablebuffer);
#else
			lstrcpyW(psci->wszTitle, MultibyteToWide(stringtablebuffer).c_str());
#endif
			MAKESTRING(IDS_COLDESCSHORTURL);
#ifdef UNICODE
			lstrcpyW(psci->wszDescription, stringtablebuffer);
#else
			lstrcpyW(psci->wszDescription, MultibyteToWide(stringtablebuffer).c_str());
#endif
			break;
		case 4:
			psci->scid.fmtid = FMTID_SummaryInformation;	// predefined FMTID
			psci->scid.pid   = PIDSI_AUTHOR;				// Predefined - author
			psci->vt         = VT_LPSTR;					// We'll return the data as a string
			psci->fmt        = LVCFMT_LEFT;					// Text will be left-aligned in the column
			psci->csFlags    = SHCOLSTATE_TYPE_STR;			// Data should be sorted as strings
			psci->cChars     = 32;							// Default col width in chars
			break;
	}

	return S_OK;
}

STDMETHODIMP CShellExt::GetItemData(LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, VARIANT *pvarData)
{
	LoadLangDll();
	DWORD dwWaitResult = 0;
	if (pscid->fmtid == CLSID_TortoiseSVN_UPTODATE && pscid->pid < 4) 
	{
		PreserveChdir preserveChdir;
		stdstring szInfo;
#ifdef UNICODE
		std::wstring path = pscd->wszFile;
#else
		std::string path = WideToUTF8(pscd->wszFile);
#endif

		TCHAR buf[MAX_PATH];
		switch (pscid->pid) 
		{
			case 0:
				dwWaitResult = WaitForSingleObject(hMutex, 100);
				if (dwWaitResult == WAIT_OBJECT_0)
				{
					GetColumnStatus(path);
					SVNStatus::GetStatusString(g_hResInst, filestatus, buf, sizeof(buf), (WORD)CRegStdWORD(_T("Software\\TortoiseSVN\\LanguageID"), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT)));
					szInfo = buf;
					ReleaseMutex(hMutex);
				} // if (dwWaitResult == WAIT_OBJECT_0)
				break;
			case 1:
				dwWaitResult = WaitForSingleObject(hMutex, 100);
				if (dwWaitResult == WAIT_OBJECT_0)
				{
					GetColumnStatus(path);
					if (columnrev >= 0)
						_sntprintf(buf, MAX_PATH, _T("%d"), columnrev);
					else
						buf[0] = '\0';
					ReleaseMutex(hMutex);
				}
				szInfo = buf;
				break;
			case 2:
				dwWaitResult = WaitForSingleObject(hMutex, 100);
				if (dwWaitResult == WAIT_OBJECT_0)
				{
					GetColumnStatus(path);
					szInfo = itemurl;
					ReleaseMutex(hMutex);
				}
				break;
			case 3:
				dwWaitResult = WaitForSingleObject(hMutex, 100);
				if (dwWaitResult == WAIT_OBJECT_0)
				{
					GetColumnStatus(path);
					szInfo = itemshorturl;
					ReleaseMutex(hMutex);
				}
				break;
			default:
				return S_FALSE;
		}
#ifdef UNICODE
		wide_string wsInfo = szInfo;
#else
		wide_string wsInfo = MultibyteToWide(szInfo);
#endif
		V_VT(pvarData) = VT_BSTR;
		V_BSTR(pvarData) = SysAllocString(wsInfo.c_str());
		return S_OK;
	}
	if (pscid->fmtid == FMTID_SummaryInformation)
	{
		PreserveChdir preserveChdir;

		stdstring szInfo;
#ifdef UNICODE
		std::wstring path = pscd->wszFile;
#else
		std::string path = WideToMultibyte(pscd->wszFile);
#endif

		switch (pscid->pid)
		{
			case PIDSI_AUTHOR:			// author
				dwWaitResult = WaitForSingleObject(hMutex, 10);
				if (dwWaitResult == WAIT_OBJECT_0)
				{
					GetColumnStatus(path);
					szInfo = columnauthor;
					ReleaseMutex(hMutex);
				}
				break;
			default:
				return S_FALSE;
		} // switch (pscid->pid)
#ifdef UNICODE
		wide_string wsInfo = szInfo;
#else
		wide_string wsInfo = MultibyteToWide(szInfo);
#endif
		V_VT(pvarData) = VT_BSTR;
		V_BSTR(pvarData) = SysAllocString(wsInfo.c_str());
		return S_OK;
	}

	return S_FALSE;
}

STDMETHODIMP CShellExt::Initialize(LPCSHCOLUMNINIT psci)
{
#ifdef COLUMN_ALWAYS_SHOW
	// psci->wszFolder (WCHAR) holds the path to the folder to be displayed
	// Should check to see if its a "SVN" folder and if not return E_FAIL

	// The only problem with doing this is that we can't add this column in non-SVN folders.
	// Infact it shows up in non-SVN folders as some random other column).
	// Thats why this is turned off for !COLUMN_ALWAYS_SHOW
	PreserveChdir preserveChdir;
#ifdef UNICODE
	std::wstring path = psci->wszFolder;
#else
	std::string path = WideToMultibyte(psci->wszFolder);
#endif
	if (!path.empty())
	{
		if (svn_wc_status_unversioned==SVNStatus::GetAllStatus(path.c_str()))
			return E_FAIL;
	}
#else
#endif

	return S_OK;
}

void CShellExt::GetColumnStatus(stdstring path)
{
	if (columnfilepath.compare(path)==0)
		return;
	LoadLangDll();
	columnfilepath = path;
	filestatuscache * status = g_CachedStatus.GetFullStatus(path.c_str());
	filestatus = status->status;

#ifdef UNICODE
	columnauthor = UTF8ToWide(status->author);
#else
	columnauthor = status->author;
#endif
	columnrev = status->rev;
#ifdef UNICODE
	itemurl = UTF8ToWide(status->url);
#else
	itemurl = status->url;
#endif
	TCHAR urlpath[INTERNET_MAX_URL_LENGTH+1];

	URL_COMPONENTS urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);
	urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszUrlPath = urlpath;
	if (InternetCrackUrl(itemurl.c_str(), 0, ICU_DECODE, &urlComponents))
	{
		TCHAR * ptr = _tcsrchr(urlComponents.lpszUrlPath, '/');
		if (ptr == NULL)
			ptr = _tcsrchr(urlComponents.lpszUrlPath, '\\');
		if (ptr)
		{
			*ptr = '\0';
			itemshorturl = urlComponents.lpszUrlPath;
		} // if (ptr)
		else 
			itemshorturl = _T(" ");
	}
	else
		itemshorturl = _T(" ");
}

