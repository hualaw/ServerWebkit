#ifndef _SERVER_WEBKIT_WRAPPER_REQUEST_BUFFER_H__
#define _SERVER_WEBKIT_WRAPPER_REQUEST_BUFFER_H__

#include "SWUtil.h"

class RequestBuffer
{
public:
	RequestBuffer(int len, const char *msg)
		: length(len), data(msg), ptr(msg)
	{ 
	}

	~RequestBuffer()
	{
		length = 0; 
		data = NULL; 
		ptr = NULL;
	}

	char read_char() 
	{ 
		return char(*(ptr++)); 
	}

	uchar read_uchar() 
	{ 
		return *(ptr++); 
	}

	int read_int()
	{
		int value = make_int(ptr, 0);
		ptr += INT_SIZE;
		return value;
	}

	uint read_uint()
	{
		uint value = make_uint(ptr, 0);
		ptr += INT_SIZE;
		return value;
	}

	short read_short()
	{
		short value = make_short(ptr, 0);
		ptr += SHORT_SIZE;
		return value;
	}

	ushort read_ushort()
	{
		ushort value = make_ushort(ptr, 0);
		ptr += SHORT_SIZE;
		return value;
	}

	QString read_string()
	{
		int readBytes = 0;
		QString value = make_string(ptr, 0, length - (ptr - data), readBytes);
		ptr += readBytes;
		return value;
	}

	QString read_header()
	{
		int readBytes = 0;
		QString value = make_header(ptr, 0, length - (ptr - data), readBytes);
		ptr += readBytes;
		return value;
	}

	int get_ptr_size()
	{
		return ptr - data;
	}

private:
	int length;
	char const *data;
	const char *ptr;
};

#endif // _SERVER_WEBKIT_WRAPPER_REQUEST_BUFFER_H__
