#ifndef PFMLOADER_H
#define PFMLOADER_H

#include "ofImage.h"
#include "ofFileUtils.h"
#include <sstream>


using namespace std;

class pfmImage : public ofFloatImage {

public:
    bool 	loadPFMImage(string fileName);

    static vector<float> getFloatPixels(string filename);

};

#endif // PFMLOADER_H
