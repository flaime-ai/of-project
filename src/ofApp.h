#pragma once

#include "ofMain.h"

#include "ofxDeferredShading.h"
#include "ofxGui.h"
#include "ofxEasing.h"

#include "pfmloader.h"

class ofApp : public ofBaseApp{

public:

    vector<string> arguments;


    void setup();
    void setupFrames();
    void setupMesh();
    void setupDeferred();

    void update();

    void draw();
    void drawObjects();
    void logging();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);


    vector<string> depth_video_paths;
//    vector<pfmImage> depths;
    vector<vector<float>> depths;
    vector<ofPixels> rgbs;
    string scene_path;
    //    vector<ofPixels> depths;
   // vector<ofPixels> histograms;

 //   ofTexture rgb_tex;
    ofMesh mesh_plane;
    int width;
    int height;
//    ofImage frame_img;

//    ofMesh pointCloud;

    //    float person_depth;
    //    float person_back_zpos;
    //    float background_depth;
    //    float background_max_zpos;
        //float front_depth;

//   size_t current_frame = 0;
    size_t current_frame_index;
    size_t n_frames;
    float frame_lerp;
    float frame_lerp_bounce;
    float rotation_timer;
    float rotation;
    float max_angle;
    glm::vec3 mesh_translate;

    float max_depth;
    float min_depth;

    ofParameter<float> squeeze;
    float squeeze_factor;

    ofCamera cam;
    ofImage banner;
 //   ofEasyCam cam;

    //ofxPostProcessing post;

    ofxDeferredProcessing deferred;
    ofxDeferred::PointLightPass::Ptr pointLightPass;
    ofxDeferred::ShadowLightPass::Ptr shadowLightPass;
    ofxDeferred::SsaoPass::Ptr ssao;
    ofxDeferred::DofPass::Ptr dof;
    ofxDeferred::BloomPass::Ptr bloom;

    bool edit_mode;
    bool isShowPanel;
    ofxPanel panel;
};
