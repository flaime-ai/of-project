#include "ofMain.h"
#include "ofApp.h"

//========================================================================

int main(int argc, char *argv[]) {

    ofGLWindowSettings settings;
    settings.setGLVersion(4, 1); // now we use OpenGL 4.1
    settings.setSize(1024, 768);

    ofCreateWindow(settings);
    vector<string> myArgs;
    if(argc > 1){
        for(int i = 0; i < argc; i++){
            myArgs.push_back(argv[i]);
        }
    }

    std::shared_ptr<ofApp> app = std::make_shared<ofApp>();
    app->arguments = myArgs;

    ofRunApp(app);


//    ofSetupOpenGL(592, 1072, OF_WINDOW);

//    vector<string> myArgs;
//    if(argc > 1){
//        for(int i = 0; i < argc; i++){
//            myArgs.push_back(argv[i]);
//        }
//    }

//    std::shared_ptr<ofApp> app = std::make_shared<ofApp>();
//    app->arguments = myArgs;
//    return ofRunApp(app);
}
