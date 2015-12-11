#include "ofApp.h"

#include "ofxNumpy.h"
#include "ofxPubSubOsc.h"

//--------------------------------------------------------------
void ofApp::setup() {
    std::function<void(ofxOscMessage &)> f = [&](ofxOscMessage& m) {
        samplePrev = sample;
        sample.x = m.getArgAsFloat(0);
        sample.y = m.getArgAsFloat(1);
        sampleIndex = m.getArgAsInt(2);

        yNew.push_back(sample);

        ofVec2f newPos;
        newPos.x = ofMap(sample.x, minXY.x, maxXY.x, 10, ofGetWidth() - 10);
        newPos.y = ofMap(sample.y, minXY.y, maxXY.y, 10, ofGetHeight() - 10);
    };
    ofxSubscribeOsc(8000, "/muse/tsne", f);
    sampleIndex = 0;
    ofSetWindowShape(640, 640);
    //ofSetWindowShape(560, 315);
    ofxNumpy::load("C:\\Users\\ocad\\Documents\\bci_workshop\\tsneResult.npy", y);

    cnpy::NpyArray t;
    size_t dim, n;
    double* data;
    vector<string> filenames;
    filenames.push_back("C:\\Users\\ocad\\Documents\\bci_workshop\\t0.npy");
    for (auto filename : filenames)
    {
        t = cnpy::npy_load(filename);
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
    }

    float minimum = 1000;
    float maximum = -1000;
    for (int i = 0; i < feat_matrix.size(); i++)
    {
        float a = 0;
//        a += feat_matrix.at(i).at(8);
//        a += feat_matrix.at(i).at(9);
//        a += feat_matrix.at(i).at(10);
//        a += feat_matrix.at(i).at(11);
        alphas.push_back(a);
        minimum = min(minimum, a);
        maximum = max(maximum, a);
    }
    ofLogError() << minimum << " " << maximum;
    ofLogError() << feat_matrix.at(0).at(0);

    ofDisableArbTex();
    softPoint.loadImage(ofToDataPath("Soft64.png"));
    softPoint.resize(16, 16);
    ofEnableArbTex();

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

    strings.setMode(OF_PRIMITIVE_LINES);
    for (int i = 0; i < y.size(); i++)
    {
        for (int j = i; j < y.size(); j++)
        {
            auto p0 = y.at(i);
            auto p1 = y.at(j);
            p0.x = ofMap(p0.x, minXY.x, maxXY.x, -ofGetWidth() * 0.5f + 10, ofGetWidth() * 0.5f - 10);
            p0.y = ofMap(p0.y, minXY.y, maxXY.y, -ofGetHeight() * 0.5f + 10, ofGetHeight() * 0.5f - 10);
            p1.x = ofMap(p1.x, minXY.x, maxXY.x, -ofGetWidth() * 0.5f + 10, ofGetWidth() * 0.5f - 10);
            p1.y = ofMap(p1.y, minXY.y, maxXY.y, -ofGetHeight() * 0.5f + 10, ofGetHeight() * 0.5f - 10);
            float dist = p0.distance(p1);
            float distThreshold = 100;
            if (dist < distThreshold)
            {
                strings.addVertex(p0);
                strings.addVertex(p1);
                //strings.addColor(ofFloatColor::black);
                //strings.addColor(ofFloatColor::black);
                strings.addColor(ofFloatColor(1, ofMap(dist, distThreshold, 0, 0, 0.5f)));
                strings.addColor(ofFloatColor(1, ofMap(dist, distThreshold, 0, 0, 0.5f)));
                strings.addIndex(strings.getNumVertices() - 2);
                strings.addIndex(strings.getNumVertices() - 1);
            }
        }
    }

    gui.setup();
    gui.add(sliderChannel.setup("Channel", 0, 0, 15));
    gui.add(sliderUpperLimit.setup("Upper Limit", 2, 0, 6));
    gui.add(toggleColor.setup("Color", false));
    gui.add(refreshSec.setup("Refresh Sec", 20, 10, 100));
    gui.add(mouseDebug.setup("Mouse Debug", false));
    gui.loadFromFile("settings.xml");
    drawGui = true;
}

//--------------------------------------------------------------
void ofApp::update(){
    if (mouseDebug && ofGetFrameNum() % 15 == 0)
    {
        ofVec2f newPos;
        newPos.x = mouseX;
        newPos.y = mouseY;
        newPosPrev = newPos;
    }

    if (yNew.size() >= refreshSec)
        yNew.clear();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableAlphaBlending();

    ofBackground(255, 255);
    //ofSetColor(ofFloatColor::white);
    //ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    int count = 0;
    ofVec2f pPrev;

    ofFloatColor c;

    ofPushMatrix();
    ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
    for (auto& p : y)
    {
        //ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 0.1));
        //ofDrawCircle(p, 5 * 0.1f);

        float radius = 10;
        if(toggleColor)
            ofSetColor(ofFloatColor::fromHsb(0, 1, 1, ofMap(feat_matrix.at(count).at(sliderChannel), 0, sliderUpperLimit, 0, 0.2f, true)));
        else
            ofSetColor(ofFloatColor::fromHsb((float)count / y.size() * 0.75f, 1, 1, 0.2f));

        ofVec2f newPos;
        newPos.x = ofMap(p.x, minXY.x, maxXY.x, -ofGetWidth() * 0.5f + 10, ofGetWidth() * 0.5f - 10);
        newPos.y = ofMap(p.y, minXY.y, maxXY.y, -ofGetHeight() * 0.5f + 10, ofGetHeight() * 0.5f - 10);

        //softPoint.draw(newPos, radius * 2, radius * 2);
        //if(count > 0)
        //    ofLine(pPrev, newPos);
        //pPrev = newPos;

        count++;
    }

    count = 0;
    for (auto& p : yNew)
    {
        float radius = 10;
        if (toggleColor)
            ofSetColor(ofFloatColor::fromHsb(0, 1, 1, ofMap(feat_matrix.at(count).at(sliderChannel), 0, sliderUpperLimit, 0, 0.2f, true)));
        else
            ofSetColor(ofFloatColor::fromHsb((float)count / refreshSec * 0.75f, 1, 1, 0.2f));

        ofVec2f newPos;
        newPos.x = ofMap(p.x, minXY.x, maxXY.x, -ofGetWidth() * 0.5f + 10, ofGetWidth() * 0.5f - 10);
        newPos.y = ofMap(p.y, minXY.y, maxXY.y, -ofGetHeight() * 0.5f + 10, ofGetHeight() * 0.5f - 10);

        softPoint.draw(newPos, radius * 2, radius * 2);
        count++;
    }
    ofPopMatrix();

    if(drawGui)
        gui.draw();

    if (ofGetKeyPressed())
    {
        //ofSaveScreen("screen.png");
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_TAB)
        drawGui = !drawGui;
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
