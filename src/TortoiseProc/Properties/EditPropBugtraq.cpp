// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2010 - TortoiseSVN

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
#include "stdafx.h"
#include "TortoiseProc.h"
#include "EditPropBugtraq.h"
#include "UnicodeUtils.h"

// CEditPropBugtraq dialog

IMPLEMENT_DYNAMIC(CEditPropBugtraq, CResizableStandAloneDialog)

CEditPropBugtraq::CEditPropBugtraq(CWnd* pParent /*=NULL*/)
	: CResizableStandAloneDialog(CEditPropBugtraq::IDD, pParent)
    , EditPropBase()
    , m_bWarnIfNoIssue(FALSE)
    , m_sBugtraqUrl(_T(""))
    , m_sBugtraqMessage(_T(""))
    , m_sBugtraqLabel(_T(""))
    , m_sBugtraqRegex1(_T(""))
    , m_sBugtraqRegex2(_T(""))
    , m_sProviderUUID(_T(""))
    , m_sProviderUUID64(_T(""))
    , m_sProviderParams(_T(""))
{

}

CEditPropBugtraq::~CEditPropBugtraq()
{
}

void CEditPropBugtraq::DoDataExchange(CDataExchange* pDX)
{
    CResizableStandAloneDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_PROPRECURSIVE, m_bRecursive);
    DDX_Check(pDX, IDC_BUGTRAQWARN, m_bWarnIfNoIssue);
    DDX_Text(pDX, IDC_BUGTRAQURL, m_sBugtraqUrl);
    DDX_Text(pDX, IDC_BUGTRAQMESSAGE, m_sBugtraqMessage);
    DDX_Text(pDX, IDC_BUGTRAQLABEL, m_sBugtraqLabel);
    DDX_Text(pDX, IDC_BUGTRAQLOGREGEX1, m_sBugtraqRegex1);
    DDX_Text(pDX, IDC_BUGTRAQLOGREGEX2, m_sBugtraqRegex2);
    DDX_Text(pDX, IDC_UUID32, m_sProviderUUID);
    DDX_Text(pDX, IDC_UUID64, m_sProviderUUID64);
    DDX_Text(pDX, IDC_PARAMS, m_sProviderParams);
}


BEGIN_MESSAGE_MAP(CEditPropBugtraq, CResizableStandAloneDialog)
    ON_BN_CLICKED(IDHELP, &CEditPropBugtraq::OnBnClickedHelp)
    ON_WM_SIZING()
END_MESSAGE_MAP()


// CEditPropBugtraq message handlers

