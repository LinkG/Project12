#ifndef Network_Fast
#define Network_Fast

#include <string.h>
#include <iostream>
#include <math.h>

class NetworkFast {
public:
    static int num_layers;
    int i, j, k, *sizes;
    float **bias;
    float **activations;
    float **dels;
    float ***weights;
    float avg_cost;
    int prev_epoch;
    //------------------------
    NetworkFast(int size, const int (&t)[]);
    float sigmoid(float x);
    float sigmoidprime(float x);
    void backpropagate(float in[], int correctvals[10]);
    void descent(float, float **d, int*, int);
    void infer(float in[784], float o[10]);
    int inferCorrect(float in[784]);
    void setLayer(float*, float**, int);
    int getSize(int x) { return sizes[x]; }
    int numLayers() { return num_layers; }
    float*** getWeightPointer();
    float** getBiasPointer();
    void feedforward(float in[784]);
    float getcost(){return avg_cost;};
    ~NetworkFast();
};

#endif