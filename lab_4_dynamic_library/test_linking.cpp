#include <iostream>

using namespace std;
extern "C" int PrimeCount(int A, int B); 
extern "C" float E(double x);

int main()  {
    cout << "Enter '1' if you want to use PrimeCount, and enter '2' if you want to use E" << endl;
    int action;
    while (cin >> action) {
        if(action == 3) {
            return 0;
        }
        if (action == 1) {
            int A, B;
            cin >> A >> B;
            int prime_count = PrimeCount(A, B);
            cout << prime_count << endl;
        } else if (action == 2) {
            double X;
            cin >> X;
            float e = E(X);
            cout << e << endl;
        } else {
            cout << "This action does not exist, check the entered command =(" << endl;
        }
    }
}