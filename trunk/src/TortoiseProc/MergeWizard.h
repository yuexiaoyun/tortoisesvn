// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2007 - TortoiseSVN

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
#include "TSVNPath.h"

#include "MergeWizardStart.h"
#include "MergeWizardTree.h"
#include "MergeWizardRevRange.h"
#include "MergeWizardOptions.h"

// CMergeWizard

class CMergeWizard : public CPropertySheet
{
	DECLARE_DYNAMIC(CMergeWizard)

public:
	CMergeWizard(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CMergeWizard();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();

	CMergeWizardStart				page1;
	CMergeWizardTree				tree;
	CMergeWizardRevRange			revrange;
	CMergeWizardOptions				options;
public:
	CTSVNPath						wcPath;
	CString							url;
	CString							sUUID;
	bool							bRevRangeMerge;

	CString							URL1;
	CString							URL2;
	SVNRev							startRev;
	SVNRev							endRev;
	SVNRevList						revList;
	BOOL							bReverseMerge;

	BOOL							m_bIgnoreAncestry;
	svn_depth_t						m_depth;
	BOOL							m_bIgnoreEOL;
	svn_diff_file_ignore_space_t	m_IgnoreSpaces;

};


