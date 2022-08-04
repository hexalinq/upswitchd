#include "pcsl-glibc-stub.h"
#include "linux.h"

typedef struct __attribute__((packed)) {
	uint8_t dDestinationMAC[6];
	uint8_t dSourceMAC[6];
	uint16_t iProtocol;
} eth2_header_t;

#define ETH_BROADCAST_ADDR ((uint8_t[]){ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff })
#define MTU (1500 + sizeof(eth2_header_t))

typedef union {
	uint8_t dData[MTU];
	struct {
		eth2_header_t tEthernet;
		uint8_t dEthernetPayload[MTU - sizeof(eth2_header_t)];
	};
} packet_t;

#define MAC_PRINT_FMT "%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC_PRINT(x) 0[(uint8_t*)x], 1[(uint8_t*)x], 2[(uint8_t*)x], 3[(uint8_t*)x], 4[(uint8_t*)x], 5[(uint8_t*)x]

static void _PrintUsage(const char* sPath) {
	printf("upswitchd " PROGRAM_VERSION_STR "\n");
	printf("Copyright (C) 2022 Hexalinq <info@hexalinq.com>\n");
	printf("The source code is available at <https://github.com/hexalinq/upswitchd>\n");
	printf("\n");

	printf("This program is free software; you can redistribute it and/or modify\n");
	printf("it under the terms of the GNU General Public License version 2 as\n");
	printf("published by the Free Software Foundation.\n");
	printf("\n");

	printf("This program is distributed in the hope that it will be useful,\n");
	printf("but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
	printf("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
	printf("GNU General Public License for more details.\n");
	printf("\n");

	printf("You should have received a copy of the GNU General Public License along\n");
	printf("with this program; if not, write to the Free Software Foundation, Inc.,\n");
	printf("51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n");
	printf("\n");

	printf("\033[1mUsage: %s <interface-1> <interface-2>\033[0m\n", sPath);
}

static void* _ForwarderThread(void* pArg) {
	interface_t* pRX = ((interface_t**)pArg)[0];
	interface_t* pTX = ((interface_t**)pArg)[1];
	packet_t uPacket;
	printf("\"%s\" => \"%s\" ready\n", pRX->sName, pTX->sName);
	for(;;) {
		ssize_t iRead = recv(pRX->hSocket, uPacket.dData, MTU, 0);
		if(iRead <= 0) crash();
		if(iRead < sizeof(eth2_header_t)) {
			printf("Invalid packet received at interface \"%s\"\n", pRX->sName);
			continue;
		}

		if(send(pTX->hSocket, uPacket.dData, iRead, 0) != iRead) crash();
	}
}

int main(int iArgs, char** aArgs) {
	if(iArgs != 3) {
		_PrintUsage(aArgs[0]);
		return 1;
	}

	if(!strcmp(aArgs[1], aArgs[2])) crash("Interface #1 and interface #2 are the same");

	interface_t* pInterface1 = OpenInterface(aArgs[1]);
	printf("Interface #1 name: %s\n", pInterface1->sName);
	printf("Interface #1 file descriptor: %d\n", pInterface1->hSocket);
	printf("Interface #1 MAC address: " MAC_PRINT_FMT "\n", MAC_PRINT(pInterface1->dMAC));
	printf("\n");

	interface_t* pInterface2 = OpenInterface(aArgs[2]);
	printf("Interface #2 name: %s\n", pInterface2->sName);
	printf("Interface #2 file descriptor: %d\n", pInterface2->hSocket);
	printf("Interface #2 MAC address: " MAC_PRINT_FMT "\n", MAC_PRINT(pInterface2->dMAC));
	printf("\n");

	interface_t* aDirection1[] = { pInterface1, pInterface2 };
	interface_t* aDirection2[] = { pInterface2, pInterface1 };
	pthread_t tThread1;
	pthread_t tThread2;
	if(pthread_create(&tThread1, NULL, _ForwarderThread, aDirection1)) crash();
	if(pthread_create(&tThread2, NULL, _ForwarderThread, aDirection2)) crash();
	if(pthread_join(tThread1, NULL)) crash();
	if(pthread_join(tThread2, NULL)) crash();

	CloseInterface(pInterface1);
	CloseInterface(pInterface2);

	return 0;
}
