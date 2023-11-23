#include <iostream>
#include <dlfcn.h>
using namespace std;

int main(int argc,const char* argv[]) { // передаем названия библиотек 
    /* Проверяем кол-во параметров.
    Если чего не так выводим сообщение об ошибке и уходим. */
    if (argc<2) {
        cout << "arg" << endl;
        exit(1);
    }
    int check_lib = 1; // первая библиотке открыта
    /* Пытаемся загрузить библиотеку. */
    cout << "Try to load library lib1 " << argv[1] << endl;
    void *  lib_work = dlopen(argv[1], RTLD_LAZY);
    /* Если чего не так выводим сообщение об ошибке и уходим. */
    if (lib_work == NULL) {
        cout << "Error lib1 =(" << endl;
        exit(2);
    }
    cout << "Ok! Loaded lib1!" << endl;
    /* Пытаемся загрузить функцию библиотеки. */
    int end = 3;
    int action;
    while(cin >> action) {
        if (action == end) {
            return 0;
        }
        if (action == 0) {
            dlclose(lib_work);
            /* Пытаемся загрузить библиотеку. */
            if (check_lib == 1) {
                cout << "Try to load library lib2 " << argv[2] << endl;
                void *  lib_work = dlopen(argv[2], RTLD_LAZY);
                /* Если чего не так выводим сообщение об ошибке и уходим. */
                if (lib_work == NULL) {
                    cout << "Error lib2 =(" << endl;
                    exit(2);
                }
                cout << "Ok! Loaded lib2!" << endl;
                check_lib = 2;
                continue;
            } else {
                cout << "Try to load library lib1 " << argv[1] << endl;
                void *  lib_work = dlopen(argv[1], RTLD_LAZY);
                /* Если чего не так выводим сообщение об ошибке и уходим. */
                if (lib_work == NULL) {
                    cout << "Error lib1 =(" << endl;
                    exit(2);
                }
                cout << "Ok! Loaded lib1!" << endl;
                check_lib = 1;
                continue;
            }
        }else if (action == 1) {
            int A, B;
            cin >> A >> B;
            int (*PrimeCount)(int A, int B) = (int (*)(int, int))dlsym(lib_work, "PrimeCount");
            /* Если чего не так выводим сообщение об ошибке и уходим. */
            if (PrimeCount == NULL) {
                cout << "Error PrimeCount in lib1 =(" << endl;
                exit(3);
            }
            /* Вызываем функцию из библиотеки передаем при этом дискриптор для вывода данных. */
            int prime_count = (*PrimeCount)(A, B);
            cout << prime_count << endl;
        } else if (action == 2) {
            double X;
            cin >> X;
            float (*E)(double X) = (float (*)(double))dlsym(lib_work, "E");
            /* Если чего не так выводим сообщение об ошибке и уходим. */
            if (E == NULL) {
                cout << "Error E in lib1 =(" << endl;
                exit(3);
            }
            /* Вызываем функцию из библиотеки передаем при этом дискриптор для вывода данных. */
            float e = (*E)(X);
            cout << e << endl;
        } else {
            cout << "This action does not exist, check the entered command =(" << endl;
        }
        
    }
    dlclose(lib_work);
}

