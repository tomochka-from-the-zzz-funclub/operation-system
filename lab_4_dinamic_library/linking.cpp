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

float E_pow(double x) { 
    return pow((1 + 1 / x), 1 / x);
}
