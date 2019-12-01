#ifndef Network_Fast
#define Network_Fast

#include <string.h>
#include <iostream>
#include "NetworkPref.H"
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
    NetworkFast(NetworkPreferences &prefs);
    float sigmoid(float x);
    float sigmoidprime(float x);
    void backpropagate(float in[], int correctvals[]);
    void descent(float, float **d, char*, int, NetworkPreferences &np);
    void infer(float in[], float o[]);
    int inferCorrect(float in[]);
    void setLayer(float*, float**, int);
    int getSize(int x) { return sizes[x]; }
    int numLayers() { return num_layers; }
    float*** getWeightPointer();
    float** getBiasPointer();
    void feedforward(float in[]);
    float getcost(){return avg_cost;};
    ~NetworkFast();
};

#endif