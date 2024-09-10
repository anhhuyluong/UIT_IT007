#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 100

typedef struct Node {
    char command[MAX_COMMAND_LENGTH];
    struct Node* next;
} Node;

Node* history = NULL;
Node* currentHistory = NULL;

void addToHistory(const char* command) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->command, command);
    newNode->next = history;
    history = newNode;
    currentHistory = history;
}

void displayHistory() {
    if (currentHistory == NULL) {
        printf("No command in history.\n");
    } else {
        printf("Recent command: %s\n", currentHistory->command);
        currentHistory = currentHistory->next;
    }
}

void freeHistory() {
    while (history != NULL) {
        Node* temp = history;
        history = history->next;
        free(temp);
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];

    while (1) {
        printf("Shell> ");
        fgets(command, MAX_COMMAND_LENGTH, stdin);

        // Loại bỏ ký tự newline từ chuỗi
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "HF") == 0) {
            // Hiển thị lịch sử khi người dùng nhập 'HF'
            displayHistory();
        } else {
            // Thực thi lệnh trong tiến trình con
            addToHistory(command);

            pid_t pid = fork();

            if (pid == 0) {
                // Tiến trình con
                execlp(command, command, (char *)NULL);
                exit(0);
            } else {
                // Tiến trình cha đợi tiến trình con kết thúc
                wait(NULL);
            }
        }
    }

    // Giải phóng bộ nhớ khi kết thúc chương trình
    freeHistory();

    return 0;
}
