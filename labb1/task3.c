#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <mqueue.h>
#include <sys/fcntl.h>
#include <sys/wait.h>

int main()
{
    const char *name = "/mymq";
    pid_t pid;

    printf("Forking");
    pid = fork();

    if (pid > 0)
    { // parent
        const char *wmsg = "hello";
        mqd_t mqd;
        mqd = mq_open(name, O_WRONLY);

        if (mqd == -1)
        {
            perror("mp_open");
            _exit(1);
        }
        mq_send(mqd, wmsg, strlen(wmsg) + 1, 0) == 0 ? printf("Message sent\n") : printf("Message could not be sent\n");
        wait(NULL);
        // mq_close(mqd);
    }
    else if (pid == 0)
    {
        const char *name = "/mymq";
        const int msg_num_max = 10;
        const int msg_size_max = 128;
        mqd_t mqd;
        struct mq_attr attr;
        char buf[msg_size_max];
        ssize_t n;

        attr.mq_maxmsg = msg_num_max;
        attr.mq_msgsize = msg_size_max;
        mqd = mq_open(name, O_RDONLY | O_CREAT, 0666, &attr);

        if (mqd == -1) {
            perror("mq_open");
            _exit(1);
        }
        // n = mq_receive(mqd, buf, msg_size_max, NULL);
        // printf("Recieved message of len=%ld, msg:%s\n", n, buf);
        mq_close(mqd);
    }

    return 0;
}