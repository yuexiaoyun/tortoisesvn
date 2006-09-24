// LogCache.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "RootInStream.h"
#include "RootOutStream.h"
#include "StringDictonary.h"
#include "CachedLogInfo.h"
#include "XMLLogReader.h"

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
	CCachedLogInfo logInfo (L"C:\\temp\\kde.stream");

	CXMLLogReader::LoadFromXML (L"C:\\temp\\kde.log.xml", logInfo);
//	logInfo.SaveAsXML (L"C:\\temp\\tsvntrunk.xml.out");
	logInfo.Save();
}

int _tmain(int argc, _TCHAR* argv[])
{
	WriteStream (L"C:\\temp\\test.stream");
	ReadStream (L"C:\\temp\\test.stream");

	TestXMLIO();

	return 0;
}

