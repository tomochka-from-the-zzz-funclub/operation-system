#include <iostream>
#include <cmath>
#include <malloc.h>
//#include <mutex>
//#include <thread>
#include <chrono>
using namespace std;

long double default_value_of_variable = 1;

typedef struct{
	int rowSize;//размер строк
    int rowStr; // количество строк
    long double* free_val;
    long double* result;
    int* strings; //номера строк для обработки в этом потоке
    int len_strings;
	//указатели на матрицу
	long double** matrix; 
} pthrData;

void print_matrix (long double** matrix, long double* free_value, int n, int m) {
    cout << "===============print_matrix===============" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m+1; j++) {
            if (j == m) {
                cout << free_value[i];
                continue;
            }
            cout << matrix[i][j] << " ";
        }
        cout << "\n";
    }
    cout << "===========================================" << endl;
}

void* straight_stroke (void* thread_data) {
    pthrData* data = (pthrData*) thread_data;
    for (int i = 0; i < data->len_strings; i++) {
        if(data->matrix[data->strings[i]][data->strings[i]] == 0) {
            for (int i = data->strings[i] + 1; i < data->strings[i]; i++) {
                if(data->matrix[i][data->strings[i]] == 0) {
                    continue;
                }
                long double* test = new long double [data->rowSize];
                for (int k = 0; k < data->rowSize; k++) {
                    test[k] = data->matrix[data->strings[i]][k];
                }
                for (int j = 0; j < data->rowSize; j++) {
                    data->matrix[data->strings[i]][j] = data->matrix[i][j];
                }
                for (int k = 0; k < data->rowSize; k++) {
                    data->matrix[i][k] = test[k];
                }
                long double test_free = data->free_val[i];
                data->free_val[i] = data->free_val[data->strings[i]];
                data->free_val[data->strings[i]] = test_free;
                break;
            }
        }
        //cout << "============="<< endl;
        for(int k = data->strings[i]+1; k < data->rowStr; k++) {
            long double mnoj = -data->matrix[k][data->strings[i]] / data->matrix[data->strings[i]][data->strings[i]];
            for (int j = 0; j < data->rowSize; j++) {
                data->matrix[k][j] += data->matrix[data->strings[i]][j]*mnoj;
                //cout << i << " " << k << " " << j << " " << mnoj << endl;
            }
            data->free_val[k] += data->free_val[data->strings[i]]*mnoj;
        }
    }
    return NULL;
}
void* reverse_stroke (void* thred_data) {
    pthrData* data = (pthrData*) thred_data;
    for (int i = data->len_strings; i > 0; i-- ){
        data->result[data->strings[i]] -=default_value_of_variable;
        if (data->rowSize - data->strings[i] - 1 > 0) {
            for (int v = data->strings[i]+1; v < data->rowStr; v++) {
                data->free_val[data->strings[i]] -= data->result[v] * data->matrix[data->strings[i]][v];
                data->matrix[data->strings[i]][v] = 0;
            }
        }
        if(data->matrix[data->strings[i]][data->strings[i]] == 0){
            data->result[data->strings[i]] = default_value_of_variable;
            return NULL;
        } 
        data->result[data->strings[i]] = data->free_val[data->strings[i]] / data->matrix[data->strings[i]][data->strings[i]];
    }
    return NULL;
}



