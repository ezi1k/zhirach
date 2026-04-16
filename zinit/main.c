#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

static volatile sig_atomic_t stop = 0;
static const char *scripts_path = "/etc/init.z/*";
static const char *shell_path = "/bin/zshell";

void handle_signal(int sig) {
    (void)sig;
    stop = 1;
}

pid_t run_process(const char *path, char *const argv[]) {
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        setsid();
        signal(SIGTERM, SIG_DFL);
        signal(SIGINT, SIG_DFL);
        execv(path, argv);
        _exit(127);
    }
    return pid;
}

int main(void) {
    pid_t pid_init = -1;
    pid_t pid_shell = -1;

    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    char *shell_args[] = {(char *)shell_path, "-i", NULL};
    char *init_args[] = {"/bin/sh", "-c", (char *)scripts_path, NULL};

    while (!stop) {
        if (pid_init <= 0) pid_init = run_process("/bin/sh", init_args);
        if (pid_shell <= 0) pid_shell = run_process(shell_path, shell_args);

        int status;
        pid_t w = waitpid(-1, &status, WNOHANG);

        if (w > 0) {
            if (w == pid_init) pid_init = -1;
            if (w == pid_shell) pid_shell = -1;
        }

        if (stop) break;
        usleep(200000);
    }

    if (pid_init > 0) kill(pid_init, SIGTERM);
    if (pid_shell > 0) kill(pid_shell, SIGTERM);

    return 0;
}



