#include "ofApp.h"

void addNormals(ofMesh & mesh){
    auto& vertices = mesh.getVertices();
    auto& normals = mesh.getNormals();
    auto& indices = mesh.getIndices();

    for(auto& v : vertices) {
        mesh.addNormal({});
    }

    for(size_t i = 0; i < indices.size(); i+=3) {
        int ia = indices[i];
        int ib = indices[i+1];
        int ic = indices[i+2];

        auto e1 = vertices[ia] - vertices[ib];
        auto e2 = vertices[ic] - vertices[ib];
        auto no = glm::cross(e1, e2);

        normals[ia] += no;
        normals[ib] += no;
        normals[ic] += no;
    }

    for(auto& normal : mesh.getNormals()) {
        normal = glm::normalize(normal);
    }
}

void computeNormals(ofMesh & mesh){
    auto& vertices = mesh.getVertices();
    auto& normals = mesh.getNormals();
    auto& indices = mesh.getIndices();
    for(size_t i = 0; i < indices.size(); i+=3) {
        int ia = indices[i];
        int ib = indices[i+1];
        int ic = indices[i+2];

        auto e1 = vertices[ia] - vertices[ib];
        auto e2 = vertices[ic] - vertices[ib];
        auto no = glm::cross(e1, e2);

        normals[ia] = no;
        normals[ib] = no;
        normals[ic] = no;
    }

    for(auto& normal : mesh.getNormals()) {
        normal = glm::normalize(normal);
    }
}

float rgb_to_grey(float r, float g, float b) {
    return 0.30*r + 0.59*g + 0.11*b;
}

void ofApp::setupFrames() {
    string video_name = "1596318869_video";
    string scene_num = "008";
    scene_path = "/home/davy/Documents/Flaime/Triller/assets/Test15/processed/16:10:2020-17:43:34/scenes/" + video_name + "/" + video_name + "-Scene-" + scene_num;

    scene_path = "/home/davy/Documents/Flaime/Triller/processed/23:10:2020-13:14:21/videos/1598917036_video/encoded/001";
    edit_mode = true;
    if (arguments.size() >= 1) {
        edit_mode = false;
        scene_path = arguments[1];
    }

    ofDirectory rgb_dir(scene_path + "/frames");
    ofDirectory depth_dir(scene_path + "/depthmaps");
//    ofDirectory hist_dir(scene_path + "/histograms");

    rgb_dir.listDir();
    rgb_dir.sort();
    rgb_dir.allowExt("png");

    depth_dir.listDir();
    depth_dir.sort();
    depth_dir.allowExt("pfm");
//    depth_dir.allowExt("png");

//    hist_dir.listDir();
//    hist_dir.sort();
//    hist_dir.allowExt("png");

    max_depth = 0;
    min_depth = numeric_limits<float>().max();

    for (size_t j = 0; j < rgb_dir.size(); j++) {
        const string rgb_filename = rgb_dir.getName(j);
        const string depth_dir_name = depth_dir.getAbsolutePath();
        const string depth_image_path = depth_dir_name + "/" + ofSplitString(rgb_filename, ".png")[0] + ".pfm";
//        const string hist_dir_name = hist_dir.getAbsolutePath();
//        const string hist_image_path = hist_dir_name + "/" + rgb_filename;

//        if (ofFile::doesFileExist(depth_image_path) && ofFile::doesFileExist(hist_image_path)) {
        if (ofFile::doesFileExist(depth_image_path)) {
            ofPixels rgb_pix;
            const string rgb_image_path = rgb_dir.getPath(j);
            ofLoadImage(rgb_pix, rgb_image_path);
            rgbs.push_back(rgb_pix);

//            ofPixels depth_pix;
//            ofLoadImage(depth_pix, depth_image_path);
//            depths.push_back(depth_pix);

//            ofPixels hist_pix;
//            ofLoadImage(hist_pix, hist_image_path);
//            histograms.push_back(hist_pix);

//            pfmImage depth_pix;
//            depth_pix.loadPFMImage(depth_image_path);
            vector<float> pixs = pfmImage::getFloatPixels(depth_image_path);
            for (float pix : pixs) {
                max_depth = (pix > max_depth) ? pix : max_depth;
                min_depth = (pix < min_depth) ? pix : min_depth;

            }
            //ofLoadImage(depth_pix, depth_image_path);
            depths.push_back(pixs);

        }
    }

    banner.load("/home/davy/Documents/Flaime/Triller/assets/pepsi.png");

}

