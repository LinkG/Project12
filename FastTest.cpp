#include<iostream>
#include "MNISTRead.H"
#include "Network_Fast.H"
#include "FastSave.H"
#include <random>

#define layer_size 3
#define layer_topology {117, 43, 10}
#define alpha 0.1

#define num_layers NetworkFast::num_layers

#define p(x) std::cout << x;
#define prnt(x) std::cout << x << '\n';

void shuffleImagesAndLabels(float** &images, int* &labels, int size) {
    float swap1[784];
    int swap2;
    int k = size / 2;
    for(int i = 0; i < k; i++) {
        int t = (rand() % k) + 1;
        memmove(swap1, images[i], sizeof(float) * 784);
        swap2 = labels[i];
        memmove(images[i], images[size - t], sizeof(float) * 784);
        labels[i] = labels[size - t];
        labels[size - t] = swap2;
        memmove(images[size - t], swap1, sizeof(float) * 784);
    }
}

void display(float number[784]) {
    for(int i1 = 0; i1 < 28; i1++) {
        for(int j1 = 0; j1 < 28; j1++) {
            std::cout << (number[i1*28 + j1] == 1 ? 'o' : '.') << ' ';
        }
        std::cout << '\n';
    }
}

void randomWeightAndBias(NetworkFast &net) {
    std::default_random_engine generator;
    std::normal_distribution<float> distribuition(0.0f, 1.0f);

    int layer_sizes[2];
    for(int i = 1; i <= num_layers; i++) {
        layer_sizes[0] = net.getSize(i - 1);
        layer_sizes[1] = net.getSize(i);
        float *w[layer_sizes[1]], b[layer_sizes[1]];
        for(int k = 0; k < layer_sizes[1]; k++) {
            b[k] = distribuition(generator);
        }
        for(int j = 0; j < layer_sizes[1]; j++) {
            w[j] = new float[layer_sizes[0]];
            for(int k = 0; k < layer_sizes[0]; k++) {
                w[j][k] = distribuition(generator);
            }
        }
        net.setLayer(b, w, i - 1);
    }
}

void inferNetwork(float** images, int* labels, NetworkFast &net, int &size, int l = -1) {
    int guessed;
    int correct = 0, checked = 0;
    if(size == 1) {
        guessed = net.inferCorrect(images[0]);
        display(images[0]);
        p("Guessed ");
        prnt(guessed);
        return;
    }
    for(int q = 0; q < size; q++) {
        if(labels[q] == l || l == -1) {
            guessed = net.inferCorrect(images[q]);
            checked++;
            if(guessed == labels[q]) {
                correct++;
            } else {
                display(images[q]);
                p("It is ")
                p(labels[q]);
                p(" guessed ");
                prnt(guessed);
            }
            prnt(float(correct) / checked);
        }
    }
}

int main(int argc, char* argv[]) {
    srand(time(0));
    if(argc < 3) {
        prnt("Missing argument/s");
        return 0;
    }
    NetworkFast net(layer_size, layer_topology);
    MNISTData data("Images/images-ubyte", "Images/labels-ubyte");
    int num_img, size;
    float** images = data.getImages(num_img, size);
    int* label = data.getLabels(size);
    char filename[100];
    strcpy(filename, argv[1]);
    Save svfile(filename);
    bool match;
    bool new_file = false;
    if(svfile.checkForFile()) {
        match = svfile.ReadToNetwork(net);
    } else {
        new_file = true;
    }
    if(!strcasecmp("train", argv[2])) {
        if(!match && argc == 3 && !new_file) {
            prnt("Fix the network..");
            return 0;
        }
        if(new_file) {
            prnt("Creating new file");
            randomWeightAndBias(net);
        }
        if(argc == 4 && !new_file) {
            if(!match && tolower(argv[3][0]) == 'o') {
                prnt("Failed to match, overwriting..");
                randomWeightAndBias(net);
            }
            if(match && tolower(argv[3][0]) == 'o') {
                prnt("Overwriting..");
                randomWeightAndBias(net);
            }
        } else {
            prnt("Successfully read/created from file..");
        }
    } else if(!strcasecmp("infer", argv[2])) {
        if(!match) {
            prnt("Fix the network..");
            return 0;
        }
        if(match) {
            prnt("Successfully read from file..");
        }
        if(argc == 3) {
            inferNetwork(images, label, net, size);
        } else if(isalpha(argv[3][0])) {
            std::ifstream file(argv[3]);
            size = 1;
            uchar temp;
            for(int i = 0; i < 784; i++) {
                file.read((char*)&temp, sizeof(temp));
                images[0][i] = !temp;
            }
            file.close();
            float outputs[10];
            net.infer(images[0], outputs);
            p("[");
            p(outputs[0]);
            for(int i = 1; i < 10; i++) {
                p(",");
                p(outputs[i]);
            }
            prnt("]");
        } else {
            inferNetwork(images, label, net, size, atoi(argv[3]));
        }
        return 1;
    }
    svfile.SaveToFile(net);
    prnt("Epochs:")
    int epoch;
    std::cin >> epoch;
    for(int i = 0; i < epoch ; i++) {
        net.descent(alpha, images, label, 50000);
        shuffleImagesAndLabels(images, label, size);
        prnt(i);
        svfile.SaveToFile(net);
    }
    prnt("----");
    float outs[10];
    display(images[17]);
    net.infer(images[17], outs);
    float max = outs[0];
    int index = 0;
    std::cout << "[" << outs[0];
    for(int i = 1; i < 10; i++) {
        if(outs[i] > max) {
            max = outs[i];
            index = i;
        }
        std::cout << ", " << outs[i];
    }
    std::cout << "]\n";
    prnt(index);
    return 0;
}
