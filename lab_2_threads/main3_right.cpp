#include <iostream>
#include <cmath>
#include <malloc.h>
#include <mutex>
#include <chrono>
using namespace std;

long double default_value_of_variable = 1;

typedef struct{
	int num_elem; //номер строки ведущего элемента
    int work_str; //номер рабочей строки 
	int num_column; //размер строки
    int num_str;
    int num_threads;
    long double* free_val;
    long double* result;
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
    for (int i = data->work_str; i < data->num_str; i+=data->num_threads) {
        long double mnoj = -data->matrix[i][data->num_elem] / data->matrix[data->num_elem][data->num_elem];
        for (int t = 0; t < data->num_column; t++) {
            data->matrix[i][t] += data->matrix[data->num_elem][t]*mnoj;
        }
        data->free_val[i] += data->free_val[data->num_elem]*mnoj;
    }
    return NULL;
}

int main() {
    int str, column;
    cout << "Введите количество уравнений в системе" << endl;
    cin >> str;
    cout << "Введите количество перменных" << endl;
    cin >> column;
    long double* free_value = new long double [str];
    long double** matrix = new long double* [str];
    for (int i = 0; i < str; i++){
        matrix[i] = new long double [column];
    }
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

    if (quantity_thread > str - 1) {
        cout << "Количество потоков больше, чем максимально возможное для системы" << endl;
        quantity_thread = str - 1;
    } 
    auto start = chrono::steady_clock::now();
    pthread_t* threads =  new  pthread_t [quantity_thread];
    pthrData* thread_data =  new  pthrData [quantity_thread];
    
    // прямой ход - исключение переменных
    for (int i = 0; i < str; i++) {
        if (matrix[i][i] == 0) {
            for (int i = i + 1; i < str; i++) {
                if (matrix[i][i] == 0) {
                    continue;
                }
                long double* test = new long double [column];
                for (int k = 0; k < column; k++) {
                    test[k] = matrix[i][k];
                }
                for (int j = 0; j < column; j++) {
                    matrix[i][j] = matrix[i][j];
                }
                for (int k = 0; k < column; k++) {
                    matrix[i][k] = test[k];
                }
                long double test_free = free_value[i];
                free_value[i] = free_value[i];
                free_value[i]= test_free;
                delete test;
                break;
            }
        }
            for(int j = 0; j < quantity_thread; j++) {
                thread_data[j].num_elem = i;
                thread_data[j].work_str = i+j+1; 
                thread_data[j].num_column = column;
                thread_data[j].num_str = str;
                thread_data[j].free_val = free_value;
                thread_data[j].matrix = matrix;
                thread_data[j].num_threads = quantity_thread;
                thread_data[j].result = NULL;
                pthread_create(&threads[j], NULL, straight_stroke, &thread_data[j]);
            }
            for(int j = 0; j < quantity_thread; j++) {
            pthread_join(threads[j], NULL);
            }
    }

    // обратный ход - выражение неизвестных
    long double* result = new long double [column];
    for (int i = 0; i < str; i++){
        result[i] = default_value_of_variable;
    }

    for (int i = str - 1; i >= 0; i--) {
        result[i] -=default_value_of_variable;
        if (column - i - 1 > 0) {
            for (int v = i+1; v < str; v++) {
                free_value[i] -= result[v] * matrix[i][v];
                matrix[i][v] = 0;
            }
        }
        if(matrix[i][i] == 0){
            result[i] = default_value_of_variable;
            break;
        } 
        result[i] = free_value[i] / matrix[i][i];
    }

    cout << "Решение системы:" << endl;
    for (int i = 0; i < column; i++) {
        cout << result[i] << " ";
    }
    auto end = chrono::steady_clock::now();
    cout << "/n Время выполнения: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
    delete result;
    for (int i = 0; i < str; i++){
        delete matrix[i];
    }
    delete matrix;
    delete free_value;
    delete thread_data; 
    delete threads;
    return 0;
}