#include "stdafx.h"
#include "CenterServer.h"

int main(int argc, char *argv[])
{
#if defined(WIN32) || defined(WIN64)
	bool	bDaemon = false;
#else
	bool	bDaemon = (0 == strcmp("-d", argv[0]));
#endif

	if (!g_pCenterServer.Initialize(bDaemon))
	{
#if defined(WIN32) || defined(WIN64)
		puts("press any key to exit ...");
		getchar();
#endif
		return 0;
	}

	g_pCenterServer.Run();

#if defined(WIN32) || defined(WIN64)
	puts("press any key to exit ...");
	getchar();
#endif
}
