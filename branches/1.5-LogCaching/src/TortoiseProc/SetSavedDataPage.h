// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2003-2006 - Stefan Kueng

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
#pragma once
#include "afxwin.h"
#include "Balloon.h"


/**
 * \ingroup TortoiseProc
 * Setting page to clear various saved/cached data.
 */
class CSetSavedDataPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSetSavedDataPage)

public:
	CSetSavedDataPage();
	virtual ~CSetSavedDataPage();

	/**
	 * Saves the changed settings to the registry.
	 * returns 0 if no restart is needed for the changes to take effect
	 * \remark If the dialog is closed/dismissed without calling
	 * this method first then all settings the user made must be
	 * discarded!
	 */
	int SaveData() {return 0;}

	UINT GetIconID() {return IDI_SAVEDDATA;}

	enum { IDD = IDD_SETTINGSSAVEDDATA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedUrlhistclear();
	afx_msg void OnBnClickedLoghistclear();
	afx_msg void OnBnClickedResizablehistclear();
	afx_msg void OnBnClickedAuthhistclear();

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
private:
	CBalloon		m_tooltips;
	CButton			m_btnUrlHistClear;
	CButton			m_btnLogHistClear;
	CButton			m_btnResizableHistClear;
	CButton			m_btnAuthHistClear;
};
