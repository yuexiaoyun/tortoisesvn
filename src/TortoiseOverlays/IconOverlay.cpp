// TortoiseOverlays - an overlay handler for Tortoise clients
// Copyright (C) 2007,2009-2010 - TortoiseSVN
#include "stdafx.h"
#include "ShellExt.h"
#include "Guids.h"

// "The Shell calls IShellIconOverlayIdentifier::GetOverlayInfo to request the
//  location of the handler's icon overlay. The icon overlay handler returns
//  the name of the file containing the overlay image, and its index within
//  that file. The Shell then adds the icon overlay to the system image list."
STDMETHODIMP CShellExt::GetOverlayInfo(LPWSTR pwszIconFile, int cchMax, int *pIndex, DWORD *pdwFlags)
{
    int nInstalledOverlays = GetInstalledOverlays();

    // only a limited number of overlay slots can be used (determined by testing,
    // since not all overlay handlers are registered in the registry, e.g., the
    // shortcut (arrow) overlay isn't listed there).
    // The following overlays must be accounted for but are not listed under ShellIconOverlayIdentifiers:
    // * Shortcut arrow
    // * gray X on Vista+ (black dot on XP) for archived files
    // * Shared Hand (Windows XP only)
    // * UAC shield (Windows Vista+ only)

    const int nOverlayLimit = 12;


    bool dropIgnored = DropHandler(_T("ShowIgnoredOverlay"));
    if (dropIgnored)
        nInstalledOverlays--;

    bool dropUnversioned = DropHandler(_T("ShowUnversionedOverlay"));
    if (dropUnversioned)
        nInstalledOverlays--;

    bool dropAdded = DropHandler(_T("ShowAddedOverlay"));
    if (dropAdded)
        nInstalledOverlays--;

    bool dropLocked = DropHandler(_T("ShowLockedOverlay"));
    if (dropLocked)
        nInstalledOverlays--;

    bool dropReadonly = DropHandler(_T("ShowReadonlyOverlay"));
    if (dropReadonly)
        nInstalledOverlays--;

    bool dropDeleted = DropHandler(_T("ShowDeletedOverlay"));
    if (dropDeleted)
        nInstalledOverlays--;
    // The conflict, modified and normal overlays must not be disabled since
    // those are essential for Tortoise clients.


    if (dropIgnored     && (m_State == FileStateIgnored))
        return S_FALSE;
    if (dropUnversioned && (m_State == FileStateUnversioned))
        return S_FALSE;
    if (dropAdded       && (m_State == FileStateAdded))
        return S_FALSE;
    if (dropLocked      && (m_State == FileStateLocked))
        return S_FALSE;
    if (dropReadonly    && (m_State == FileStateReadOnly))
        return S_FALSE;
    if (dropDeleted     && (m_State == FileStateDeleted))
        return S_FALSE;

    //
    // If there are more than the maximum number of handlers registered, then
    // we have to drop some of our handlers to make sure that
    // the 'important' handlers are loaded properly:
    //
    // max     registered: drop the locked overlay
    // max + 1 registered: drop the locked and the ignored overlay
    // max + 2 registered: drop the locked, ignored and readonly overlay
    // max + 3 or more registered: drop the locked, ignored, readonly and unversioned overlay

    if ((m_State == FileStateUnversioned)&&(nInstalledOverlays > nOverlayLimit + 3))
        return S_FALSE;     // don't use the 'unversioned' overlay
    if ((m_State == FileStateReadOnly)&&(nInstalledOverlays > nOverlayLimit + 2))
        return S_FALSE;     // don't show the 'needs-lock' overlay
    if ((m_State == FileStateIgnored)&&(nInstalledOverlays > nOverlayLimit + 1))
        return S_FALSE;     // don't use the 'ignored' overlay
    if ((m_State == FileStateLocked)&&(nInstalledOverlays > nOverlayLimit))
        return S_FALSE;     // don't show the 'locked' overlay

    // Get folder icons from registry
    // Default icons are stored in LOCAL MACHINE
    // User selected icons are stored in CURRENT USER
    TCHAR regVal[1024];
    DWORD len = 1024;

    wstring icon;
    wstring iconName;

    HKEY hkeys [] = { HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE };
    switch (m_State)
    {
        case FileStateNormal        : iconName = _T("NormalIcon"); break;
        case FileStateModified      : iconName = _T("ModifiedIcon"); break;
        case FileStateConflict      : iconName = _T("ConflictIcon"); break;
        case FileStateDeleted       : iconName = _T("DeletedIcon"); break;
        case FileStateReadOnly      : iconName = _T("ReadOnlyIcon"); break;
        case FileStateLocked        : iconName = _T("LockedIcon"); break;
        case FileStateAdded         : iconName = _T("AddedIcon"); break;
        case FileStateIgnored       : iconName = _T("IgnoredIcon"); break;
        case FileStateUnversioned   : iconName = _T("UnversionedIcon"); break;
    }

    for (int i = 0; i < 2; ++i)
    {
        HKEY hkey = 0;

        if (::RegOpenKeyEx (hkeys[i],
            _T("Software\\TortoiseOverlays"),
                    0,
                    KEY_QUERY_VALUE,
                    &hkey) != ERROR_SUCCESS)
            continue;

        if (icon.empty() == true
            && (::RegQueryValueEx (hkey,
                             iconName.c_str(),
                             NULL,
                             NULL,
                             (LPBYTE) regVal,
                             &len)) == ERROR_SUCCESS)
            icon.assign (regVal, len);

        ::RegCloseKey(hkey);

    }

    // now load the Tortoise handlers and call their GetOverlayInfo method
    // note: we overwrite any changes a Tortoise handler will do to the
    // params and overwrite them later.
    LoadHandlers(pwszIconFile, cchMax, pIndex, pdwFlags);

    // Add name of appropriate icon
    if (icon.empty() == false)
        wcsncpy_s (pwszIconFile, cchMax, icon.c_str(), cchMax);
    else
        return S_FALSE;

    *pIndex = 0;
    *pdwFlags = ISIOI_ICONFILE;
    return S_OK;
};

