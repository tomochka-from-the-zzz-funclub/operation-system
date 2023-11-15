//Тренировочноя работа с отработкой простых программ с потоками
// реализация работы с массивом си в многопоточном режиме (увеличение каждого элемента на 1, с эмуляцией сложных времязатратных процессов ) 


#include <thread>
#include <vector>
#include <iostream>
#include <chrono>


using namespace std;

void* func_plus(void* data_array) {
    this_thread::sleep_for(chrono::milliseconds(1000));
    int *array = (int*) data_array;
    (*array)++;
    cout << this_thread::get_id() << endl;
    return NULL;
}


int main() {
    int n;
    cout << "Введите количество элементов массива \n";
    cin >> n;

    vector <int> array (n);
    for (int i = 0; i < n; i++) {
        cin >> array[i];
    }
    cout << this_thread::get_id() << endl;
    pthread_t* thread_id = (pthread_t*)malloc(n * sizeof(pthread_t*));
    for(int i = 0; i < n; i++) {
        pthread_create(&thread_id[i], NULL, func_plus, &array[i]);
    }
	
    for(int i = 0; i < n; i++) {
        //ждем завершения потока
	pthread_join(thread_id[i], NULL);
    }

    for (int i = 0; i < n; i++) {
        cout << array[i] << " ";
    }
    cout << "\n";
    return 0;
    }