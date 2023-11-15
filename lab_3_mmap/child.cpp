#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <iostream>
#include <sys/stat.h>
#include <semaphore.h>


#include <sys/ipc.h>
#include <sys/sem.h>
#define MAX_LIMIT 100000
#define EMPTY 0

using namespace std;
int main (int argc, char* argv[]) {
    int file_parent_to_child = open(argv[0], O_RDWR | O_APPEND|O_CREAT, 0644);
    int file_child_to_parent = open(argv[1], O_RDWR | O_APPEND|O_CREAT, 0644);
    int file_out = open (argv[2], O_WRONLY | O_APPEND | O_CREAT, 0644); //файл для вывода сток прошедщих проверку
    char* pipe1 = (char*) mmap(0, MAX_LIMIT, PROT_READ, MAP_SHARED, file_parent_to_child, 0); 
    char* pipe2 = (char*) mmap(0, MAX_LIMIT, PROT_WRITE, MAP_SHARED, file_child_to_parent, 0);
    int size_str = 0;
    int file_size = 0;
    int data_len = 0;
    int data_len_pipe = 0;
    sem_t* semaphore = sem_open(argv[3], O_CREAT, 0666, 2);
    int state = 0;

    while (true) {
        int getvalue_error = sem_getvalue(semaphore, &state);
        if (getvalue_error == -1) {
            printf("sem_value");
            return -1;
        }
        if (state == 1){
            char str[MAX_LIMIT]; 
            for (int k = 0; pipe1[k + data_len] != '\n'; k++) {
                str[k] = pipe1[k + data_len];
            }
            size_str = strlen(str);
            data_len += size_str + 1;
            if (str[size_str-1] == '.') {
                write (file_out, str, size_str * sizeof(char));
                write (file_out, "\n", sizeof(char));
            } else {
                data_len_pipe += size_str + 1;
                file_size += sizeof(char)*(size_str + 1);
                ftruncate(file_child_to_parent, file_size);
                
                for (int i = data_len_pipe - 1 - size_str; i < data_len_pipe; i++) {
                    pipe2[i] = str[i - data_len_pipe + size_str + 1];
                }
                pipe2[data_len_pipe] = '\n';
                if (strcmp(str, "end") == 0) {
                    sem_wait(semaphore);
                    getvalue_error = sem_getvalue(semaphore, &state);
                    close(file_out);
                    break;
                }
            }
        }
    }
}