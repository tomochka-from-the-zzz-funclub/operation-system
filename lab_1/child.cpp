//#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define MAX_LIMIT 100000

using namespace std;
int main (int argc, char* argv[]) {
    int file_desc = open(argv[0], O_WRONLY | O_APPEND|O_CREAT, 0644);
    int size_str;
    char str[MAX_LIMIT]; 
    while (read(0, &size_str, sizeof(int))) {
        read(0, str, size_str * sizeof(char));
        if (str[size_str-1] == '.') {
            write (file_desc, str, size_str * sizeof(char));
            write (file_desc, "\n", sizeof(char));
        } else {
            write (1, &size_str, sizeof(int));
            write (1, str, size_str * sizeof(char));
        }
    }
}