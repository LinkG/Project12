#ifndef MNISTRead
#define MNISTRead
#include <fstream>
typedef unsigned char uchar;
class MNISTData {
    int l;
    int reverseInt(int);
    std::ifstream labels;
    std::ifstream images;
    int getMagicNum(std::ifstream&);
    public:
    MNISTData(const char *, const char *);
    char* getLabels(int&);
    float** getImages(int&, int&);
    ~MNISTData();
};

#endif