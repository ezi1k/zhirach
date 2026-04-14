#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

    char *argv[] = {"sh", "-c", "/etc/init.z/*", NULL};
    char *envp[] = {NULL};

    char *argv2[] = {"sh", NULL};

int main()
{
  pid_t pid1 = getpid();
  fork();
  pid_t pid2 = getpid();
  if (pid1 == pid2)
  {
    execve("/bin/sh", argv2, envp);
  }
  else
  {
  execve("/bin/sh", argv, envp);
  }
  printf("done");
    while (true) {sleep(120); }
}
