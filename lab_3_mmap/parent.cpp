#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
//#include <sys/types.h>
#include <sys/stat.h>

#include <sys/ipc.h>
#include <sys/sem.h>



#define EMPTY 0
#define MAX_LIMIT 100

using namespace std;
/*мы считываем строки из стандартного ввода, отправляем их в файл  parent_to_child и создаем для него мапу, записываем сначала длину, потом саму строку. 
В экзкл передадим его имя и откроем файл на чтение в дочернем процессе и создадим для него мапу на чтение. В дочернем процессе откроем файл для вывода строк, 
прошедших проверку file_name_right,и там же создадим файл child_to_parent 
*/
int main() {  
    char file_name_parent_to_child [MAX_LIMIT] = "parent_to_child.txt"; //  передаем строчки в дочерний процесс
    char file_name_child_to_parent [MAX_LIMIT] = "child_to_parent.txt"; // принимаем строчки из дочернего
    char file_out[MAX_LIMIT] = "result.txt";
    int file_parent_to_child = open (file_name_parent_to_child, O_RDWR | O_APPEND | O_CREAT, 0777);
    int file_child_to_parent = open (file_name_child_to_parent, O_RDWR | O_APPEND | O_CREAT, 0777);
    char* pipe1 = (char*) mmap(0, 100, PROT_WRITE , MAP_SHARED, file_parent_to_child, 0); 
    char* pipe2 = (char*) mmap(0, MAX_LIMIT, PROT_READ, MAP_SHARED, file_child_to_parent, 0);  

    char name_sem [MAX_LIMIT] = "parent_child";
    

    int process_id = fork();
    if (process_id == -1) {
        printf("fork");
        return -1;
    }
    if (process_id > 0) {
        char str[MAX_LIMIT];  
        printf ("Enter strings\n");
        char end [MAX_LIMIT] ="end";
        int file_size = 0;
        int data_len_in_char = 0;
        int size_str_err = 0 ;
        int ofset = 0; //смещаемся для чтения следующей строки
        int status_pipe_2 = 0;
        sem_unlink(name_sem);
        sem_t* semaphore = sem_open(name_sem, O_CREAT, 0666, 2);
        int state = 2;
        int getvalue_error = sem_getvalue(semaphore, &state);


        if (getvalue_error == -1) {
            printf("sem_value");
            return -1;
        }
        if (state == 2) {
            while (true){ //считываем из стандартного потка
                scanf("%s", str);
                int size_str = strlen(str);
                file_size += sizeof(char)*(size_str+1);
                data_len_in_char += size_str + 1; 
                int t = ftruncate(file_parent_to_child, file_size);
                for (int i = data_len_in_char - size_str - 1; i < data_len_in_char; i++) {
                    pipe1[i] = str[i - data_len_in_char + size_str + 1];
                }
                //cout << str << endl;
                pipe1[data_len_in_char] = '\n';
                if (strcmp(str, end) == 0){
                    sem_wait(semaphore);
                    getvalue_error = sem_getvalue(semaphore, &state);
                    break;
                } 
            }
        }
        data_len_in_char = 0;
        while (state != 0) {
            getvalue_error = sem_getvalue(semaphore, &state);
        }
        //cout << state << " p\n";
        if (state == 0){ 
            while(true){
                char str_err[MAX_LIMIT];
                for (int k = 0; pipe2[k + data_len_in_char] != '\n'; k++) {
                    str_err[k] = pipe2[k + data_len_in_char];
                    //cout << k << " " << k + data_len_in_char << str_err[k] << endl;
                }
                int size = strlen(str_err);
                data_len_in_char += size + 1;
                //cout << str_err << " " << data_len_in_char << "p\n";
                if (strcmp(str_err, end) == 0) {
                    //cout << end << endl;
                    munmap(pipe1, data_len_in_char);
                    munmap(pipe2, data_len_in_char);
                    close(file_parent_to_child);
                    close(file_child_to_parent);
                    sem_unlink(name_sem);
                    return 0;
                }
                printf("The string does not pass the checking: %s \n", str_err);
            }
        }
        
    } else {
        execl("son", file_name_parent_to_child, file_name_child_to_parent, file_out, name_sem, NULL);
    }
}