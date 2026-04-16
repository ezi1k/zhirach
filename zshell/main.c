#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    char cmd[512], *args[64];

    while (printf("$ ") && fgets(cmd, sizeof(cmd), stdin)) {
        cmd[strcspn(cmd, "\n")] = 0;

        int i = 0;
        args[i++] = strtok(cmd, " \t");
        while (i < 63 && (args[i++] = strtok(NULL, " \t")));

        if (!args[0]) continue;
        if (!strcmp(args[0], "exit")) break;
        if (!strcmp(args[0], "cd")) {
            chdir(args[1] ? args[1] : getenv("HOME"));
            continue;
        }

        if (fork() == 0) {
            execvp(args[0], args);
            perror(args[0]);
            exit(1);
        }
        wait(NULL);
    }
    return 0;
}

