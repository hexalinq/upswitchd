#include "pcsl-glibc-stub.h"
#include "linux.h"

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

int main(int iArgs, char** aArgs) {
	if(iArgs != 3) {
		_PrintUsage(aArgs[0]);
		return 1;
	}

	if(!strcmp(aArgs[1], aArgs[2])) crash("Interface #1 and interface #2 are the same");

	interface_t* pInterface1 = OpenInterface(aArgs[1]);
	printf("Interface #1 (%s) file descriptor: %d\n", pInterface1->sName, pInterface1->hSocket);

	interface_t* pInterface2 = OpenInterface(aArgs[2]);
	printf("Interface #2 (%s) file descriptor: %d\n", pInterface2->sName, pInterface2->hSocket);

	CloseInterface(pInterface1);
	CloseInterface(pInterface2);

	return 0;
}
