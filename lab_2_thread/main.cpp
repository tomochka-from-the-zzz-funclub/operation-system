#include <iostream>
#include <cmath>
#include <malloc.h>
//#include <mutex>
//#include <thread>
#include <chrono>
using namespace std;

float default_value_of_variable = 1;

typedef struct{
	int rowN; //номер строки ведущего элемента
	int rowSize; //размер строки
    int rowStr;
    float* free_val;
    float* result;
	//указатели на матрицу
	float** matrix; 
} pthrData;

void print_matrix (float** matrix, float* free_value, int n, int m) {
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
    if(data->matrix[data->rowN][data->rowN] == 0) {
        for (int i = data->rowN + 1; i < data->rowStr; i++) {
            if(data->matrix[i][data->rowN] == 0) {
                 continue;
            }
            float* test = new float [data->rowSize];
            for (int k = 0; k < data->rowSize; k++) {
                test[k] = data->matrix[data->rowN][k];
            }
            for (int j = 0; j < data->rowSize; j++) {
                data->matrix[data->rowN][j] = data->matrix[i][j];
            }
            for (int k = 0; k < data->rowSize; k++) {
                data->matrix[i][k] = test[k];
            }
            float test_free = data->free_val[i];
            data->free_val[i] = data->free_val[data->rowN];
            data->free_val[data->rowN] = test_free;
            break;
        }
    }
    //cout << "============="<< endl;
        for(int k = data->rowN+1; k < data->rowStr; k++) {
            float mnoj = -data->matrix[k][data->rowN] / data->matrix[data->rowN][data->rowN];
            for (int j = 0; j < data->rowSize; j++) {
                data->matrix[k][j] += data->matrix[data->rowN][j]*mnoj;
                //cout << i << " " << k << " " << j << " " << mnoj << endl;
            }
            data->free_val[k] += data->free_val[data->rowN]*mnoj;
        }
    return NULL;
}
void* reverse_stroke (void* thred_data) {
    pthrData* data = (pthrData*) thred_data;
    data->result[data->rowN] -=default_value_of_variable;
    if (data->rowSize - data->rowN - 1 > 0) {
        for (int v = data->rowN+1; v < data->rowStr; v++) {
            data->free_val[data->rowN] -= data->result[v] * data->matrix[data->rowN][v];
            data->matrix[data->rowN][v] = 0;
        }
    }
    if(data->matrix[data->rowN][data->rowN] == 0){
        data->result[data->rowN] = default_value_of_variable;
        return NULL;
    } 
    data->result[data->rowN] = data->free_val[data->rowN] / data->matrix[data->rowN][data->rowN];
    return NULL;
}

int main() {
    long int str, column; //количество строк и столбцов в матрице
    cout << "Введите количество уравнений в системе" << endl;
    cin >> str;
    cout << "Введите количество перменных" << endl;
    cin >> column;
    float* free_value = new float [str];
    float** matrix = new float* [str];
    for (int i = 0; i < str; i++){
        matrix[i] = new float [column];
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
    pthread_t* threads =  new  pthread_t [quantity_thread];
    pthrData* thread_data =  new  pthrData [str];
    
    // прямой ход - исключение переменных
    for(int i = 0; i < str; i++) {
        thread_data[i].rowN = i;
        thread_data[i].rowSize = column;
        thread_data[i].rowStr = str;
        thread_data[i].free_val = free_value;
        thread_data[i].matrix = matrix;
        thread_data[i].result = NULL;
        pthread_create(&threads[i%quantity_thread], NULL, straight_stroke, &thread_data[i]);
        //print_matrix (matrix, free_value, str, column);
        //cout  << "1 "<< i%quantity_thread << endl;
    }

    for(int i = 0; i < quantity_thread; i++) {
        //ждем завершения потока
	pthread_join(threads[i], NULL);
    }

    //print_matrix (matrix, free_value, str, column);
    // обратный ход - выражение неизвестных
    float* result = new float [column];
    for (int i = 0; i < str; i++){
        result[i] = default_value_of_variable;
    }
    for (int i = str-1; i >= 0; i--) {
        thread_data[i].rowN = i;
        thread_data[i].rowSize = column;
        thread_data[i].rowStr = str;
        thread_data[i].free_val = free_value;
        thread_data[i].matrix = matrix;
        thread_data[i].result = result;
        pthread_create(&threads[i%quantity_thread], NULL, reverse_stroke, &thread_data[i]);
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
    cout << "/n Время выполнения: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
    return 0;
}