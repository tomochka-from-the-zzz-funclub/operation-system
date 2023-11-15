#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

using namespace std;
int main() {
    char file_name [1000];
    fgets(file_name, 1000, stdin);
    cout << "\\"<<file_name <<  "\\"<< endl;
    int file = open("e", O_RDWR | O_APPEND, 0666);
    cerr << "Error code: " << strerror(errno); 
    cout << file <<endl;
    char* mapp = (char*) mmap(0, 10, PROT_READ | PROT_WRITE , MAP_SHARED, file, 0); 
    cout << mapp << endl;
    cout << int('5') << endl; 
    // ftruncate(file, 10);
    // for (int i = 0; i < 10; i++) {
    //     mapp[i] = '0';
    //     cout << mapp[i] << "";
    //}
    //msync(mapp, 10, MS_INVALIDATE);
    munmap(mapp, 10);
    close(file);
}