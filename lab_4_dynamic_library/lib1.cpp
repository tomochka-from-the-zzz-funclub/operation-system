#include <cmath>

extern "C" int PrimeCount(int A, int B) {
    int prime_count = 0;
    int *numbers = new int[B - A + 1];
    for (int i = A; i < B -A + 1; i++){
        numbers[i] = i;
    }
    int del = 0;
    for(int i = A; i < B - A + 1; i++) {
        if (i > 1){
            for (int j = 2; j < i; j++) {
                if (numbers[i] % numbers[j] == 0){
                    del++;
                }
            }
            if(del == 0) {
               prime_count++;
            }
        }
        del = 0;
    }
    return prime_count;
}
int factorial(double count){
    double num = 1;
    for (double i = 1; i < count + 1; i++)
        num = num * i;
    return num;
}

extern "C" float E(double x) {
    double e = 1;
    for (int i = 1; i < x + 1; i++) {
        e += (1 / factorial(i));
    }
    return e;
}
