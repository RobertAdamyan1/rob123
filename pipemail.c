#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>

#define MAX_MSG_LEN 200
#define FIFO_BASE "/tmp/mailbox_"
#define SEM_BASE "/mailsem_"

char username[50];
sem_t *sem;
int fifo_fd;

void cleanup(int signum) {
    close(fifo_fd);
    char fifo_path[100];
    snprintf(fifo_path, sizeof(fifo_path), "%s%s", FIFO_BASE, username);
    unlink(fifo_path);
    sem_close(sem);
    char sem_name[100];
    snprintf(sem_name, sizeof(sem_name), "%s%s", SEM_BASE, username);
    sem_unlink(sem_name);
    exit(0);
}

void receive_mode(const char *user) {
    strcpy(username, user);
    char fifo_path[100], sem_name[100];
    
    snprintf(fifo_path, sizeof(fifo_path), "%s%s", FIFO_BASE, username);
    mkfifo(fifo_path, 0666);
    
    snprintf(sem_name, sizeof(sem_name), "%s%s", SEM_BASE, username);
    sem = sem_open(sem_name, O_CREAT, 0644, 1);
    
    signal(SIGINT, cleanup);
    fifo_fd = open(fifo_path, O_RDONLY);
    
    char buffer[MAX_MSG_LEN + 100];
    while(1) {
        if(read(fifo_fd, buffer, sizeof(buffer)) > 0) {
            printf("%s\n", buffer);
        }
    }
}

void send_message() {
    char recipient[50], message[MAX_MSG_LEN];
    printf("Recipient: ");
    scanf("%49s", recipient);
    
    while(getchar() != '\n');
    printf("Message: ");
    fgets(message, MAX_MSG_LEN, stdin);
    message[strcspn(message, "\n")] = '\0';
    
    char sem_name[100];
    snprintf(sem_name, sizeof(sem_name), "%s%s", SEM_BASE, recipient);
    sem_t *recipient_sem = sem_open(sem_name, 0);
    
    sem_wait(recipient_sem);
    char fifo_path[100];
    snprintf(fifo_path, sizeof(fifo_path), "%s%s", FIFO_BASE, recipient);
    
    int fd = open(fifo_path, O_WRONLY);
    time_t now = time(NULL);
    char *timestamp = ctime(&now);
    timestamp[strlen(timestamp)-1] = '\0';
    
    char full_msg[300];
    snprintf(full_msg, sizeof(full_msg), "[%s] From %s: %s", timestamp, username, message);
    write(fd, full_msg, strlen(full_msg)+1);
    
    close(fd);
    sem_post(recipient_sem);
    sem_close(recipient_sem);
}

int main(int argc, char *argv[]) {
    if(argc == 3 && strcmp(argv[1], "--receive") == 0) {
        receive_mode(argv[2]);
    } else {
        printf("Your username: ");
        scanf("%49s", username);
        
        int choice;
        do {
            printf("\n1. Send message\n2. Exit\nChoice: ");
            scanf("%d", &choice);
            
            if(choice == 1) send_message();
            else if(choice != 2) printf("Invalid choice\n");
        } while(choice != 2);
    }
    return 0;
}
