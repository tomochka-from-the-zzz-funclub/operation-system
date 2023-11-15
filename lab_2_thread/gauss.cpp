#include <iostream>
#include <cmath>
#include <malloc.h>
//#include <mutex>
//#include <thread>
#include <ctime>

using namespace std;

float default_value_of_variable = 1;
int main() {
    unsigned int start_time =  clock(); // начальное время

    long int str, column; //количество строк и столбцов в матрице
    cout << "Введите количество уравнений в системе" << endl;
    cin >> str;
    cout << "Введите количество перменных" << endl;
    cin >> column;
    float* free_value = new float [str];
    float* result = new float [column];
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
    for (int i = 0; i < column; i++) {
        result[i] = default_value_of_variable;
    }

    // прямой ход - исключение переменных
    for(int i = 0; i < str; i++) {
        for(int k = i+1; k < str; k++) {
            float mnoj = -matrix[k][i]/matrix[i][i];
            for (int j = 0; j < column ; j++) {
                matrix[k][j] += matrix[i][j]*mnoj;
                //cout << i << " " << k << " " << j << " " << mnoj << endl;
            }
            free_value[k] += free_value[i]*mnoj;
        }
    }
    //print_matrix (matrix, free_value, str, column);

    // обратный ход - выражение неизвестных

    for (int i = str-1; i >= 0; i--) {
        result[i] -=default_value_of_variable;
        if (column-i-1 > 0) {
            for (int v = i+1; v < str; v++) {
                free_value [i] -= result[v]*matrix[i][v];
                matrix[i][v] = 0;
            }
        }
        result[i] = free_value[i]/matrix[i][i];
    }
    cout << "Решение системы:" << endl;
    for (int i = 0; i < column; i++) {
        cout << result[i] << " ";
    }
    unsigned int end_time = clock(); // конечное время
    unsigned int search_time = end_time - start_time; // искомое время
    cout << "\n" << search_time << endl;
}