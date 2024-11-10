
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>



// Master process reads fifo and creates new forks that all
// write onto this fork. Master then prints entire message.
int main(void) {
        pid_t pid;
        const char *fifo1 = "/tmp/fifo1";
        const int buf_size = 256;
        char rbuf[buf_size];
        int fd;

        printf("Make fifo 1.\n");
        mkfifo(fifo1, 0666);

        pid = fork();

        if(pid == -1) {
                 perror("fork() failed.");
                _exit(1);
        }
        if(pid > 0) 
        {
            // Parent process.
            printf("This is the parent process of pid=%d\n", pid);
            const char *wbuf = "Hello from the other side.";
            fd = open(fifo1, O_WRONLY);
            write(fd, wbuf, strlen(wbuf)+1);
            wait(NULL);
    
            //close(fd);
            unlink(fifo1);
        }
        else if(pid == 0) {
            // pid = 0 meaning child process.
            printf("This is the child process. (pid=%d)\n", pid);
            fd = open(fifo1, O_RDONLY);
            read(fd, rbuf, buf_size);
            close(fd);
            printf("Recieved: \"%s\"\n", rbuf);
            _exit(0);
        }

        return 0;
    }

