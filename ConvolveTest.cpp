#include <iostream>

float convolve(float** inLayer, float** kernel, int W, int F, int P, int stride) {
    int num_runs = 1 + ((W + (2 * P) - F) / 2);
    float sum = 0;
    for(int i = 0; i < num_runs; i++) {
        for(int j = 0; j < num_runs; j++) {
            
        }
    }
}

int main() {
    std::cout << "Hello world\n";
    return 0;
}