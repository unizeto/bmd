/** 
@file

This file contains information about network mechanisms implemented by default 

*/

#ifndef _NET_MECHANISMS_H
#define _NET_MECHANISMS_H

#include <bmd/libbmdnet/libbmdnet.h>

extern const bmdnet_method_t __bmdnet_method_PLAINTCP;
extern const bmdnet_method_t __bmdnet_method_CONNECT_PROXY;
extern const bmdnet_method_t __bmdnet_method_SOCKS4;
extern const bmdnet_method_t __bmdnet_method_SOCKS4A;
extern const bmdnet_method_t __bmdnet_method_SOCKS5;

#define BMDNET_LOAD_DEFAULT_MECHANISMS() { \
	bmdnet_register_method(__bmdnet_method_PLAINTCP); \
	bmdnet_register_method(__bmdnet_method_CONNECT_PROXY); \
	bmdnet_register_method(__bmdnet_method_SOCKS4); \
	bmdnet_register_method(__bmdnet_method_SOCKS4A); \
	bmdnet_register_method(__bmdnet_method_SOCKS5); \
};

#endif