void ofApp::setupMesh() {
    // build mesh for depth images
    for (int y = 0; y < height; y++) {
        for (int x = 0; x<width; x++) {
            mesh_plane.addVertex(glm::vec3(x -(width / 2.0),y -(height / 2.0), 0));
            mesh_plane.addColor(ofFloatColor(0,0,0));
        }
    }
    for (int y = 0; y < height - 1; y++) {
        for (int x = 0; x < width - 1; x++) {
            mesh_plane.addIndex(x+y*width);
            mesh_plane.addIndex((x+1)+y*width);
            mesh_plane.addIndex(x+(y+1)*width);
            mesh_plane.addIndex((x+1)+y*width);
            mesh_plane.addIndex((x+1)+(y+1)*width);
            mesh_plane.addIndex(x+(y+1)*width);
        }
    }
    addNormals(mesh_plane);
    mesh_plane.enableNormals();
}

void ofApp::setup() {

    ofSetFrameRate(60);

    setupFrames();
    width = rgbs[0].getWidth();
    height = rgbs[0].getHeight();

    setupMesh();

    ofSetWindowShape(width, height);

//    int bgs[] = {0xFF9CEE, 0xBFFCC6,0x6EB5FF,0xFFeC59};
//    ofSetBackgroundColor(ofColor::fromHex(bgs[int(floor(ofRandom(0,4)))]));


    cam.setupPerspective();
//    cam.enableOrtho();
    cam.setFarClip(3000.);
    cam.setPosition(glm::vec3(0,0,-1000));
    cam.lookAt(glm::vec3(0,0,0));
    setupDeferred();

    current_frame_index = 0;
    n_frames = rgbs.size();
    mesh_translate = glm::vec3(0, height / 2.0, 1000);

    squeeze_factor = 2.0;
    squeeze = 0;

}

//--------------------------------------------------------------
void ofApp::update(){

    if (current_frame_index >= n_frames - 1) {
        current_frame_index = 0;
        if (!edit_mode) {
            ofExit(0);
        }
    }

    frame_lerp = current_frame_index / float(n_frames);
    frame_lerp_bounce = (frame_lerp < 0.5) ? frame_lerp : 1 - frame_lerp;

    ofPixels& current_rgb = rgbs[current_frame_index];
    vector<float> current_d = depths[current_frame_index];

//    ofPixels& current_d = depths[current_frame_index];
//    ofPixels& current_hist = histograms[current_frame_index];

    size_t n_rgb_Channels = current_rgb.getNumChannels();
    size_t width_of_line = current_rgb.getWidth();

//    person_depth = current_hist[0];
//    background_depth = current_hist[(width_of_line - 2) * n_rgb_Channels];
    size_t i = 0;

         for (size_t y = 0; y < current_rgb.getHeight(); y++) {
             for (size_t x = 0; x < width_of_line; x++) {


  //      const size_t i = (y * (current_rgb.getHeight() - 1) ) + x;
        float depth = current_d[( (current_rgb.getHeight() - 1 - y) * width_of_line) + x];
        const float range = max_depth - min_depth;
        depth -= min_depth + (range / 2.0);
        depth /= range;
        depth *= 1000;
        depth *= squeeze;
 //       ofLog() << ofToString(edepth);
//        // flatten everything behind person and remap to depth = -100*squeeze -> 100*squeeze
//        const bool is_behind_person = (depth < person_depth);
//        const float tmp_z = ofMap(depth, person_depth, 255, -100 * squeeze, 100 * squeeze, true);
        const float tmp_z = depth;
        glm::vec3 tmp_pos = mesh_plane.getVertex(i);
        tmp_pos.z = tmp_z;
        mesh_plane.setVertex(i, tmp_pos);

        int red = current_rgb[i * n_rgb_Channels];
        int green = current_rgb[i * n_rgb_Channels + 1];
        int blue = current_rgb[i * n_rgb_Channels + 2];
//        float grey = rgb_to_grey(red, green, blue);

        int alpha = 255;
        ofColor pixel_col = ofColor(red, green, blue, alpha);

//        int alpha = is_behind_person ? 255 : 255;
//        ofColor pixel_col = is_behind_person ? ofColor(grey, alpha) : ofColor(red, green, blue, alpha);

        mesh_plane.setColor(i, pixel_col);

        i++;
        }
    }

//    squeeze = ofxeasing::map(frame_lerp_bounce, 0.0, 0.5, 0.0, squeeze_factor, ofxeasing::exp::easeIn);
    squeeze = ofxeasing::map(1.0 - frame_lerp, 0.0, 1.0, 0.0, squeeze_factor, ofxeasing::linear::easeNone);
    squeeze = 1;
    current_frame_index += 1;


    computeNormals(mesh_plane);

}

