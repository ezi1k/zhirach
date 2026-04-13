#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

    char *argv[] = {"sh", "-c", "/etc/zinit/enabled/*", NULL};
    char *envp[] = {NULL};

int main()
{
  pid_t pid1 = getpid();
  //printf("%d\n", pid1);
  fork();
  pid_t pid2 = getpid();
  //printf("%d\n", pid2);
  if (pid1 == pid2)
  {
  }
  else
  {
  execve("/bin/sh", argv, envp);
  }
  printf("done");
    while (true) {sleep(120); }
}
