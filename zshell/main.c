#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>
#include <signal.h>

#define MAX_HISTORY 10
#define BUF_SIZE 512

char *path_color = "\033[38;5;142m"; 
char *int_color  = "\033[38;5;167m"; 
char *reset      = "\033[0m";

char history[MAX_HISTORY][BUF_SIZE];
int hist_count = 0;

void apply_theme(const char *name) {
    if (strcmp(name, "default") == 0) {
        path_color = "\033[1;32m"; int_color = "\033[1;31m";
    } else if (strcmp(name, "ocean") == 0) {
        path_color = "\033[38;5;38m"; int_color = "\033[38;5;204m";
    } else if (strcmp(name, "gruvbox") == 0) {
        path_color = "\033[38;5;142m"; int_color = "\033[38;5;167m";
    } else if (strcmp(name, "monokai") == 0) {
        path_color = "\033[38;5;197m"; int_color = "\033[38;5;190m";
    } else if (strcmp(name, "dracula") == 0) {
        path_color = "\033[38;5;141m"; int_color = "\033[38;5;84m";
    } else if (strcmp(name, "nord") == 0) {
        path_color = "\033[38;5;110m"; int_color = "\033[38;5;111m";
    }
}

void load_config() {
    FILE *f = fopen(".zshellrc", "r");
    if (!f) {
        f = fopen(".zshellrc", "w");
        if (f) {
            fprintf(f, "theme=gruvbox\n# theme=ocean\n# theme=monokai\n# theme=dracula\n# theme=nord\n# theme=default\n");
            fclose(f);
        }
        apply_theme("gruvbox");
        return; 
    }

    char line[BUF_SIZE];
    while (fgets(line, sizeof(line), f)) {
        char *ptr = line;
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '#' || *ptr == '\n' || *ptr == '\r' || *ptr == '\0') continue;
        ptr[strcspn(ptr, "\n\r")] = 0;
        if (strncmp(ptr, "theme=", 6) == 0) {
            apply_theme(ptr + 6);
        }
    }
    fclose(f);
}

void handle_sigint(int sig) {
    (void)sig;
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        printf("\n%s%s%s %sInterrupt%s $> ", path_color, cwd, reset, int_color, reset);
    }
    fflush(stdout);
}

void get_input(char *buf, size_t size) {
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    int pos = 0, current_hist = hist_count;
    memset(buf, 0, size);

    while (1) {
        int c = getchar();
        if (c == '\n' || c == EOF) {
            buf[pos] = '\0';
            printf("\n");
            break;
        } else if (c == 27) {
            if (getchar() == '[') {
                char dir = getchar();
                if (dir == 'A' || dir == 'B') {
                    while(pos > 0) { printf("\b \b"); pos--; }
                    if (dir == 'A' && current_hist > 0) current_hist--;
                    else if (dir == 'B' && current_hist < hist_count) current_hist++;
                    if (current_hist < hist_count) strncpy(buf, history[current_hist % MAX_HISTORY], size - 1);
                    else buf[0] = '\0';
                    pos = strlen(buf);
                    printf("%s", buf);
                }
            }
        } else if (c == 127 || c == 8) {
            if (pos > 0) {
                pos--; buf[pos] = '\0';
                printf("\b \b");
            }
        } else if (c >= 32 && c <= 126) {
            if (pos < (int)size - 1) {
                buf[pos++] = (char)c;
                putchar(c);
            }
        }
        fflush(stdout);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

int main(void) {
    load_config();

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    char cmd[BUF_SIZE], *args[64], cwd[1024];

    while (1) {
        if (getcwd(cwd, sizeof(cwd))) {
            printf("%s%s%s $> ", path_color, cwd, reset);
            fflush(stdout);
        }

        get_input(cmd, sizeof(cmd));
        char *trimmed = cmd;
        while (*trimmed == ' ') trimmed++;
        if (*trimmed == '\0') continue;

        strncpy(history[hist_count % MAX_HISTORY], trimmed, BUF_SIZE - 1);
        hist_count++;

        char temp_cmd[BUF_SIZE];
        strncpy(temp_cmd, trimmed, BUF_SIZE - 1);
        int i = 0;
        args[i] = strtok(temp_cmd, " \t\r\n");
        while (args[i] != NULL && i < 62) {
            args[++i] = strtok(NULL, " \t\r\n");
        }
        args[i] = NULL;

        if (!args[0]) continue;
        if (strcmp(args[0], "exit") == 0) break;
        if (strcmp(args[0], "reload") == 0) { load_config(); continue; }
        if (strcmp(args[0], "cd") == 0) {
            if (chdir(args[1] ? args[1] : getenv("HOME")) != 0) perror("cd");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            signal(SIGINT, SIG_DFL);
            execvp(args[0], args);
            perror(args[0]);
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            waitpid(pid, NULL, 0);
        }
    }
    return 0;
}

