#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 64
#define MAX_CMD 512

int main(void) {
    char cmd[MAX_CMD];
    char *args[MAX_ARGS];
    pid_t pid;

    while (1) {
        if (printf("$ ") < 0) break;
        fflush(stdout);

        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            if (feof(stdin)) break;
            continue;
        }

        cmd[strcspn(cmd, "\n")] = '\0';
        if (cmd[0] == '\0') continue;

        int arg_count = 0;
        char *p = cmd;
        while (*p && arg_count < MAX_ARGS - 1) {
            while (*p == ' ' || *p == '\t') p++;
            if (!*p) break;
            args[arg_count++] = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p++ = '\0';
        }
        args[arg_count] = NULL;
        if (arg_count == 0) continue;

        if (strcmp(args[0], "exit") == 0) break;
        if (strcmp(args[0], "cd") == 0) {
            chdir(arg_count > 1 ? args[1] : getenv("HOME"));
            continue;
        }

        pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            _exit(127);
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);
        } else {
            perror("fork");
        }
    }

    return 0;
}

