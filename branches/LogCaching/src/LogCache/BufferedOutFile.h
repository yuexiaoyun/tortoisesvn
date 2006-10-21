#pragma once

///////////////////////////////////////////////////////////////
//
// CBufferedOutFile
//
//		class that provides a simple, buffered file write stream.
//		Future enhancements may include async. file I/O.
//
///////////////////////////////////////////////////////////////

class CBufferedOutFile
{
private:

	// the file

	HANDLE file;

	// our local buffer

	enum {BUFFER_SIZE = 1024*1024};

	std::auto_ptr<unsigned char> buffer;
	DWORD used;

	// physical file size + used

	size_t fileSize;

protected:

	// write buffer content to disk

	void Flush();

public:

	// construction / destruction: auto- open/close

	CBufferedOutFile (const std::wstring& fileName);
	virtual ~CBufferedOutFile();

	// write data to file

	void Add (const unsigned char* data, DWORD bytes);
	void Add (const char* data, DWORD bytes);
	void Add (DWORD value);

	// file properties

	size_t GetFileSize() const;
	bool IsOpen() const;
};

///////////////////////////////////////////////////////////////
// write data to file
///////////////////////////////////////////////////////////////

inline void CBufferedOutFile::Add (const char* data, DWORD bytes)
{
	Add (reinterpret_cast<const unsigned char*>(data), bytes);
}

inline void CBufferedOutFile::Add (DWORD value)
{
	Add ((unsigned char*)&value, sizeof (value));
}

///////////////////////////////////////////////////////////////
// file properties
///////////////////////////////////////////////////////////////

inline size_t CBufferedOutFile::GetFileSize() const
{
	return fileSize;
}

inline bool CBufferedOutFile::IsOpen() const
{
	return file != INVALID_HANDLE_VALUE;
}

///////////////////////////////////////////////////////////////
// file stream operation
///////////////////////////////////////////////////////////////

CBufferedOutFile& operator<< (CBufferedOutFile& dest, int value);

inline CBufferedOutFile& operator<< (CBufferedOutFile& dest, const char* value)
{
	dest.Add (value, (DWORD)strlen (value));
	return dest;
}

inline CBufferedOutFile& operator<< (CBufferedOutFile& dest, const std::string& value)
{
	dest.Add (value.c_str(), (DWORD)value.length());
	return dest;
}
