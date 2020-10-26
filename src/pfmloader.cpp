#include "pfmloader.h"
#include <limits>

bool pfmImage::loadPFMImage(string fileName) {

    ifstream file;
    file.open(ofToDataPath(fileName, true), ios::binary | ios::in);

    char lineBuffer[2048];

    try {

        //HEADER line 1
        file.getline(lineBuffer, 2048);
        bool colour;
        if (lineBuffer[0] == 'P' && lineBuffer[1] == 'F')
            colour = true;
        else if (lineBuffer[0] == 'P' && lineBuffer[1] == 'f')
            colour = false;
        else
            throw(0);

        //HEADER line 2
        file.getline(lineBuffer, 2048);
        int width, height;
        istringstream line(lineBuffer);

        line >> width;
        line >> height;

        if (width <= 0 || height <= 0)
            throw(0);

        this->allocate(width, height, colour ? ofImageType::OF_IMAGE_COLOR : ofImageType::OF_IMAGE_GRAYSCALE);

        //HEADER line 3
        file.getline(lineBuffer, 2048);
        float scale;
        line = istringstream(lineBuffer);

        line >> scale;
        scale = abs(scale);

        /**HACK**/
        //for the time being we ignore endianness


        //BODY
        ofFloatPixels pix = this->getPixels();
        float value;

        int expectedPixels = this->getWidth() * this->getHeight() * this->getPixels().getNumChannels();

        float max_val = 0.0f;
        float min_val = std::numeric_limits<float>::max();

        vector<float> tmp_pix;

        for (int i = 0; i < expectedPixels; i++) {
            file.read((char*)&value, sizeof(float));
            max_val = (max_val < value) ? value : max_val;
            min_val = (value < min_val) ? value : min_val;
            tmp_pix.push_back(value);
            //pix[i] = value * scale;
        }

  //      ofLog() << ofToString(min_val) << "," << ofToString(max_val);
        const float val_range = max_val - min_val;
        for (int i = 0; i < expectedPixels; i++) {
            const float scaled_val = (tmp_pix[i] - min_val) / val_range;
            pix[i] = scaled_val;
//            ofLog() << ofToString(tmp_pix[i]) << "," << ofToString(scaled_val);
        }
        //this->setFromPixels(pix);

        this->update();
        return true;

    } catch (...) {
        this->pixels.clear();
        return false;
    }
}

vector<float> pfmImage::getFloatPixels(string fileName) {

    ifstream file;
    file.open(ofToDataPath(fileName, true), ios::binary | ios::in);

    char lineBuffer[2048];

    vector<float> pixels;

    try {

        //HEADER line 1
        file.getline(lineBuffer, 2048);
        bool colour;
        if (lineBuffer[0] == 'P' && lineBuffer[1] == 'F')
            colour = true;
        else if (lineBuffer[0] == 'P' && lineBuffer[1] == 'f')
            colour = false;
        else
            throw(0);

        //HEADER line 2
        file.getline(lineBuffer, 2048);
        int width, height;
        istringstream line(lineBuffer);

        line >> width;
        line >> height;

        if (width <= 0 || height <= 0)
            throw(0);


        //HEADER line 3
        file.getline(lineBuffer, 2048);
        float scale;
        line = istringstream(lineBuffer);

        line >> scale;
        scale = abs(scale);

        /**HACK**/
        //for the time being we ignore endianness


        //BODY
        float value;

        int expectedPixels = width * height * (colour ? 3 : 1);

        float max_val = 0.0f;
        float min_val = std::numeric_limits<float>::max();

        vector<float> tmp_pix;

        for (int i = 0; i < expectedPixels; i++) {
            file.read((char*)&value, sizeof(float));
            max_val = (max_val < value) ? value : max_val;
            min_val = (value < min_val) ? value : min_val;
            tmp_pix.push_back(value);
            //pix[i] = value * scale;
            pixels.push_back(value);
        }

//        const float val_range = max_val - min_val;
//        for (int i = 0; i < expectedPixels; i++) {
//            const float scaled_val = (tmp_pix[i] - min_val) / val_range;
//            pixels.push_back(scaled_val);
//        }

        return pixels;

    } catch (...) {
        return pixels;
    }
}
