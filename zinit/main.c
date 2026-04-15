#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

static volatile sig_atomic_t stop = 0;
static const char *cmd = "/etc/init.z/*";

void term(int s){ (void)s; stop = 1; }

int main(void){
    pid_t child = -1;
    signal(SIGTERM, term);
    signal(SIGINT, term);
    while(!stop){
        if ((child = fork()) < 0){
            sleep(1);
            continue;
        }
        if (child == 0){
            signal(SIGTERM, SIG_DFL);
            execlp("sh","sh","-c",cmd,(char*)NULL);
            _exit(127);
        }
        int status;
        while(!stop){
            pid_t w = waitpid(child, &status, 0);
            if (w == child) break;
            if (w < 0) break;
        }
        if (!stop){
            sleep(1);
        } else {
            kill(child, SIGTERM);
            sleep(1);
            kill(child, SIGKILL);
            waitpid(child, NULL, 0);
        }
    }
    return 0;
}