//--------------------------------------------------------------

void ofApp::draw(){
    ofEnableDepthTest();
    ofEnableAlphaBlending();
    ofBackground(0);
//    const glm::vec3 tmp_img_z = cam.worldToScreen(glm::vec3(0, 0, 0));
//    const glm::vec3 img_tl = cam.screenToWorld(glm::vec3(-150, -206, tmp_img_z.z));
//    const glm::vec3 img_br = cam.screenToWorld(glm::vec3(150,206, tmp_img_z.z));

//    const glm::vec3 tmp_mesh_z = cam.worldToScreen(glm::vec3(0, 0, mesh_translate.z));
//    const glm::vec3 mesh_lhs = cam.screenToWorld(glm::vec3(-width / 2.0, mesh_translate.y, tmp_mesh_z.z));
//    const glm::vec3 mesh_rhs = cam.screenToWorld(glm::vec3(width + (width / 2.0), mesh_translate.y, tmp_mesh_z.z));
//    mesh_translate.x = ofxeasing::map(current_frame_index, 0, n_frames - 1, mesh_lhs.x, mesh_rhs.x, ofxeasing::linear::easeNone);

//    shadowLightPass->beginShadowMap();
//    drawObjects();
//    shadowLightPass->endShadowMap();

    deferred.begin(cam);
    drawObjects();
    if (isShowPanel) {
        pointLightPass->drawLights();
    }
    deferred.end();

    ofDisableDepthTest();

    if (!edit_mode) {
        logging();
    }
    else {
        if (isShowPanel) {
            deferred.debugDraw();
            shadowLightPass->debugDraw();
            dof->debugDraw();
            bloom->debugDraw();
            panel.draw();
            ofDrawBitmapString(ofToString(ofGetFrameRate()), 12, 16);
        }
    }
}

void ofApp::drawObjects() {
    glPointSize(1);

//    ofPushMatrix();
//    ofTranslate(-banner.getWidth() / 2,-banner.getHeight() / 2,500);
//    banner.draw(0,0);
//    ofPopMatrix();

    ofPushMatrix();
    const float t_z = ofxeasing::map(0.5 - frame_lerp_bounce,0,0.5,300,7000,ofxeasing::exp::easeIn);
    ofTranslate(glm::vec3(0, 0, t_z));
    ofRotateYDeg(180);
    const float r_y = ofxeasing::map(frame_lerp,0,1,-35,35,ofxeasing::quad::easeOut);
    ofRotateYDeg(r_y);
    mesh_plane.drawVertices();
    ofPopMatrix();
}

void ofApp::logging() {
    char buffer[256]; sprintf(buffer, "%06d", int(current_frame_index));
    string str(buffer);
    ofSaveScreen(scene_path + "/final/frame_" + str + ".png");
}

void ofApp::setupDeferred() {
    deferred.init(width, height);

    auto bg = deferred.createPass<ofxDeferred::BgPass>();
    bg->begin();
    ofClear(1, 3, 6, 255);
    bg->end();

    auto e = deferred.createPass<ofxDeferred::EdgePass>();
    e->setEdgeColor(1);
    e->setUseReadColor(true);

    ssao = deferred.createPass<ofxDeferred::SsaoPass>();
    ssao->setDarkness(0.3);
    ssao->setOcculusionRadius(10);

    shadowLightPass = deferred.createPass<ofxDeferred::ShadowLightPass>();
    shadowLightPass->setDarkness(0.9f);
    shadowLightPass->setViewPortSize(height);
    shadowLightPass->setFar(3000.);
    shadowLightPass->setNear(40.);

    pointLightPass = deferred.createPass<ofxDeferred::PointLightPass>();
    auto l = pointLightPass->addLight();
    pointLightPass->addLight();
    pointLightPass->setEnabled(true);

    dof = deferred.createPass<ofxDeferred::DofPass>();

    bloom = deferred.createPass<ofxDeferred::BloomPass>();
    bloom->setThreshold(0.6);

    panel.setup();
    panel.setDefaultBackgroundColor(ofFloatColor(0., 0.5));
    panel.setDefaultHeaderBackgroundColor(ofFloatColor(0.6, 0.6, 0.3, 0.6));
    panel.setDefaultFillColor(ofFloatColor(0.3, 0.3, 0.6, 0.6));
    panel.add(squeeze.set("stretch", 1, 0.0, 10.0));
    panel.add(deferred.getParameters());
    panel.minimizeAll();
    panel.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 's') isShowPanel = !isShowPanel;
    else if (key == 'l') pointLightPass->addLight();
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
