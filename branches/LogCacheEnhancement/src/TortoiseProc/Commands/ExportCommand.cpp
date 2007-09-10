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
#include "StdAfx.h"
#include "ExportCommand.h"

#include "ExportDlg.h"
#include "SVNProgressDlg.h"
#include "BrowseFolder.h"
#include "MessageBox.h"
#include "SVNStatus.h"

bool ExportCommand::Execute()
{
	// When the user clicked on a working copy, we know that the export should
	// be done from that. We then have to ask where the export should go to.
	// If however the user clicked on an unversioned folder, we assume that
	// this is where the export should go to and have to ask from where
	// the export should be done from.
	TCHAR saveto[MAX_PATH];
	bool bURL = !!SVN::PathIsURL(cmdLinePath.GetSVNPathString());
	if ((bURL)||(SVNStatus::GetAllStatus(cmdLinePath) == svn_wc_status_unversioned))
	{
		// ask from where the export has to be done
		CExportDlg dlg;
		if (bURL)
			dlg.m_URL = cmdLinePath.GetSVNPathString();
		else
			dlg.m_strExportDirectory = cmdLinePath.GetWinPathString();
		if (parser.HasKey(_T("revision")))
		{
			LONG lRev = parser.GetLongVal(_T("revision"));
			dlg.Revision = lRev;
		}	
		if (dlg.DoModal() == IDOK)
		{
			CTSVNPath exportPath(dlg.m_strExportDirectory);

			CSVNProgressDlg progDlg;
			progDlg.m_dwCloseOnEnd = parser.GetLongVal(_T("closeonend"));
			theApp.m_pMainWnd = &progDlg;
			int options = dlg.m_bNoExternals ? ProgOptIgnoreExternals : 0;
			if (dlg.m_eolStyle.CompareNoCase(_T("CRLF"))==0)
				options |= ProgOptEolCRLF;
			if (dlg.m_eolStyle.CompareNoCase(_T("CR"))==0)
				options |= ProgOptEolCR;
			if (dlg.m_eolStyle.CompareNoCase(_T("LF"))==0)
				options |= ProgOptEolLF;
			progDlg.SetParams(CSVNProgressDlg::SVNProgress_Export, options, CTSVNPathList(exportPath), dlg.m_URL, _T(""), dlg.Revision);
			progDlg.SetDepth(dlg.m_depth);
			progDlg.DoModal();
		}
	}
	else
	{
		// ask where the export should go to.
		CBrowseFolder folderBrowser;
		CString strTemp;
		strTemp.LoadString(IDS_PROC_EXPORT_1);
		folderBrowser.SetInfo(strTemp);
		folderBrowser.m_style = BIF_NEWDIALOGSTYLE | BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS | BIF_VALIDATE;
		strTemp.LoadString(IDS_PROC_EXPORT_2);
		folderBrowser.SetCheckBoxText(strTemp);
		strTemp.LoadString(IDS_PROC_OMMITEXTERNALS);
		folderBrowser.SetCheckBoxText2(strTemp);
		folderBrowser.DisableCheckBox2WhenCheckbox1IsEnabled(true);
		CRegDWORD regExtended = CRegDWORD(_T("Software\\TortoiseSVN\\ExportExtended"), FALSE);
		CBrowseFolder::m_bCheck = regExtended;
		if (folderBrowser.Show(hwndExplorer, saveto, MAX_PATH)==CBrowseFolder::OK)
		{
			CString saveplace = CString(saveto);
			saveplace += _T("\\") + cmdLinePath.GetFileOrDirectoryName();
			TRACE(_T("export %s to %s\n"), (LPCTSTR)cmdLinePath.GetUIPathString(), (LPCTSTR)saveto);
			SVN svn;
			if (!svn.Export(cmdLinePath, CTSVNPath(saveplace), bURL ? SVNRev::REV_HEAD : SVNRev::REV_WC, 
				bURL ? SVNRev::REV_HEAD : SVNRev::REV_WC, FALSE, folderBrowser.m_bCheck2, svn_depth_infinity,
				hwndExplorer, folderBrowser.m_bCheck))
			{
				CMessageBox::Show(hwndExplorer, svn.GetLastErrorMessage(), _T("TortoiseSVN"), MB_OK | MB_ICONERROR);
			}
			else
			{
				CString strMessage;
				strMessage.Format(IDS_PROC_EXPORT_4, (LPCTSTR)cmdLinePath.GetUIPathString(), (LPCTSTR)saveplace);
				CMessageBox::Show(hwndExplorer, strMessage, _T("TortoiseSVN"), MB_OK | MB_ICONINFORMATION);
			}
			regExtended = CBrowseFolder::m_bCheck;
		}
	}
	return true;
}