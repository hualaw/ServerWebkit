#ifndef _SERVER_WEBKIT_WRAPPER_RESPONSE_H__
#define _SERVER_WEBKIT_WRAPPER_RESPONSE_H__

typedef struct {
	int flag;
	std::vector<unsigned char> data;
} SWResponse;

#endif // _SERVER_WEBKIT_WRAPPER_RESPONSE_H__
