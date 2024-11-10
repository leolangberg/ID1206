#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <mqueue.h>
#include <sys/wait.h>

#define msg_num_max 10
#define msg_size_max 128

int main()
{
    const char *parent_to_child = "/mymq";
    const char *child_to_parent = "/mymq2";
    pid_t pid;

    struct mq_attr attr;
    mqd_t mqd, mqd2;

    attr.mq_maxmsg = msg_num_max;
    attr.mq_msgsize = msg_size_max;

    printf("Forking. \n");
    pid = fork();

    if (pid > 0)
    { // parent
        const char *wmsg = "hello";
        char buf[msg_size_max];
        ssize_t recievedMsgFromChild;

        // Create parent to child message queue
        mqd = mq_open(parent_to_child, O_WRONLY);
        if (mqd == -1)
        {
            perror("mp_open");
            _exit(1);
        }

        // Create child to parent message queue
        mqd2 = mq_open(child_to_parent, O_RDONLY | O_CREAT, 0666, &attr);
        if (mqd2 == -1)
        {
            perror("mq_open child_to_parent");
            _exit(1);
        }

        // Send message to child
        mq_send(mqd, wmsg, strlen(wmsg) + 1, 0) == 0 ? printf("Message sent from parent\n") : printf("Message could not be sent\n");


        // Wait for message from child
        recievedMsgFromChild = mq_receive(mqd2, buf, sizeof(buf), NULL);
        printf("Received message from child of len=%ld, msg:%s\n", recievedMsgFromChild, buf);

        wait(NULL);

        // Clean up
        mq_close(mqd);
        mq_close(mqd2);
        mq_unlink(parent_to_child);
        mq_unlink(child_to_parent);
    }
    else if (pid == 0)
    {
        char buf[msg_size_max];
        ssize_t recievedMsgFromParent;

        // Open the parent-to-child message queue
        mqd = mq_open(parent_to_child, O_RDONLY | O_CREAT, 0666, &attr);
        if (mqd == -1)
        {
            // perror("mq_open");
            _exit(1);
        }

        // Open the child-to-parent message queue
        mqd2 = mq_open(child_to_parent, O_WRONLY);
        if (mqd2 == -1)
        {
            // perror("mq_open child_to_parent");
             _exit(1);
        }

        // Receive message from parent
        recievedMsgFromParent = mq_receive(mqd, buf, msg_size_max, NULL);
        printf("Received message of len=%ld, msg:%s\n", recievedMsgFromParent, buf);

        // Send acknowledgment to parent
        const char *wmsg2 = "Hello from Child";
        mq_send(mqd2, wmsg2, strlen(wmsg2) + 1, 0) == 0 ? printf("Message sent from child\n") : printf("Message could not be sent\n");
        
        

        // Clean up
        // mq_close(mqd);
        // mq_close(mqd2);

    }

    return 0;
}