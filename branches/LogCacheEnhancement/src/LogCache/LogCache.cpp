// TortoiseSVN - a Windows shell extension for easy version control

// Copyright (C) 2007-2007 - TortoiseSVN

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
// LogCache.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RootInStream.h"
#include "RootOutStream.h"
#include "StringDictonary.h"
#include "CachedLogInfo.h"
#include "XMLLogReader.h"
#include "XMLLogWriter.h"
#include "XMLLogWriter.h"
#include "CompositeInStream.h"
#include "CompositeOutStream.h"
#include "HighResClock.h"

using namespace LogCache;

void ReadStream (const std::wstring& fileName)
{
	CRootInStream stream (fileName);

	CStringDictionary dictionary;

	stream >> dictionary;
}

void WriteStream (const std::wstring& fileName)
{
	CRootOutStream stream (fileName);

	CStringDictionary dictionary;
	dictionary.Insert ("test");
	dictionary.Insert ("hugo");
	dictionary.Insert ("otto");

	stream << dictionary;
}

void TestXMLIO()
{
	CCachedLogInfo logInfo (L"E:\\temp\\kde.stream");
//	CCachedLogInfo logInfo (L"E:\\temp\\tsvntrunk.stream");
//	logInfo.Load();
//	logInfo.Clear();

	CHighResClock clock1;
	CXMLLogReader::LoadFromXML (L"E:\\temp\\kde.log.xml", logInfo);
	clock1.Stop();

//	CXMLLogReader::LoadFromXML (L"E:\\temp\\tsvntrunk.log.xml", logInfo);
	logInfo.Save();
	logInfo.Clear();

	CHighResClock clock2;
	logInfo.Load();
	clock2.Stop();

	CHighResClock clock3;
	CXMLLogWriter::SaveToXML (L"E:\\temp\\kde.xml.out", logInfo, true);
	clock3.Stop();

//	CXMLLogWriter::SaveToXML (L"E:\\temp\\tsvntrunk.xml.out", logInfo, true);

	Sleep(5000);

	CHighResClock clock4;
	logInfo.Save();
	clock4.Stop();

	CStringA s;
	s.Format ("\nimport: %5.4f  load: %5.4f  export: %5.4f  save: %5.4f\n"
			 , clock1.GetMusecsTaken() / 1e+06
			 , clock2.GetMusecsTaken() / 1e+06
			 , clock3.GetMusecsTaken() / 1e+06
			 , clock4.GetMusecsTaken() / 1e+06);

	printf (s);
}

int _tmain(int argc, _TCHAR* argv[])
{
	WriteStream (L"C:\\temp\\test.stream");
//	ReadStream (L"C:\\temp\\test.stream");

	TestXMLIO();

	return 0;
}

