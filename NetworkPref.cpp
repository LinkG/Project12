#include "NetworkPref.H"

NetworkPreferences::NetworkPreferences() {
    numLayers = -1;
}

NetworkPreferences::NetworkPreferences(const char* outs, int *t, int nl) {
    outputLayer = new char[strlen(outs)];
    num_activations = strlen(outs);
    strcpy(outputLayer, outs);
    numLayers = nl;
    layers = new int[nl];
    for(int i = 0; i < nl; i++) {
        layers[i] = t[i];
    }
}

NetworkPreferences::NetworkPreferences(const char* outs, int* a, int size) {
    num_activations = strlen(outs);
    outputLayer = new char[strlen(outs)];
    strcpy(outputLayer, outs);
    numLayers = size;
    layers = new int[size];
    for(int i = 0; i < size; i++) {
        layers[i] = a[i];
    }
}

NetworkPreferences::NetworkPreferences(char* outs) {
    num_activations = strlen(outs);
    outputLayer = new char[strlen(outs)];
    strcpy(outputLayer, outs);
    numLayers = -1;
}

NetworkPreferences::~NetworkPreferences() {
    delete[] outputLayer;
    delete[] layers;
}

int NetworkPreferences::getNumber(char symbol) {
    int i = 0;
    for( ; i < num_activations; i++) {
        if(outputLayer[i] == symbol) {
            break;
        }
    }
    return i >= num_activations ? -1 : i;
}
