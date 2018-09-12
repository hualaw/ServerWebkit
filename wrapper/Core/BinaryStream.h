#ifndef _SERVER_WEBKIT_WRAPPER_BINARY_STREAM_H__
#define _SERVER_WEBKIT_WRAPPER_BINARY_STREAM_H__

#include <vector>

typedef std::vector<unsigned char> BinaryBuffer;
class BinaryStream : public BinaryBuffer
{
public:
	void write_char(char c) { push_back(c); }
	void write_uchar(unsigned char c) { push_back(c); }

	void write_int(int i)
    {
	#if defined(BIG_ENDIAN)
		push_back((i>>24) & 0xff);
        push_back((i>>16) & 0xff);
        push_back((i>>8)  & 0xff);
        push_back(i & 0xff);
	#else
        push_back(i & 0xff);
        push_back((i>>8)  & 0xff);
        push_back((i>>16) & 0xff);
        push_back((i>>24) & 0xff);
    #endif
	}

    void set_int(unsigned int pos, int i)
    {
        BinaryStream &self = *this;
        if (pos>=self.size()+4) return;

    #if defined(BIG_ENDIAN)
		self[pos++] = ((i>>24) & 0xff);
		self[pos++] = ((i>>16) & 0xff);
		self[pos++] = ((i>>8)  & 0xff);
		self[pos] = (i & 0xff);
    #else
            self[pos++] = (i & 0xff);
            self[pos++] = ((i>>8)  & 0xff);
            self[pos++] = ((i>>16) & 0xff);
            self[pos] = ((i>>24) & 0xff);
    #endif
    }

    void write_uint(unsigned int i)
    {
	#if defined(BIG_ENDIAN)
        push_back((i>>24) & 0xff);
        push_back((i>>16) & 0xff);
        push_back((i>>8)  & 0xff);
        push_back(i & 0xff);
	#else
        push_back(i & 0xff);
        push_back((i>>8)  & 0xff);
        push_back((i>>16) & 0xff);
        push_back((i>>24) & 0xff);
	#endif
    }

    void set_uint(unsigned int pos, unsigned int i)
    {
        BinaryStream &self = *this;
        if (pos>=self.size()+4) return;

	#if defined(BIG_ENDIAN)
        self[pos++] = ((i>>24) & 0xff);
        self[pos++] = ((i>>16) & 0xff);
        self[pos++] = ((i>>8)  & 0xff);
        self[pos] = (i & 0xff);
	#else
        self[pos++] = (i & 0xff);
        self[pos++] = ((i>>8)  & 0xff);
        self[pos++] = ((i>>16) & 0xff);
        self[pos] = ((i>>24) & 0xff);
	#endif
    }

    void write_short(short s)
    {
    #if defined(BIG_ENDIAN)
        push_back((s>>8)  & 0xff);
        push_back(s & 0xff);
    #else
        push_back(s & 0xff);
        push_back((s>>8)  & 0xff);
    #endif
    }

    void set_short(unsigned int pos, short s)
    {
        BinaryStream &self = *this;
        if (pos>=self.size()+2) return;

    #if defined(BIG_ENDIAN)
        self[pos++] = ((s>>8)  & 0xff);
        self[pos] = (s & 0xff);
    #else
        self[pos++] = (s & 0xff);
        self[pos] = ((s>>8)  & 0xff);
    #endif
    }

    void write_ushort(unsigned short s)
    {
    #if defined(BIG_ENDIAN)
        push_back((s>>8)  & 0xff);
        push_back(s & 0xff);
    #else
        push_back(s & 0xff);
        push_back((s>>8)  & 0xff);
    #endif
    }

    void set_ushort(unsigned int pos, unsigned short s)
    {
        BinaryStream &self = *this;
        if (pos>=self.size()+2) return;

    #if defined(BIG_ENDIAN)
        self[pos++] = ((s>>8)  & 0xff);
        self[pos] = (s & 0xff);
    #else
        self[pos++] = (s & 0xff);
        self[pos] = ((s>>8)  & 0xff);
    #endif
    }

    void write_string(const char *str)
    {
        //CString s = str.utf8();
        //write_ushort(s.length());
        int length = strlen(str);
        write_ushort(length);

        // slow version
        const char *sbuf = str;
        for (unsigned int i=0; i<length; ++i, ++sbuf)
        {
            if (*sbuf== '\n')
                push_back(' ');
            else
                push_back(*sbuf);
        }

        // fast version, to test
        //resize(size()+s.length());
        //std::copy(s.data(), s.data()+s.length(), this->end());
    }

    void write_bytes(unsigned int length, unsigned char const *src)
    {
        // slow version
        for (unsigned int i=0; i<length; ++i, ++src) push_back(*src);
    }

    void write_bytes(char const *src)
    {
        // slow version
        unsigned int length = strlen(src);
        for (unsigned int i=0; i<length; ++i, ++src) push_back(*src);
    }
};

#endif  //_SERVER_WEBKIT_WRAPPER_BINARY_STREAM_H__
