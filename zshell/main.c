#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MAX_ARGS 64
#define MAX_CMD 512

static void drain_stdin(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

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

        if (strchr(cmd, '\n') == NULL && !feof(stdin)) {
            fprintf(stderr, "warning: input truncated\n");
            drain_stdin();
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
        if (arg_count >= MAX_ARGS - 1) {
            fprintf(stderr, "warning: too many arguments (max %d), extras ignored\n", MAX_ARGS - 1);
        }
        args[arg_count] = NULL;
        if (arg_count == 0) continue;

        if (strcmp(args[0], "exit") == 0) break;

        if (strcmp(args[0], "cd") == 0) {
            const char *dir = (arg_count > 1) ? args[1] : getenv("HOME");
            if (!dir) {
                fprintf(stderr, "cd: HOME not set\n");
                continue;
            }
            if (chdir(dir) != 0) {
                perror("cd");
            }
            continue;
        }

        pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror(args[0]);
            _exit(127);
        } else if (pid > 0) {
            int status;
            pid_t w;
            do {
                w = waitpid(pid, &status, 0);
            } while (w == -1 && errno == EINTR);

            if (w == -1) {
                perror("waitpid");
            } else {
                if (WIFSIGNALED(status)) {
                    fprintf(stderr, "child terminated by signal %d%s\n",
                            WTERMSIG(status),
#ifdef WCOREDUMP
                            WCOREDUMP(status) ? " (core dumped)" : ""
#else
                            ""
#endif
                    );
                }
            }
        } else {
            perror("fork");
        }
    }

    return 0;
}


