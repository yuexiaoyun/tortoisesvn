#pragma once

class CStatusCacheEntry
{
public:
	CStatusCacheEntry();
	CStatusCacheEntry(const svn_wc_status_t* pSVNStatus, __int64 lastWriteTime);
	bool HasExpired(long now) const;
	const svn_wc_status_t* Status() const;
	bool IsVersioned() const;
	bool DoesFileTimeMatch(__int64 testTime) const;
	bool ForceStatus(svn_wc_status_kind forcedStatus);
	svn_wc_status_kind GetEffectiveStatus() const { return m_highestPriorityLocalStatus; }

private:
	void SetAsUnversioned();

private:
	long				m_discardAtTime;
	svn_wc_status_kind	m_highestPriorityLocalStatus;
	svn_wc_status_t		m_svnStatus;
	__int64				m_lastWriteTime;

//	friend class CSVNStatusCache;
};
