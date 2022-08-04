#pragma once
#include "pcsl-glibc-stub.h"

typedef struct {
	int hSocket;
	char sName[IFNAMSIZ + 1];
	struct sockaddr_ll tAddress;
} interface_t;

interface_t* OpenInterface(const char* sInterface);
void CloseInterface(interface_t* this);
