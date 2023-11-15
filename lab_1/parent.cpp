#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define MAX_LIMIT 100000

using namespace std;


int main() {  
    int parent_to_child[2]; //pipe1
  int child_to_parent[2]; //pipe2
    if (pipe(parent_to_child) == -1) {
        printf("pipe1");
    return -1;
    }
    if (pipe(child_to_parent) == -1) {
        printf("pipe2");
    return -1;
    }
    char file_name[MAX_LIMIT];

    printf("Enter the file name\n");
    fgets(file_name, MAX_LIMIT, stdin);
    char str[MAX_LIMIT]; 
    char str_err[MAX_LIMIT]; 
    int process_id = fork();
    if (process_id == -1) {
        printf("fork");
        return -1;
    } else if (process_id > 0) {
        close(parent_to_child[0]);
        close(child_to_parent[1]);
        printf ("Enter string\n");
        char end [MAX_LIMIT] ="end";
        while (scanf("%s", str)){
            int size_str = strlen(str);
            write (parent_to_child[1], &size_str, sizeof(int));
            write (parent_to_child[1], str, size_str * sizeof(char));
            if (strcmp(str, end) == 0){
                break;
            } 
        } 

        int size_str_err;
        while (read(child_to_parent[0], &size_str_err, sizeof(int))) {
            memset(str_err, '\0', sizeof(str_err));
            read (child_to_parent[0], str_err, size_str_err * sizeof(char));
            printf("The string does not pass the checking: %s \n", str_err);
             if (strcmp(str_err, end) == 0) {
            return 0;
            }
        }
        close (parent_to_child[1]);
        close (child_to_parent[0]);
    } else {
        close (parent_to_child[1]);
        close (child_to_parent[0]);
        dup2(parent_to_child[0], STDIN_FILENO);
        dup2(child_to_parent[1], STDOUT_FILENO);
        execl("son", file_name, NULL);
    }
    return 0;
}