#pragma once
#include "pcsl-glibc-stub.h"

typedef struct {
	int hSocket;
	char sName[IFNAMSIZ + 1];
	uint8_t dMAC[6];
	struct sockaddr_ll tAddress;
} interface_t;

interface_t* OpenInterface(const char* sInterface);
void CloseInterface(interface_t* this);
