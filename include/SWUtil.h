#ifndef _SERVER_WEBKIT_WRAPPER_UTIL_H__
#define _SERVER_WEBKIT_WRAPPER_UTIL_H__

#define BIG_ENDIAN

const static int BYTE_BITS = 8;
const static int INT_SIZE = sizeof(int);
const static int SHORT_SIZE = 2;

int make_int(const char *pData, int offset)
{
	int value = 0;
	const char* ptr = pData + offset;
	for (int i = 0; i < INT_SIZE; i++)
	{
#if defined (BIG_ENDIAN)
		value |= (int(*(ptr + i)) & 0xFF) << BYTE_BITS * (INT_SIZE - i - 1);
#else
		value |= (int(*(ptr + i)) & 0xFF) << BYTE_BITS * i;
#endif
	}

	return value;
}

uint make_uint(const char *pData, int offset)
{
	uint value = 0;
	const char* ptr = pData + offset;
	for (int i = 0; i < INT_SIZE; i++)
	{
#if defined (BIG_ENDIAN)
		value |= (uint(*(ptr + i)) & 0xFF) << BYTE_BITS * (INT_SIZE - i - 1);
#else
		value |= (uint(*(ptr + i)) & 0xFF) << BYTE_BITS * i;
#endif
	}

	return value;
}

char make_char(const char *pData, int offset)
{
	return *(pData + offset);
}

unsigned char make_uchar(const char *pData, int offset) 
{ 
	return (unsigned char)(*(pData + offset)); 
}

short make_short(const char *pData, int offset)
{
	short value = 0;
	const char* ptr = pData + offset;
	for (int i = 0; i < SHORT_SIZE; i++)
	{
#if defined (BIG_ENDIAN)
		value |= (uint(*(ptr + i)) & 0xFF) << BYTE_BITS * (SHORT_SIZE - i - 1);
#else
		value |= (uint(*(ptr + i)) & 0xFF) << BYTE_BITS * i;
#endif
	}

	return value;
}

ushort make_ushort(const char *pData, int offset)
{
	ushort value = 0;
	const char* ptr = pData + offset;

	for (int i = 0; i < SHORT_SIZE; i++)
	{
#if defined (BIG_ENDIAN)
		value |= (ushort(*(ptr + i)) & 0xFF) << BYTE_BITS * (INT_SIZE - i - 1);
#else
		value |= (ushort(*(ptr + i)) & 0xFF) << BYTE_BITS * i;
#endif
	}

	return value;
}

QString make_string(const char *pData, int offset, int limit, int &readBytes)
{
	const char* ptr = pData + offset;
	short len = make_short(ptr, 0);
	if (len < 0 || len + offset > limit)
	{
		return QString::null;
	}
	else if(len == 0)
    {
        readBytes = SHORT_SIZE;
		return QString::null;
    }
    else
	{
		QString str = QString::fromUtf8(ptr + SHORT_SIZE, len);
		readBytes = SHORT_SIZE + len;
		return str;
	}
}

QString make_header(const char *pData, int offset, int limit, int &readBytes)
{
	const char* ptr = pData + offset;
	int len = make_int(ptr, 0);
	if (len < 0 || len + offset > limit)
	{
		return QString::null;
	}
    else if( len == 0 )
    {
        readBytes = SHORT_SIZE;
		return QString::null;
    }
	else
	{
		QString str = QString::fromUtf8(ptr + INT_SIZE, len);
		readBytes = INT_SIZE + len;
		return str;
	}
}

#endif // _SERVER_WEBKIT_WRAPPER_UTIL_H__