pthrData* data_in_thread (int str, int quantiti_thread, int column, long double* free_val, long double* result, long double** matrix) {
    //cout << str <<  " " << quantiti_thread<< endl; 
    pthrData* thread_data = new pthrData [quantiti_thread];
    for (int i =0; i < quantiti_thread; i++){
        thread_data[i].free_val = free_val;
        thread_data[i].result = result;
        thread_data[i].matrix = matrix;
        thread_data[i].rowSize = column;
        thread_data[i].rowStr = str;
    }
    //cout << "1\n";
    int big = str%quantiti_thread; // gпоследний большой массив
    for (int i = 0; i < quantiti_thread; i++) {
         if (i < big) {
            thread_data[i].strings = new int [str/quantiti_thread+((str%quantiti_thread)/big)];
            thread_data[i].len_strings = str/quantiti_thread+((str%quantiti_thread)/big);
            continue;
        }
        thread_data[i].strings = new int [str/quantiti_thread];
        thread_data[i].len_strings = str/quantiti_thread;
    }
    //cout <<"nhfff" << big << " " << str/quantiti_thread << " " << str%quantiti_thread <<" " <<str/quantiti_thread+((str%quantiti_thread)/big)<< endl;
    for (int i = 0; i < str; i++) {
        //cout << i << endl;
        int num_thread = i % quantiti_thread; // номер потока для массива (индекс в массиве данных)
        //cout << i << " " << num_thread << endl;

        thread_data[num_thread].strings[i/quantiti_thread] = i;
        //cout << i << " " << num_thread << endl;
    }
    //  for (int i = 0; i < quantiti_thread; i++) {
    //     if (i < big) {
    //         for (int j = 0; j < str/quantiti_thread+(str%quantiti_thread)/big; j++) {
    //             cout << thread_data[i].str[j] << " ";
    //         }
    //         cout << "\n";
    //         continue;
    //     }
    //     for (int j = 0; j < str/quantiti_thread; j++) {
    //         cout << thread_data[i].str[j] << " ";
    //     }
    //     cout << "\n";
    // }
    return thread_data;
}


int main() {
    long int str, column; //количество строк и столбцов в матрице
    cout << "Введите количество уравнений в системе" << endl;
    cin >> str;
    cout << "Введите количество перменных" << endl;
    cin >> column;
    long double* free_value = new long double [str];
    long double** matrix = new long double* [str];
    for (int i = 0; i < str; i++){
        matrix[i] = new long double [column];
    }
    //вводим матрицу
    for (int i = 0; i < str; i++) {
        for (int j = 0; j < column+1; j++) {
            if (j == column){
                cin >> free_value[i];
                continue;
            }
            cin >> matrix[i][j];
        }
    }

    int quantity_thread;
    cout << "Введите количество потоков" << endl;
    cin >> quantity_thread; 

    if (quantity_thread > str -1) {
        cout << "Количество потоков больше, чем максимально возможное для системы" << endl;
        quantity_thread = str;
    } 
    auto start = chrono::steady_clock::now();

    long double* result = new long double [column];
    for (int i = 0; i < str; i++){
        result[i] = default_value_of_variable;
    }

    pthread_t* threads =  new  pthread_t [quantity_thread];
    // массив данных для потоков
    pthrData* thread_data =  data_in_thread (str, quantity_thread, column, free_value, result, matrix);
    // прямой ход - исключение переменных
    for(int i = 0; i < quantity_thread; i++) {
        pthread_create(&threads[i], NULL, straight_stroke, &thread_data[i]);
        //print_matrix (matrix, free_value, str, column);
        //cout  << "1 "<< i%quantity_thread << endl;
    }

    for(int i = 0; i < quantity_thread; i++) {
        //ждем завершения потока
	pthread_join(threads[i], NULL);
    }

    //print_matrix (matrix, free_value, str, column);
    // обратный ход - выражение неизвестных
    //long double* result = new long double [column];
    for (int i = 0; i < quantity_thread; i++) {
        pthread_create(&threads[i], NULL, reverse_stroke, &thread_data[i]);
        //print_matrix (matrix, free_value, str, column);
        //cout <<"2 " <<i%quantity_thread << endl;
    }
    for(int i = 0; i < quantity_thread; i++) {
        //ждем завершения потока
	pthread_join(threads[i], NULL);
    }
    cout << "Решение системы:" << endl;
    for (int i = 0; i < column; i++) {
        cout << result[i] << " ";
    }
    auto end = chrono::steady_clock::now();
    cout << "\n Время выполнения: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
    return 0;
}