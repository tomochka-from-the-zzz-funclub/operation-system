#include <cmath>

extern "C" int PrimeCount(int A, int B) {
  int prime_count = 0;
  int *numbers = new int[B - A + 1];
  for (int i = A; i < B - A + 1; i++){
    numbers[i] = i;
  } 
  for (int p = A; p < B - A + 1; p++){
    if(p>1){
        if (numbers[p] != 0){
        prime_count++;
        for (int j = p*p; j < B - A + 1; j += p)
            numbers[j] = 0;
        }
    }
  }
  return prime_count;
} 

extern "C" float E(double x) { 
    return pow((1 + 1 / x), 1 / x);
}
