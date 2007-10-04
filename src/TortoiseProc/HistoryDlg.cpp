// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2007 - TortoiseSVN

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
#include "Registry.h"
#include ".\historydlg.h"


IMPLEMENT_DYNAMIC(CHistoryDlg, CResizableStandAloneDialog)
CHistoryDlg::CHistoryDlg(CWnd* pParent /*=NULL*/)
	: CResizableStandAloneDialog(CHistoryDlg::IDD, pParent)
{
}

CHistoryDlg::~CHistoryDlg()
{
}

void CHistoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CResizableStandAloneDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HISTORYLIST, m_List);
}


BEGIN_MESSAGE_MAP(CHistoryDlg, CResizableStandAloneDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_LBN_DBLCLK(IDC_HISTORYLIST, OnLbnDblclkHistorylist)
END_MESSAGE_MAP()


void CHistoryDlg::OnBnClickedOk()
{
	int pos = m_List.GetCurSel();
	if (pos != LB_ERR)
	{
		m_SelectedText = m_history.GetEntry(pos);
	}
	else
		m_SelectedText.Empty();
	OnOK();
}

BOOL CHistoryDlg::OnInitDialog()
{
	CResizableStandAloneDialog::OnInitDialog();

	// calculate and set listbox width
	CDC* pDC=m_List.GetDC();
	CSize itemExtent;
	int horizExtent = 1;
	for (int i=0; i<(int)m_history.GetCount(); ++i)
	{
		CString sEntry = m_history.GetEntry(i);
		sEntry.Replace(_T("\r"), _T(""));
		sEntry.Replace('\n', ' ');
		m_List.AddString(sEntry);
		itemExtent=pDC->GetTextExtent(sEntry);
		horizExtent=max(horizExtent, itemExtent.cx+5);
	}
	m_List.SetHorizontalExtent(horizExtent);
	ReleaseDC(pDC); 

	AddAnchor(IDC_HISTORYLIST, TOP_LEFT, BOTTOM_RIGHT);
	AddAnchor(IDOK, BOTTOM_RIGHT);
	AddAnchor(IDCANCEL, BOTTOM_RIGHT);
	EnableSaveRestore(_T("HistoryDlg"));
	m_List.SetFocus();
	return FALSE;
}

void CHistoryDlg::OnLbnDblclkHistorylist()
{
	int pos = m_List.GetCurSel();
	if (pos != LB_ERR)
	{
		m_SelectedText = m_history.GetEntry(pos);
		OnOK();
	}
	else
		m_SelectedText.Empty();
}
