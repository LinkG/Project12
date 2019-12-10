#ifndef FastSave
#define FastSave

#include <fstream>
#include <string.h>
#include <iostream>
#include <sys/stat.h>
#include "Network_Fast.H"

class Save {
    std::fstream fileStream;
    bool isc;//isConfig
    char *fname;
    int i, j, k;
public:
    Save();
    Save(const char *file, bool isc);
    void SaveToFile(NetworkFast &net);
    bool ReadToNetwork(NetworkFast &net);
    char* getPreferences(NetworkPreferences &np);
    bool checkForFile();
    void closeStream();
    ~Save();
};

#endif