STDMETHODIMP CShellExt::GetPriority(int *pPriority)
{
    if(pPriority == 0)
        return E_POINTER;
    switch (m_State)
    {
        case FileStateConflict:
            *pPriority = 0;
            break;
        case FileStateModified:
            *pPriority = 1;
            break;
        case FileStateDeleted:
            *pPriority = 2;
            break;
        case FileStateAdded:
            *pPriority = 3;
            break;
        case FileStateNormal:
            *pPriority = 4;
            break;
        case FileStateUnversioned:
            *pPriority = 5;
            break;
        case FileStateReadOnly:
            *pPriority = 6;
            break;
        case FileStateIgnored:
            *pPriority = 7;
            break;
        case FileStateLocked:
            *pPriority = 8;
            break;
        default:
            *pPriority = 100;
            return S_FALSE;
    }
    return S_OK;
}

STDMETHODIMP CShellExt::IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
{
    if(pwszPath == 0)
        return E_INVALIDARG;
    for (vector<DLLPointers>::iterator it = m_dllpointers.begin(); it != m_dllpointers.end(); ++it)
    {
        if (it->pShellIconOverlayIdentifier)
        {
            if (it->pShellIconOverlayIdentifier->IsMemberOf(pwszPath, dwAttrib) == S_OK)
                return S_OK;
        }
    }
    return S_FALSE;
}

int CShellExt::GetInstalledOverlays()
{
    // if there are more than 12 overlay handlers installed, then that means not all
    // of the overlay handlers can be shown. Windows chooses the ones first
    // returned by RegEnumKeyEx() and just drops the ones that come last in
    // that enumeration.
    int nInstalledOverlayhandlers = 0;
    // scan the registry for installed overlay handlers
    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers"),
        0, KEY_ENUMERATE_SUB_KEYS, &hKey)==ERROR_SUCCESS)
    {
        TCHAR value[2048];
        TCHAR keystring[2048];
        for (int i = 0, rc = ERROR_SUCCESS; rc == ERROR_SUCCESS; i++)
        {
            DWORD size = _countof(value );
            FILETIME last_write_time;
            rc = RegEnumKeyEx(hKey, i, value, &size, NULL, NULL, NULL, &last_write_time);
            if (rc == ERROR_SUCCESS)
            {
                DWORD dwType = 0;
                // check if there's a 'default' entry with a guid
                _tcscpy_s(keystring, _countof(keystring), _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers\\"));
                _tcscat_s(keystring, _countof(keystring), value);
                DWORD dwSize = _countof(value); // the API docs only specify "The size of the destination data buffer", 
                                                // but better be safe than sorry using _countof instead of sizeof
                if (SHGetValue(HKEY_LOCAL_MACHINE,
                    keystring,
                    NULL,
                    &dwType, value, &dwSize) == ERROR_SUCCESS)
                {
                    if ((dwSize > 10)&&(value[0] == '{'))
                        nInstalledOverlayhandlers++;
                }
            }
        }
    }
    RegCloseKey(hKey);
    return nInstalledOverlayhandlers;
}

