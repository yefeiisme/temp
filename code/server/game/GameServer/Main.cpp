#include "stdafx.h"
#include "GameServer.h"

int main(int argc, char *argv[])
{
	if (!g_pGameServer.Initialize())
	{
#ifdef WIN32
		puts("press any key to exit ...");
		getchar();
#endif
		return 0;
	}

	g_pGameServer.Run();

#ifdef WIN32
	puts("press any key to exit ...");
	getchar();
#endif
}
