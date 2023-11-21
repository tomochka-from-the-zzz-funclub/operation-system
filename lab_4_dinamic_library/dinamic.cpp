#include <iostream>
#include <cmath>
using namespace std;

void sieve_Eratosthenes(int a, int b) {
  int *numbers = new int[b - a + 1];
  for (int i = a; i < b - a + 1; i++){
    numbers[i] = i;
  } 
  for (int p = a; p < b - a + 1; p++){
    if(p>1){
        if (numbers[p] != 0){
        cout << numbers[p] << endl;
        for (int j = p*p; j < b - a + 1; j += p)
            numbers[j] = 0;
        }
    } else{
        cout << numbers[p] << endl;
    }
  }
} 

void naive_algorithm(int a, int b) {
    int *numbers = new int[b - a + 1];
    for (int i = a; i < b -a + 1; i++){
        numbers[i] = i;
    }
    int del = 0;
    for(int i = a; i < b - a + 1; i++) {
        if (i > 1){
            for (int j = 2; j < i; j++) {
                if (numbers[i] % numbers[j] == 0){
                    del++;
                }
            }
            if(del == 0) {
               cout <<  numbers[i] << endl;
            }
        } else {
            cout <<numbers[i] << endl;
        }
        del = 0;
    }
}
int factorial(double count){
    double num = 1;
    for (double i = 1; i < count + 1; i++)
        num = num * i;
    return num;
}

float E_factorial(double x) {
    double e = 1;
    for (int i = 1; i < x + 1; i++) {
        e += (1 / factorial(i));
    }
    return e;
}

float E_pow(double x) { 
    return pow((1 + 1 / x), 1 / x);
}