BOOL CEditPropBugtraq::OnInitDialog()
{
    CResizableStandAloneDialog::OnInitDialog();

    ExtendFrameIntoClientArea(IDC_DWM);
    m_aeroControls.SubclassControl(this, IDC_PROPRECURSIVE);
    m_aeroControls.SubclassOkCancelHelp(this);

    CheckRadioButton(IDC_TOPRADIO, IDC_BOTTOMRADIO, IDC_TOPRADIO);
    CheckRadioButton(IDC_TEXTRADIO, IDC_NUMERICRADIO, IDC_TEXTRADIO);

    for (IT it = m_properties.begin(); it != m_properties.end(); ++it)
    {
        if (it->first.compare(BUGTRAQPROPNAME_URL) == 0)
        {
            m_sBugtraqUrl = UTF8ToWide(it->second.value).c_str();
        }
        else if (it->first.compare(BUGTRAQPROPNAME_MESSAGE) == 0)
        {
            m_sBugtraqMessage = UTF8ToWide(it->second.value).c_str();
        }
        else if (it->first.compare(BUGTRAQPROPNAME_LABEL) == 0)
        {
            m_sBugtraqLabel = UTF8ToWide(it->second.value).c_str();
        }
        else if (it->first.compare(BUGTRAQPROPNAME_PROVIDERUUID) == 0)
        {
            m_sProviderUUID = UTF8ToWide(it->second.value).c_str();
        }
        else if (it->first.compare(BUGTRAQPROPNAME_PROVIDERUUID64) == 0)
        {
            m_sProviderUUID64 = UTF8ToWide(it->second.value).c_str();
        }
        else if (it->first.compare(BUGTRAQPROPNAME_PROVIDERPARAMS) == 0)
        {
            m_sProviderParams = UTF8ToWide(it->second.value).c_str();
        }
        else if (it->first.compare(BUGTRAQPROPNAME_LOGREGEX) == 0)
        {
            CString sRegex = UTF8ToWide(it->second.value).c_str();
            int nl = sRegex.Find('\n');
            if (nl >= 0)
            {
                m_sBugtraqRegex1 = sRegex.Mid(nl+1);
                m_sBugtraqRegex2 = sRegex.Left(nl);
            }
            else
                m_sBugtraqRegex1 = sRegex;
        }
        else if (it->first.compare(BUGTRAQPROPNAME_WARNIFNOISSUE) == 0)
        {
            CString sYesNo = UTF8ToWide(it->second.value).c_str();
            m_bWarnIfNoIssue = ((sYesNo.CompareNoCase(_T("yes")) == 0)||((sYesNo.CompareNoCase(_T("true")) == 0)));
        }
        else if (it->first.compare(BUGTRAQPROPNAME_APPEND) == 0)
        {
            CString sYesNo = UTF8ToWide(it->second.value).c_str();
            if ((sYesNo.CompareNoCase(_T("yes")) == 0)||((sYesNo.CompareNoCase(_T("true")) == 0)))
                CheckRadioButton(IDC_TOPRADIO, IDC_BOTTOMRADIO, IDC_BOTTOMRADIO);
        }
        else if (it->first.compare(BUGTRAQPROPNAME_NUMBER) == 0)
        {
            CString sYesNo = UTF8ToWide(it->second.value).c_str();
            if ((sYesNo.CompareNoCase(_T("yes")) == 0)||((sYesNo.CompareNoCase(_T("true")) == 0)))
                CheckRadioButton(IDC_TEXTRADIO, IDC_NUMERICRADIO, IDC_NUMERICRADIO);
        }
    }

    UpdateData(false);

    AdjustControlSize(IDC_BUGTRAQWARN);
    AdjustControlSize(IDC_TEXTRADIO);
    AdjustControlSize(IDC_NUMERICRADIO);
    AdjustControlSize(IDC_TOPRADIO);
    AdjustControlSize(IDC_BOTTOMRADIO);
    AdjustControlSize(IDC_PROPRECURSIVE);

    RECT rect;
    GetWindowRect(&rect);
    m_height = rect.bottom - rect.top;

    AddAnchor(IDC_ISSUETRACKERGROUP, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_URLLABEL, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_URLLABEL2, TOP_LEFT);
    AddAnchor(IDC_BUGTRAQURL, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_BUGTRAQWARN, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_MESSAGEGROUP, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_MESSAGEHINTLABEL, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_MESSAGEPATTERNLABEL, TOP_LEFT);
    AddAnchor(IDC_BUGTRAQMESSAGE, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_MESSAGELABEL, TOP_LEFT);
    AddAnchor(IDC_BUGTRAQLABEL, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_BUGIDLABEL, TOP_LEFT);
    AddAnchor(IDC_TEXTRADIO, TOP_LEFT);
    AddAnchor(IDC_NUMERICRADIO, TOP_LEFT);
    AddAnchor(IDC_INSERTLABEL, TOP_LEFT);
    AddAnchor(IDC_TOPRADIO, TOP_LEFT);
    AddAnchor(IDC_BOTTOMRADIO, TOP_LEFT);
    AddAnchor(IDC_REGEXGROUP, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_REGEXLABEL, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_REGEXIDLABEL, TOP_LEFT);
    AddAnchor(IDC_BUGTRAQLOGREGEX1, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_REGEXMSGLABEL, TOP_LEFT);
    AddAnchor(IDC_BUGTRAQLOGREGEX2, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_IBUGTRAQPROVIDERGROUP, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_UUIDLABEL32, TOP_LEFT);
    AddAnchor(IDC_UUID32, TOP_LEFT);
    AddAnchor(IDC_UUIDLABEL64, TOP_LEFT);
    AddAnchor(IDC_UUID64, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_PARAMSLABEL, TOP_LEFT);
    AddAnchor(IDC_PARAMS, TOP_LEFT, TOP_RIGHT);
    AddAnchor(IDC_DWM, TOP_LEFT);
    AddAnchor(IDC_PROPRECURSIVE, BOTTOM_LEFT, BOTTOM_RIGHT);
    AddAnchor(IDOK, BOTTOM_RIGHT);
    AddAnchor(IDCANCEL, BOTTOM_RIGHT);
    AddAnchor(IDHELP, BOTTOM_RIGHT);

    GetDlgItem(IDC_BUGTRAQURL)->SetFocus();
    return FALSE;
}

