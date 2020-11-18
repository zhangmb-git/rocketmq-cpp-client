#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

class CProcess {
public:
	static int startDaemon(const char *pidFile);
	static bool existPid(const char *pidFile);
	static void writePidFile(const char *pidFile);
};

#endif
