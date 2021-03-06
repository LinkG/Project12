#include "Network_Fast.H"
#include <iostream>

int NetworkFast::num_layers = 0;

NetworkFast::NetworkFast(NetworkPreferences &prefs) {
    num_layers = prefs.numLayers;
    sizes = new int[num_layers + 1];
    sizes[0] = 784;
    for(i = 1; i <= num_layers; i++) {
        sizes[i] = prefs.layers[i - 1];
    }
    bias = new float*[num_layers];
    activations = new float*[num_layers];
    dels = new float*[num_layers];
    weights = new float**[num_layers];

    prev_epoch = 0;
    for(i = 0; i < num_layers; i++) {
        activations[i] = new float[sizes[i + 1]];
        bias[i] = new float[sizes[i + 1]];
        dels[i] = new float[sizes[i + 1]];
        weights[i] = new float*[sizes[i + 1]];
        for(j = 0; j < sizes[i + 1]; j++) {
            weights[i][j] = new float[sizes[i]];
        }
    }
}

void NetworkFast::backpropagate(float in[], int corr[]) {
    float sum;
    float *z_vals[num_layers];

    //FEEDFORWARD
    for(k = 0; k < num_layers; k++) {
        z_vals[k] = new float[sizes[k + 1]];
        for(i = 0; i < sizes[k + 1]; i++) {
            sum = 0;
            for(j = 0; j < sizes[k]; j++) {
                if(k == 0) {
                    sum += (in[j]) * weights[k][i][j]; // First layer
                } else {
                    sum += activations[k - 1][j] * weights[k][i][j];
                }
            }
            sum += bias[k][i];
            z_vals[k][i] = sum;
            activations[k][i] = sigmoid(sum);
        }
    }

    //===============================
    //BACKPROP
    //Calculate error in first layer
    for(i = 0; i < sizes[num_layers]; i++) {
        dels[num_layers - 1][i] = (activations[num_layers - 1][i] - float(corr[i])) * sigmoidprime(z_vals[num_layers - 1][i]);
    }
    //Errors in remaining neurons
    for(k = num_layers; k > 1; k--) {
        int prev = k - 1;
        for(i = 0; i < sizes[prev]; i++) {
            sum = 0;
            for(j = 0; j < sizes[k]; j++) {
                sum += (dels[k - 1][j] * weights[k - 1][j][i]);
            }
            sum *= sigmoidprime(z_vals[prev - 1][i]);
            dels[prev - 1][i] = sum;
        }
    }

    for(i = 0; i < num_layers; i++) {
        delete[] z_vals[i];
    }
}

void NetworkFast::descent(float alpha, float **data, char labels[], int s, NetworkPreferences &np) {
    float *averageBias[num_layers];
    float **averageWeight[num_layers];
    for(k = 0; k < num_layers; k++) {
        averageBias[k] = new float[sizes[k + 1]];
        memset(averageBias[k], 0, sizeof(float) * sizes[k + 1]);
        averageWeight[k] = new float*[sizes[k + 1]];
        for(i = 0; i < sizes[k + 1]; i++) {
            averageWeight[k][i] = new float[sizes[k]];
            memset(averageWeight[k][i], 0, sizeof(float) * sizes[k]);
        }
    }
    int batches, mini, progress = 0;
    int *correct = new int[np.num_activations];
    memset(correct, 0, sizeof(int) * np.num_activations);
    float stepsize;
    int loop_len = s / 8;
    for(batches = 0; batches < loop_len; batches++) {
    //stepsize = (alpha + exp(-(prev_epoch + ((batches + 1) / 6250)) / 5)) / 8.0;
	stepsize = (alpha) / 8.0;
        for(mini = 0; mini < 8; mini++) {
            int sample_no = (batches * 8) + mini;
            //There was a TF here
            correct[np.getNumber(labels[sample_no])] = 1;

            backpropagate(data[sample_no], correct);

            for(k = 0; k < num_layers; k++) {
                for(i = 0; i < sizes[k + 1]; i++) {
                    averageBias[k][i] += dels[k][i];
                    for(j = 0; j < sizes[k]; j++) {
                        if(k == 0) {
                            averageWeight[k][i][j] += dels[k][i] * (data[sample_no][j]);
                        } else {
                            averageWeight[k][i][j] += dels[k][i] * activations[k - 1][j];
                        }
                    }
                }
            }
            //TF here to
            correct[np.getNumber(labels[sample_no])] = 0;
        }
        for(k = 0; k < num_layers; k++) {
            for(i = 0; i < sizes[k + 1]; i++) {
                bias[k][i] -= averageBias[k][i] * stepsize;
                for(j = 0; j < sizes[k]; j++) {
                    weights[k][i][j] -= averageWeight[k][i][j] * stepsize;
                }
                memset(averageWeight[k][i], 0, sizeof(float) * sizes[k]);
            }
            memset(averageBias[k], 0, sizeof(float) * sizes[k + 1]);
        }
        if(int((float(batches)/loop_len) * 100) > progress) {
            progress = (float(batches) / loop_len) * 100;
            std::cout << progress << '%' << '\n';
        }
    }
    prev_epoch++;
}

void NetworkFast::feedforward(float in[]) {
    float sum;
    for(k = 0; k < num_layers; k++) {
        for(i = 0; i < sizes[k + 1]; i++) {
            sum = 0;
            for(j = 0; j < sizes[k]; j++) {
                if(k == 0) {
                    sum += (in[j]) * weights[k][i][j]; // First layer
                } else {
                    sum += activations[k - 1][j] * weights[k][i][j];
                }
            }
            sum += bias[k][i];
            activations[k][i] = sigmoid(sum);
        }
    }
}

void NetworkFast::infer(float in[], float o[]) {
    feedforward(in);

    for(i = 0; i < sizes[num_layers]; i++) {
        o[i] = activations[num_layers - 1][i];
    }
}

int NetworkFast::inferCorrect(float in[784]) {
    feedforward(in);
    
    int mi = 0;
    float m = activations[num_layers - 1][0];
    for(i = 1; i < sizes[num_layers]; i++) {
        if(activations[num_layers - 1][i] > m) {
            m = activations[num_layers - 1][i];
            mi = i;
        }
    }
    return mi;
}

void NetworkFast::setLayer(float* b, float** w, int l) {
    for(i = 0; i < sizes[l + 1]; i++) {
        bias[l][i] = b[i];
        for(j = 0; j < sizes[l]; j++) {
            weights[l][i][j] = w[i][j];
        }
    }
}

NetworkFast::~NetworkFast() {
    for(i = 0; i < num_layers; i++) {
        delete[] bias[i];
        delete[] weights[i];
    }
    delete[] activations;
    delete[] dels;
    delete[] sizes;
}

float*** NetworkFast::getWeightPointer() {
    return weights;
}

float** NetworkFast::getBiasPointer() {
    return bias;
}

float NetworkFast::sigmoid(float x) {
    float xi = 1.0 / (1 + exp(-x));
    if(xi == 1) {
        return float(0.999);
    } else if(xi == 0) {
        return float(0.001);
    } else {
        return xi;
    }
}

float NetworkFast::sigmoidprime(float x) {
    float fl = sigmoid(x);
    return fl * (1 - fl);
}