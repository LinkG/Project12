#ifndef NPrefs
#define NPrefs
#include<string.h>
class NetworkPreferences {
public:
    char* outputLayer;
    int num_activations;
    int numLayers;
    int* layers;
    NetworkPreferences();
    NetworkPreferences(const char* outs, int* t, int nl);
    NetworkPreferences(char* outs);
    ~NetworkPreferences();
    int getNumber(char symbol);
    char getSymbol(int index) { return outputLayer[index]; }
};
#endif
