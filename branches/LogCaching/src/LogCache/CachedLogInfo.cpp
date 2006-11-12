#include "StdAfx.h"
#include ".\cachedloginfo.h"

#include "RootInStream.h"
#include "RootOutStream.h"

// construction / destruction (nothing to do)

CCachedLogInfo::CCachedLogInfo (const std::wstring& aFileName)
	: skippedRevisions (logInfo.GetPaths(), revisions)
	, fileName (aFileName)
	, modified (false)
	, revisionAdded (false)
{
}

CCachedLogInfo::~CCachedLogInfo (void)
{
}

// cache persistency

void CCachedLogInfo::Load()
{
	assert (revisions.GetLastRevision() == 0);

	CRootInStream stream (fileName);

	// read the data

	IHierarchicalInStream* revisionsStream
		= stream.GetSubStream (REVISIONS_STREAM_ID);
	*revisionsStream >> revisions;

	IHierarchicalInStream* logInfoStream
		= stream.GetSubStream (LOG_INFO_STREAM_ID);
	*logInfoStream >> logInfo;

	IHierarchicalInStream* skipRevisionsStream
		= stream.GetSubStream (SKIP_REVISIONS_STREAM_ID);
	*skipRevisionsStream >> skippedRevisions;
}

void CCachedLogInfo::Save (const std::wstring& newFileName)
{
	CRootOutStream stream (newFileName);

	// write the data

	IHierarchicalOutStream* revisionsStream
		= stream.OpenSubStream (REVISIONS_STREAM_ID, COMPOSITE_STREAM_TYPE_ID);
	*revisionsStream << revisions;

	IHierarchicalOutStream* logInfoStream
		= stream.OpenSubStream (LOG_INFO_STREAM_ID, COMPOSITE_STREAM_TYPE_ID);
	*logInfoStream << logInfo;

	IHierarchicalOutStream* skipRevisionsStream
		= stream.OpenSubStream (SKIP_REVISIONS_STREAM_ID, COMPOSITE_STREAM_TYPE_ID);
	*skipRevisionsStream << skippedRevisions;

	// all fine -> connect to the new file name

	fileName = newFileName;
}

// data modification (mirrors CRevisionInfoContainer)

void CCachedLogInfo::Insert ( size_t revision
							 , const std::string& author
							 , const std::string& comment
							 , __time64_t timeStamp)
{
	// there will be a modification

	modified = true;

	// add entry to cache and update the revision index

	DWORD index = (DWORD)logInfo.Insert (author, comment, timeStamp);
	revisions.SetRevisionIndex (revision, index);

	// you may call AddChange() now

	revisionAdded = true;
}

void CCachedLogInfo::AddSkipRange ( const std::string& path
								  , DWORD startRevision
								  , DWORD count)
{
	CDictionaryBasedPath dictPath (&logInfo.GetPaths(), path);

	// if the path is unknown, don't add it

	if (dictPath.GetPath() == path)
		skippedRevisions.Add (dictPath, startRevision, count);
}

void CCachedLogInfo::Clear()
{
	modified = revisions.GetLastRevision() != 0;
	revisionAdded = false;

	revisions.Clear();
	logInfo.Clear();
}
