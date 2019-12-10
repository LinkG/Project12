#include "FastSave.H"

#define p(x) std::cout << x
#define pln(x) std::cout << x << '\n'

Save::Save(const char* file, bool is) {
    fname = new char[strlen(file)];
    strcpy(fname, file);
    isc = is;
}
char* Save::getPreferences(NetworkPreferences &np) {
    if(!isc) {
        pln("Bad bad");
        return NULL;
    }
    char *fame = new char[100];
    char data[50];
    fileStream.open(fname, std::ios::in);
    fileStream.getline(fame, 100, ',');
    fileStream.getline(data, 50, ',');
    if(np.numLayers != -1) {
        delete[] np.outputLayer;
    }
    np.numLayers = std::atoi(data);
    np.layers = new int[np.numLayers];
    for(int i = 0; i < np.numLayers; i++) {
        fileStream.getline(data, 50, ',');
        np.layers[i] = std::atoi(data);
    }
    fileStream.getline(data,50);
    np.outputLayer = new char[strlen(data)];
    np.num_activations = strlen(data);
    strcpy(np.outputLayer, data);
    return fame;
}
void Save::SaveToFile(NetworkFast &net) {
    if(isc) {
        pln("Bad call");
        return;
    }
    fileStream.open(fname, std::ios::out | std::ios::binary);
    int holder = NetworkFast::num_layers;
    fileStream.write((char*)&holder, sizeof(holder));
    for(k = 1; k <= NetworkFast::num_layers; k++) {
        holder = net.getSize(k);
        fileStream.write((char*)&holder, sizeof(holder));
    }
    float temp;
    float*** w_pointer = net.getWeightPointer();
    float** b_pointer = net.getBiasPointer();
    for(k = 0; k < NetworkFast::num_layers; k++) {
        for(i = 0; i < net.getSize(k + 1); i++) {
            for(j = 0; j < net.getSize(k); j++) {
                temp = w_pointer[k][i][j];
                fileStream.write((char*)&temp, sizeof(temp));
            }
            temp = b_pointer[k][i];
            fileStream.write((char*)&temp, sizeof(temp));
        }
    }
    fileStream.close();
}


//Returns false if fails
bool Save::ReadToNetwork(NetworkFast &net) {
    if(isc) {
        pln("Bad call");
        return NULL;
    }
    fileStream.open(fname, std::ios::in | std::ios::binary);
    int holder;
    fileStream.read((char*)&holder, sizeof(holder));
    int nlay = holder;
    if(holder != NetworkFast::num_layers) {
        pln("Wrong configuration for this file, printing details.");
        p("Number of layers:");
        pln(holder);
        p("Layers: ");
        for(i = 0; i < nlay; i++) {
            fileStream.read((char*)&holder, sizeof(holder));
            p(holder);
            p(", ");
        }
        pln("END");
        return false;
    }
    p("Saved Network Configuration: ");
    bool flag = false;
    for(i = 0; i < nlay; i++) {
        fileStream.read((char*)&holder, sizeof(holder));
        if(holder != net.getSize(i + 1)) {
            flag = true;
        }
        p(holder);
        p(", ");
    }
    pln("END");
    if(flag) {
        pln("Error");
        return false;
    }
    float ***w_pointer = net.getWeightPointer();
    float **b_pointer = net.getBiasPointer();
    float temp;
    for(k = 0; k < NetworkFast::num_layers; k++) {
        for(i = 0; i < net.getSize(k + 1); i++) {
            for(j = 0; j < net.getSize(k); j++) {
                fileStream.read((char*)&temp, sizeof(temp));
                w_pointer[k][i][j] = temp;
            }
            fileStream.read((char*)&temp, sizeof(temp));
            b_pointer[k][i] = temp;
        }
    }
    fileStream.close();
    return true;
}

bool Save::checkForFile() {
    struct stat buffer;   
    return (stat (fname, &buffer) == 0);
}

void Save::closeStream() {
    if(fileStream.is_open()) {
        fileStream.close();
    }
}

Save::~Save() {
    delete[] fname;
    if(fileStream.is_open()) {
        fileStream.close();
    }
}