void CEditPropBugtraq::OnOK()
{
    UpdateData();

    // check whether the entered regex strings are valid
    try
    {
        std::tr1::wregex r1 = std::tr1::wregex(m_sBugtraqRegex1);
        UNREFERENCED_PARAMETER(r1);
    }
    catch (exception) 
    {
        ShowEditBalloon(IDC_BUGTRAQLOGREGEX1, IDS_ERR_INVALIDREGEX, IDS_ERR_ERROR);
        return;
    }
    try
    {
        std::tr1::wregex r2 = std::tr1::wregex(m_sBugtraqRegex2);
        UNREFERENCED_PARAMETER(r2);
    }
    catch (exception) 
    {
        ShowEditBalloon(IDC_BUGTRAQLOGREGEX2, IDS_ERR_INVALIDREGEX, IDS_ERR_ERROR);
        return;
    }


    TProperties newProps;
    PropValue pVal;

    // bugtraq:url
    std::string propVal = WideToUTF8((LPCTSTR)m_sBugtraqUrl);
    pVal.value = propVal;
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_URL] = pVal;

    // bugtraq:warnifnoissue
    if (m_bWarnIfNoIssue)
        pVal.value = "true";
    else
        pVal.value = "";
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_WARNIFNOISSUE] = pVal;

    // bugtraq:message
    propVal = WideToUTF8((LPCTSTR)m_sBugtraqMessage);
    pVal.value = propVal;
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_MESSAGE] = pVal;

    // bugtraq:label
    propVal = WideToUTF8((LPCTSTR)m_sBugtraqLabel);
    pVal.value = propVal;
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_LABEL] = pVal;

    // bugtraq:number
    int checked = GetCheckedRadioButton(IDC_TEXTRADIO, IDC_NUMERICRADIO);
    if (checked == IDC_NUMERICRADIO)
        pVal.value = "true";
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_NUMBER] = pVal;

    // bugtraq:append
    checked = GetCheckedRadioButton(IDC_TOPRADIO, IDC_BOTTOMRADIO);
    if (checked == IDC_BOTTOMRADIO)
        pVal.value = "true";
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_APPEND] = pVal;

    // bugtraq:logregex
    CString sLogRegex = m_sBugtraqRegex2 + _T("\n") + m_sBugtraqRegex1;
    if (m_sBugtraqRegex1.IsEmpty() && m_sBugtraqRegex2.IsEmpty())
        sLogRegex.Empty();
    propVal = WideToUTF8((LPCTSTR)sLogRegex);
    pVal.value = propVal;
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_LOGREGEX] = pVal;

    // bugtraq:providerparams
    propVal = WideToUTF8((LPCTSTR)m_sProviderParams);
    pVal.value = propVal;
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_PROVIDERPARAMS] = pVal;

    // bugtraq:provideruuid
    propVal = WideToUTF8((LPCTSTR)m_sProviderUUID);
    pVal.value = propVal;
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_PROVIDERUUID] = pVal;

    // bugtraq:provideruuid64
    propVal = WideToUTF8((LPCTSTR)m_sProviderUUID64);
    pVal.value = propVal;
    pVal.remove = (pVal.value.size() == 0);
    newProps[BUGTRAQPROPNAME_PROVIDERUUID64] = pVal;

    m_bChanged = true;

    m_properties = newProps;

    CResizableStandAloneDialog::OnOK();
}

void CEditPropBugtraq::OnSizing(UINT fwSide, LPRECT pRect)
{
    // don't allow the dialog to be changed in height
    switch (fwSide)
    {
    case WMSZ_BOTTOM:
    case WMSZ_BOTTOMLEFT:
    case WMSZ_BOTTOMRIGHT:
        pRect->bottom = pRect->top + m_height;
        break;
    case WMSZ_TOP:
    case WMSZ_TOPLEFT:
    case WMSZ_TOPRIGHT:
        pRect->top = pRect->bottom - m_height;
        break;
    }
    CResizableStandAloneDialog::OnSizing(fwSide, pRect);	
}

void CEditPropBugtraq::OnBnClickedHelp()
{
    OnHelp();
}
