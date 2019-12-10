#include<iostream>
#include "MNISTRead.H"
#include "Network_Fast.H"
#include "FastSave.H"
#include <random>

#define alpha 0.5

#define num_layers NetworkFast::num_layers

#define p(x) std::cout << x;
#define prnt(x) std::cout << x << '\n';

void shuffleImagesAndLabels(float** &images, char* &labels, int size) {
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
            std::cout << (number[i1*28 + j1] == 1 ? 'o' : ' ') << ' ';
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

void inferNetwork(float** images, char* labels, NetworkFast &net, int size,  NetworkPreferences &np, int l = -1) {
    int guessed;
    int correct = 0, checked = 0;
    if(size == 1) {
        guessed = net.inferCorrect(images[0]);
        display(images[0]);
        p("Guessed ");
        float activations[10];
        net.infer(images[0], activations);
        prnt(np.getSymbol(guessed));
        prnt("Percentages: ");
        float sum = 0;
        for(int i = 0; i < 10; i++) {
            sum += activations[i];
        }
        for(int i = 0; i < 10; i++) {
            p("=> ");
            p(i);
            p(" -> ");
            p(activations[i] * 100 / sum);
            prnt("%");
        }
        return;
    }
    for(int q = 0; q < size; q++) {
        if(np.getNumber(labels[q]) == l || l == -1) {
            guessed = net.inferCorrect(images[q]);
            checked++;
            if(np.getSymbol(guessed) == labels[q]) {
                correct++;
            } else {
                display(images[q]);
                p("It is ")
                p(labels[q]);
                p(" guessed ");
                prnt(np.getSymbol(guessed));
            }
            prnt(float(correct) / checked);
        }
    }
}

int main(int argc, char* argv[]) {
    srand(time(0));
    if(argc < 3) {
        prnt("Missing argument/s, usage:\n./<executable> <config-name> <train/infer> <o(overwrite)/1-9(infer)/imgfile(infer)>");
        return 0;
    }

    NetworkPreferences np;
    //num_img is the number of images to load from MNIST
    int num_img = 60000, size;
    MNISTData data("Images/images-ubyte", "Images/labels-ubyte", num_img);
    float** images = data.getImages(size); //From MNIST
    char* label = data.getLabels();
    char filename[100];
    strcpy(filename, argv[1]);
    Save config(filename, true);
    strcpy(filename, config.getPreferences(np));
    Save svfile(filename, false);
    NetworkFast net(np);

    //MENU
    if(!strcmp("infer", argv[2])) {
        if(!svfile.checkForFile()) {
            prnt("Cant infer from non-existant file, try changing config file.");
            return -1;
        }
        if(!svfile.ReadToNetwork(net)) {
            prnt("Unable to match the save with current network config.");
            p("The current configuration: ")
            for(int i = 0; i < net.num_layers; i++) {
                p(net.getSize(i + 1));
                p(", ");
            }
            p("END\n");
            return -1;
        }
        if(argc == 4) {
            if(strlen(argv[3]) == 1 && argv[3][0] >= '0' && argv[3][0] <= '9') {
                inferNetwork(images, label, net, num_img, np, atoi(argv[3]));
            } else {
                std::ifstream imgfile(argv[3], std::ios::in);
                uchar tmp;
                for(int i = 0; i < 784; i++) {
                    imgfile.read((char*)&tmp, sizeof(uchar));
                    images[0][i] = tmp ? 0 : 1;
                }
                inferNetwork(images, label, net, 1, np);
                imgfile.close();
            }
        } else {
            inferNetwork(images, label, net, num_img, np);
        }
        return 2;
    } else if(!strcmp("train", argv[2])) {
        if(!svfile.checkForFile()) {
            prnt("Did not find a file, randomly initializing!");
            randomWeightAndBias(net);
        } else {
            if(argc == 4 && (argv[3][0] == 'o' || argv[3][0] == 'O')) {
                prnt("Ignoring the current saved configuration.");
                randomWeightAndBias(net);
                svfile.SaveToFile(net);
            } else {
                prnt("Attempting to continue training.. ");
                if(!svfile.ReadToNetwork(net)) {
                    prnt("Unable to match the save with current network config.");
                    p("The current configuration: ")
                    for(int i = 0; i < net.num_layers; i++) {
                        p(net.getSize(i + 1));
                        p(", ");
                    }
                    p("END\n");
                    return -1;
                }
            }
        }
    }

    prnt("Epochs:")
    int epoch;
    std::cin >> epoch;
    for(int i = 0; i < epoch ; i++) {
        net.descent(alpha, images, label, num_img, np);
        shuffleImagesAndLabels(images, label, num_img);
        prnt(i);
        svfile.SaveToFile(net);
    }
    prnt("----");
    float outs[10];
    display(images[0]);
    net.infer(images[0], outs);
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
