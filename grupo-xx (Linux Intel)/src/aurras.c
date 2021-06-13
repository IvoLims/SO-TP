#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void status() {
    char *pid = itoa(getpid());

    char pid_ler[strlen(pid)+2];
    char pid_escrever[strlen(pid)+2];
    pid_ler[0] = 'r';
    pid_escrever[0] = 'w';
    strcpy(pid_ler+1,pid);
    strcpy(pid_escrever+1,pid);

    if (mkfifo(pid_ler, 0666) == 0 && mkfifo(pid_escrever, 0666) == 0) {
        int pipe_ler = open(pid_ler, O_RDONLY);
        int pipe_escrever = open(pid_escrever, O_WRONLY);

        int pipePrincipal = open("main", O_WRONLY);
        write(pipePrincipal, pid, strlen(pid));
        close(pipePrincipal);

        write(pid_escrever, "status", 6);
        close(pid_escrever);

        char buffer;
        while (read(pid_ler, &buffer, 1) > 0) write(1, &buffer, 1);

        close(pid_ler);
        unlink(pid_ler);
        unlink(pid_escrever);
    } 
}

void usr1_handler(int signum) {
    write(1, "processing\n", 11);
}

void usr2_handler(int signum) {
    _exit(0);
}

void transform(int argc, char **argv) {
    char *pid = itoa(getpid());
    char pid_escrever[strlen(pid)+2];

    pid_escrever[0] = 'w';
    strcpy(pid_escrever+1,pid);

    if (mkfifo(pid_escrever, 0666) == 0) {
        int pipe_escrever = open(pid_escrever, O_WRONLY);

        int pipePrincipal = open("main", O_WRONLY);
        write(pipePrincipal, pid, strlen(pid));
        close(pipePrincipal);

        for (int i = 1; i < argc; i++) {
            write(pid_escrever, argv[i], strlen(argv[i]));
            write(pid_escrever, " ", 1);
        }
        close(pid_escrever);
        unlink(pid_escrever);

        signal(SIGUSR1, usr1_handler);
        signal(SIGUSR2, usr2_handler);

        write(1, "pending\n", 8);

        while(1) pause();
    }
}

int main(int argc, char **argv) {
    if (argc >= 2 && !strcmp(argv[1], "status")) {
        status();
    } else if (argc >= 5 && !strcmp(argv[1], "transform")) {
        transform(argc, argv);
    } else {
        write(1,"./aurras status\n", 16);
        write(1,"./aurras transform input-filename output-filename filter-id-1 filter-id-2 ...\n",78);
    }
    return 0;
}
