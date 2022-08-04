#include "linux.h"

static pcsl_error_t _GetInterfaceIndex(int* pID, char sInterface[IFNAMSIZ]) {
	struct ifreq tReq = {};
	mmCopy(tReq.ifr_name, sInterface, IFNAMSIZ);

	int hSocket = socket(AF_PACKET, SOCK_RAW, 0);
	if(hSocket < 0) return PCSL_ERROR_SYS;

	int iStatus = ioctl(hSocket, SIOCGIFINDEX, &tReq);
	close(hSocket);
	if(iStatus < 0) return PCSL_ERROR_SYS;

	*pID = tReq.ifr_ifindex;
	return 0;
}

static pcsl_error_t _GetInterfaceMAC(uint8_t pMAC[6], char sInterface[IFNAMSIZ]) {
	struct ifreq tReq = {};
	mmCopy(tReq.ifr_name, sInterface, IFNAMSIZ);

	int hSocket = socket(AF_PACKET, SOCK_RAW, 0);
	if(hSocket < 0) return PCSL_ERROR_SYS;

	int iStatus = ioctl(hSocket, SIOCGIFHWADDR, &tReq);
	close(hSocket);
	if(iStatus < 0) return PCSL_ERROR_SYS;

	if(tReq.ifr_hwaddr.sa_family != ARPHRD_ETHER) return PCSL_ERROR_VALUE;
	mmCopy(pMAC, tReq.ifr_hwaddr.sa_data, 6);
	return 0;
}

static pcsl_error_t _SetPromiscuousMode(char sInterface[IFNAMSIZ], bool bEnabled) {
	struct ifreq tReq = {};
	mmCopy(tReq.ifr_name, sInterface, IFNAMSIZ);

	int hSocket = socket(AF_PACKET, SOCK_RAW, 0);
	if(hSocket < 0) return PCSL_ERROR_SYS;

	if(ioctl(hSocket, SIOCGIFFLAGS, &tReq)) {
		close(hSocket);
		return PCSL_ERROR_SYS;
	}

	if(bEnabled) tReq.ifr_flags |= IFF_PROMISC;
	else tReq.ifr_flags &= ~IFF_PROMISC;

	if(ioctl(hSocket, SIOCSIFFLAGS, &tReq)) {
		close(hSocket);
		return PCSL_ERROR_SYS;
	}

	close(hSocket);
	return 0;
}

interface_t* OpenInterface(const char* sInterface) {
	mmAllocateZeroTS(interface_t, this);
	if(!this) crash("Out of memory");

	size_t iNameLength = strlen(sInterface);
	if(!iNameLength || iNameLength > IFNAMSIZ) crash("Invalid interface name");
	mmCopy(this->sName, sInterface, iNameLength);

	this->tAddress.sll_family = AF_PACKET;
	if(_GetInterfaceIndex(&this->tAddress.sll_ifindex, this->sName))
		crash("Failed to look up index of interface \"%s\"", this->sName);

	if(_GetInterfaceMAC(this->dMAC, this->sName))
		crash("Failed to look up the MAC address of interface \"%s\"", this->sName);

	this->hSocket = socket(AF_PACKET, SOCK_RAW, htonl(ETH_P_ALL));
	if(this->hSocket < 0) crash("Failed to create socket for interface \"%s\"", this->sName);

	if(bind(this->hSocket, (struct sockaddr*)&this->tAddress, sizeof(this->tAddress)))
		crash("Failed to bind socket to interface \"%s\"", this->sName);

	if(_SetPromiscuousMode(this->sName, TRUE))
		crash("Failed to enable promiscuous mode on interface \"%s\"", this->sName);

	return this;

	error:
	mmFree(this);
	return NULL;
}

void CloseInterface(interface_t* this) {
	close(this->hSocket);
	mmFree(this);
}
