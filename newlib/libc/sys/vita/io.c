#include <fcntl.h>
#include <sys/unistd.h>

extern char _newlib_fd_mutex[];
extern int fd_to_scefd[];

void _init_vita_io(void) {
  int ret;

  sceKernelCreateLwMutex(_newlib_fd_mutex, "fd conversion table mutex", 0, 0, 0);

  sceKernelLockLwMutex(_newlib_fd_mutex, 1, 0);
  ret = sceIoOpen("tty0:", O_RDONLY, 0);
  if (ret >= 0) {
    fd_to_scefd[STDIN_FILENO] = ret;
  }
  ret = sceIoOpen("tty0:", O_WRONLY, 0);
  if (ret >= 0) {
    fd_to_scefd[STDOUT_FILENO] = ret;
  }
  ret = sceIoOpen("tty0:", O_WRONLY, 0);
  if (ret >= 0) {
    fd_to_scefd[STDERR_FILENO] = ret;
  }
  sceKernelUnlockLwMutex(_newlib_fd_mutex, 1);
}

void _free_vita_io(void) {
  sceKernelLockLwMutex(_newlib_fd_mutex, 1, 0);
  if (fd_to_scefd[STDIN_FILENO]) {
    sceIoClose(fd_to_scefd[STDIN_FILENO]);
    fd_to_scefd[STDIN_FILENO] = 0;
  }
  if (fd_to_scefd[STDOUT_FILENO]) {
    sceIoClose(fd_to_scefd[STDOUT_FILENO]);
    fd_to_scefd[STDOUT_FILENO] = 0;
  }
  if (fd_to_scefd[STDERR_FILENO]) {
    sceIoClose(fd_to_scefd[STDERR_FILENO]);
    fd_to_scefd[STDERR_FILENO] = 0;
  }
  sceKernelUnlockLwMutex(_newlib_fd_mutex, 1);

  sceKernelDeleteLwMutex(_newlib_fd_mutex);
}
