#include "ofApp.h"

#include "ofxNumpy.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowShape(640, 640);
    //ofSetWindowShape(560, 315);
    ofxNumpy::load(ofToDataPath("tsneResult.npy"), y);

    cnpy::NpyArray t;
    t = cnpy::npy_load(ofToDataPath("t0.npy"));
    size_t dim, n;
    ofxNumpy::getSize(t, dim, n);
    double* data = t.data<double>();
    for (int i = 0; i < n / dim; i++)
    {
        vector<float> feat_vector(dim);
        for (int j = 0; j < dim; j++)
        {
            feat_vector.at(j) = *data;
            data++;
        }
        feat_matrix.push_back(feat_vector);
    }

    t = cnpy::npy_load(ofToDataPath("t1.npy"));
    ofxNumpy::getSize(t, dim, n);
    data = t.data<double>();
    for (int i = 0; i < n / dim; i++)
    {
        vector<float> feat_vector(dim);
        for (int j = 0; j < dim; j++)
        {
            feat_vector.at(j) = *data;
            data++;
        }
        feat_matrix.push_back(feat_vector);
    }

    t = cnpy::npy_load(ofToDataPath("t2.npy"));
    ofxNumpy::getSize(t, dim, n);
    data = t.data<double>();
    for (int i = 0; i < n / dim; i++)
    {
        vector<float> feat_vector(dim);
        for (int j = 0; j < dim; j++)
        {
            feat_vector.at(j) = *data;
            data++;
        }
        feat_matrix.push_back(feat_vector);
    }

    float minimum = 1000;
    float maximum = -1000;
    for (int i = 0; i < feat_matrix.size(); i++)
    {
        float a = 0;
        a += feat_matrix.at(i).at(8);
        a += feat_matrix.at(i).at(9);
        a += feat_matrix.at(i).at(10);
        a += feat_matrix.at(i).at(11);
        alphas.push_back(a);
        minimum = min(minimum, a);
        maximum = max(maximum, a);
    }
    ofLogError() << minimum << " " << maximum;
    ofLogError() << feat_matrix.at(0).at(0);

    ys.resize(199);
    for (int i = 0; i < ys.size(); i++)
    {
        ofxNumpy::load(ofToDataPath(ofToString(i) + "tsneResult.npy"), ys.at(i));
    }
    ofDisableArbTex();
    softPoint.loadImage(ofToDataPath("Soft64.png"));
    softPoint.resize(16, 16);
    ofEnableArbTex();

    y = ys.back();
    minXY.x = maxXY.x = y.at(0).x;
    minXY.y = maxXY.y = y.at(0).y;
    for (auto& p : y)
    {
        minXY.x = min(minXY.x, p.x);
        maxXY.x = max(maxXY.x, p.x);
        minXY.y = min(minXY.y, p.y);
        maxXY.y = max(maxXY.y, p.y);
    }
    ofLogError() << minXY << " " << maxXY;
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableAlphaBlending();

    ofBackground(255, 255);
    ofSetColor(ofFloatColor::white);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    int count = 0;
    ofVec2f pPrev;
    //int index = (ofGetFrameNum()) % (ys.size() * 2);
    int index = floor(ofMap(mouseX, 0, ofGetWidth() + 1, 0, ys.size(), true));
    if (index < ys.size())
        y = ys.at(index);
    else
        y = ys.at(ys.size() * 2 - index - 1);

    for (auto& p : y)
    {
        //ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 0.1));
        //ofDrawCircle(p, 5 * 0.1f);

        if(ofGetKeyPressed(' '))
            ofSetColor(ofFloatColor::fromHsb(0, 1, 1, ofMap(alphas.at(count), 0, 5, 0, 0.2f, true)));
        else
            ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 0.2f));

        ofDrawCircle(ofMap(p.x, minXY.x, maxXY.x, 10, ofGetWidth() - 10), ofMap(p.y, minXY.y, maxXY.y, 10, ofGetHeight() - 10), 5);

        //softPoint.draw(p, 1, 1);
        //if(count > 0)
        //    ofLine(pPrev, p);
        pPrev = p;
        count++;
    }

    if (ofGetKeyPressed())
    {
        ofSaveScreen("screen.png");
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