void CShellExt::LoadHandlers(LPWSTR pwszIconFile, int cchMax, int *pIndex, DWORD *pdwFlags)
{
    HKEY hKey;
    wstring name;
    switch (m_State)
    {
    case FileStateNormal        : name = _T("Software\\TortoiseOverlays\\Normal"); break;
    case FileStateModified      : name = _T("Software\\TortoiseOverlays\\Modified"); break;
    case FileStateConflict      : name = _T("Software\\TortoiseOverlays\\Conflict"); break;
    case FileStateDeleted       : name = _T("Software\\TortoiseOverlays\\Deleted"); break;
    case FileStateReadOnly      : name = _T("Software\\TortoiseOverlays\\ReadOnly"); break;
    case FileStateLocked        : name = _T("Software\\TortoiseOverlays\\Locked"); break;
    case FileStateAdded         : name = _T("Software\\TortoiseOverlays\\Added"); break;
    case FileStateIgnored       : name = _T("Software\\TortoiseOverlays\\Ignored"); break;
    case FileStateUnversioned   : name = _T("Software\\TortoiseOverlays\\Unversioned"); break;
    }
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        name.c_str(),
        0, KEY_READ, &hKey)==ERROR_SUCCESS)
    {
        for (int i = 0, rc = ERROR_SUCCESS; rc == ERROR_SUCCESS; i++)
        {
            TCHAR k[MAX_PATH];
            TCHAR value[MAX_PATH];
            DWORD sizek = _countof(k);
            DWORD sizev = _countof(value);
            rc = RegEnumValue(hKey, i, k, &sizek, NULL, NULL, (LPBYTE)value, &sizev);
            if (rc == ERROR_SUCCESS)
            {
                TCHAR comobj[MAX_PATH];
                TCHAR modpath[MAX_PATH];
                _tcscpy_s(comobj, MAX_PATH, _T("CLSID\\"));
                _tcscat_s(comobj, MAX_PATH, value);
                _tcscat_s(comobj, MAX_PATH, _T("\\InprocServer32"));
                if (SHRegGetPath(HKEY_CLASSES_ROOT, comobj, _T(""), modpath, 0) == ERROR_SUCCESS)
                {
                    LoadRealLibrary(modpath, value, pwszIconFile, cchMax, pIndex, pdwFlags);
                }
            }
        }
    }
    RegCloseKey(hKey);
}

void CShellExt::LoadRealLibrary(LPCTSTR ModuleName, LPCTSTR clsid, LPWSTR pwszIconFile, int cchMax, int *pIndex, DWORD *pdwFlags)
{
    static const char GetClassObject[] = "DllGetClassObject";
    static const char CanUnloadNow[] = "DllCanUnloadNow";

    DLLPointers pointers;

    pointers.hDll = LoadLibraryEx(ModuleName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!pointers.hDll)
    {
        pointers.hDll = NULL;
        return;
    }

    pointers.pDllGetClassObject = NULL;
    pointers.pDllCanUnloadNow = NULL;
    pointers.pDllGetClassObject = (LPFNGETCLASSOBJECT)GetProcAddress(pointers.hDll, GetClassObject);
    if (pointers.pDllGetClassObject == NULL)
    {
        FreeLibrary(pointers.hDll);
        pointers.hDll = NULL;
        return;
    }
    pointers.pDllCanUnloadNow = (LPFNCANUNLOADNOW)GetProcAddress(pointers.hDll, CanUnloadNow);
    if (pointers.pDllCanUnloadNow == NULL)
    {
        FreeLibrary(pointers.hDll);
        pointers.hDll = NULL;
        return;
    }

    IID c;
    if (IIDFromString((LPOLESTR)clsid, &c) == S_OK)
    {
        IClassFactory * pClassFactory = NULL;
        if (SUCCEEDED(pointers.pDllGetClassObject(c, IID_IClassFactory, (LPVOID*)&pClassFactory)))
        {
            IShellIconOverlayIdentifier * pShellIconOverlayIdentifier = NULL;
            if (SUCCEEDED(pClassFactory->CreateInstance(NULL, IID_IShellIconOverlayIdentifier, (LPVOID*)&pShellIconOverlayIdentifier)))
            {
                pointers.pShellIconOverlayIdentifier = pShellIconOverlayIdentifier;
                if (pointers.pShellIconOverlayIdentifier->GetOverlayInfo(pwszIconFile, cchMax, pIndex, pdwFlags) != S_OK)
                {
                    // the overlay handler refused to be properly initialized
                    FreeLibrary(pointers.hDll);
                    pointers.hDll = NULL;
                    pClassFactory->Release();
                    pointers.pShellIconOverlayIdentifier->Release();
                    return;
                }
            }
            pClassFactory->Release();
        }
    }

    m_dllpointers.push_back(pointers);
}

bool CShellExt::DropHandler(LPCWSTR registryKey)
{
    bool drop = false;
    DWORD dwType = 0;
    DWORD dwData = 0;
    DWORD dwDataSize = 4;
    if (SHGetValue(HKEY_CURRENT_USER, _T("Software\\TortoiseOverlays"), registryKey, &dwType, &dwData, &dwDataSize) == ERROR_SUCCESS)
    {
        if (dwType == REG_DWORD)
        {
            if (dwData == 0)
            {
                drop = true;
            }
        }
    }
    return drop;
}