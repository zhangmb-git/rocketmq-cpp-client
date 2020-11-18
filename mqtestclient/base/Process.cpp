#include "Process.h"

int CProcess::startDaemon(const char *pidFile) {
    if (getppid() == 1) return 0;

    int pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) return pid;
    writePidFile(pidFile);

    int fd = open("/dev/null", O_RDWR);
    if (fd != -1) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);

        if (fd > STDERR_FILENO) close(fd);
    }
    return pid;
}

void CProcess::writePidFile(const char *pidFile) {
    char str[32];

    int fd = open(pidFile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
    if (fd < 0) exit(1);
    if (lockf(fd, F_TLOCK, 0) < 0) {
        fprintf(stderr, "Can't lock pid file: %s\n", pidFile);
        exit(0);
    }
    snprintf(str, 32, "%d\n", getpid());
    ssize_t len = strlen(str);
    ssize_t ret = write(fd, str, len);
    if (ret != len) {
        fprintf(stderr, "Can't write pid file: %s\n", pidFile);
        exit(0);
    }
    close(fd);
}

bool CProcess::existPid(const char *pidFile) {
    char buffer[64], *p;
    int otherpid = 0, fd;

    fd = open(pidFile, O_RDONLY, 0);
    if (fd > 0) {
        read(fd, buffer, 64);
        close(fd);
        buffer[63] = 0;
        p = strchr(buffer, '\n');
        if (p != NULL) *p = 0;
        otherpid = atoi(buffer);
    }
    if (otherpid > 0) {
        if (kill(otherpid, 0) != 0) {
            otherpid = 0;
        }
    }
    return !!otherpid;
}

