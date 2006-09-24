#pragma once

// all currently available stream types

enum 
{
	ROOT_STREAM_TYPE_ID             = 0,
	
	COMPOSITE_STREAM_TYPE_ID        = 0x10,

	BLOB_STREAM_TYPE_ID				= 0x20,

	BINARY_STREAM_TYPE_ID			= 0x30,
	PACKED_DWORD_STREAM_TYPE_ID		= 0x31,
	PACKED_INTEGER_STREAM_TYPE_ID	= 0x32,
	DIFF_INTEGER_STREAM_TYPE_ID     = 0x33
};

