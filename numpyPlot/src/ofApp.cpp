#include "ofApp.h"

#include "ofxNumpy.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetWindowShape(640, 640);
    //ofSetWindowShape(560, 315);
    ofxNumpy::load(ofToDataPath("tsneResult.npy"), y);

    cnpy::NpyArray t;
    size_t dim, n;
    double* data;
    vector<string> filenames;
    filenames.push_back("t0.npy");
    filenames.push_back("t1.npy");
    filenames.push_back("t2.npy");
    for (auto filename : filenames)
    {
        t = cnpy::npy_load(ofToDataPath(filename));
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

    soundStream = new ofSoundStream();
    soundStream->listDevices();
    soundStream->setDeviceID(3);
    soundStream->setup(&fftLive, 0, 1, 44100, fftLive.getBufferSize(), 4);
    fftLive.setMirrorData(false);
    fftLive.soundStream = soundStream;

    gui.setup();
    gui.add(sliderChannel.setup("Channel", 0, 0, 15));
    gui.add(sliderUpperLimit.setup("Upper Limit", 2, 0, 6));
    gui.add(sliderFrame.setup("Frame", ys.size() * 0.75f, 0, ys.size()));
    gui.add(toggleColor.setup("Color", false));
    gui.add(sliderUpperFft.setup("Upper Fft", 30, 1, fftN));
    gui.add(sliderLowerFft.setup("Lower Fft", 0, 0, fftN - 1));
}

//--------------------------------------------------------------
void ofApp::update(){
    fftLive.update();
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
    int index = sliderFrame;
    if (index < ys.size())
        y = ys.at(index);
    else
        y = ys.at(ys.size() * 2 - index - 1);

    float * audioData = new float[fftN];
    fftLive.getFftPeakData(audioData, fftN);

    ofPushMatrix();
    ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
    for (auto& p : y)
    {
        //ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 0.1));
        //ofDrawCircle(p, 5 * 0.1f);

        if(toggleColor)
            ofSetColor(ofFloatColor::fromHsb(0, 1, 1, ofMap(feat_matrix.at(count).at(sliderChannel), 0, sliderUpperLimit, 0, 0.2f, true)));
        else
            ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 0.2f));

        ofVec2f newPos;
        newPos.x = ofMap(p.x, minXY.x, maxXY.x, -ofGetWidth() * 0.5f + 10, ofGetWidth() * 0.5f - 10);
        newPos.y = ofMap(p.y, minXY.y, maxXY.y, -ofGetHeight() * 0.5f + 10, ofGetHeight() * 0.5f - 10);
        float angle = newPos.angle(ofVec2f(0, 1));
        while (angle < 0) angle += 360;
        float audioF = ofMap(angle, 0, 360, sliderLowerFft, sliderUpperFft);
        int audioM = floor(audioF);
        int audioN = audioM + 1;
        if (audioM >= fftN - 1)
        {
            audioM = fftN - 1;
            audioN = 0;
        }
        float coeff = audioData[audioM] * (audioF - audioM) + audioData[audioN] * abs(audioN - audioF);
        newPos *= ofMap(coeff, 0, 1, 0.5f, 1.5f);
        ofDrawCircle(newPos, 5);

        //softPoint.draw(newPos, 4, 4);
        //if(count > 0)
            //ofLine(pPrev, p);
        //pPrev = newPos;
        count++;
    }
    ofPopMatrix();

    //fftLive.draw(10, 30);

    gui.draw();

    if (ofGetKeyPressed())
    {
        //ofSaveScreen("screen.png");
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
