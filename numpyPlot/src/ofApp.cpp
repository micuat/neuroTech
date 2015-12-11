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

        ofVec2f newPos;
        newPos.x = ofMap(sample.x, minXY.x, maxXY.x, 10, ofGetWidth() - 10);
        newPos.y = ofMap(sample.y, minXY.y, maxXY.y, 10, ofGetHeight() - 10);
        auto c = ofFloatColor::fromHsb((float)sampleIndex / y.size() * 0.75f, 1, 1);
        fluid.addTemporalForce(newPos, (newPos - newPosPrev) * velocityCoeff, c * fluidAlpha, fluidTemp);
        newPosPrev = newPos;
        //y.push_back(sample);
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

    soundStream = new ofSoundStream();
    soundStream->listDevices();
    soundStream->setDeviceID(2);
    soundStream->setup(&fftLive, 0, 1, 44100, fftLive.getBufferSize(), 4);
    fftLive.setMirrorData(false);
    fftLive.soundStream = soundStream;

    gui.setup();
    gui.add(sliderChannel.setup("Channel", 0, 0, 15));
    gui.add(sliderUpperLimit.setup("Upper Limit", 2, 0, 6));
    gui.add(toggleColor.setup("Color", false));
    gui.add(sliderUpperFft.setup("Upper Fft", 30, 1, fftN));
    gui.add(sliderLowerFft.setup("Lower Fft", 0, 0, fftN - 1));
    gui.add(fluidDissipation.setup("Fluid Dissipation", 0.99f, 0.9f, 1));
    gui.add(velocityDissipation.setup("Velocity Dissipation", 0.99f, 0.9f, 0.9999f));
    gui.add(velocityCoeff.setup("Velocity Coeff", 0.1f, 0, 2));
    gui.add(fluidTemp.setup("Fluid Temp", 15, 5, 30));
    gui.add(fluidAlpha.setup("Fluid Alpha", 0.5f, 0, 1));
    gui.add(refreshSec.setup("Refresh Sec", 5, 1, 10));
    gui.add(mouseDebug.setup("Mouse Debug", false));
    gui.loadFromFile("settings.xml");
    drawGui = true;

    fluid.dissipation = 1;
    fluid.velocityDissipation = 0.99f;

    fluid.allocate(640, 640);

    fluid.begin();
    ofBackground(255, 255);
    ofSetColor(0, 255);
    ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
    strings.draw();
    int nn = 3;
    for (int i = -nn; i <= nn; i++)
        for (int j = -nn; j <= nn; j++)
        {
            ofPushMatrix();
            ofTranslate(i, j);
            strings.draw();
            ofPopMatrix();
        }
    fluid.end();
    fluid.setUseObstacles(false);

    fluid.setGravity(ofPoint());

    fbo.allocate(640, 640, GL_RGBA);
    fbo.begin();
    ofBackground(0, 255);
    ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
    strings.draw();
    fbo.end();
}

//--------------------------------------------------------------
void ofApp::update(){
    if (mouseDebug && ofGetFrameNum() % 15 == 0)
    {
        ofVec2f newPos;
        newPos.x = mouseX;
        newPos.y = mouseY;
        auto c = ofFloatColor::fromHsb(ofMap(sinf(ofGetElapsedTimef()), -1, 1, 0, 0.75f), 1, 1, 0.5f);
        fluid.addTemporalForce(newPos, (newPos - newPosPrev) * velocityCoeff, c * fluidAlpha, fluidTemp);
        newPosPrev = newPos;
    }
    fftLive.update();
    fluid.update();
    //fluid.dissipation = fluidDissipation;
    //fluid.velocityDissipation = velocityDissipation;

    if (ofGetFrameNum() % (refreshSec * 60) == 0)
    {
        fluid.clear();
        //fluid.addColor(fbo);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableAlphaBlending();

    ofBackground(0, 255);
    //ofSetColor(ofFloatColor::white);
    //ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    int count = 0;
    ofVec2f pPrev;

    float * audioData = new float[fftN];
    fftLive.getFftPeakData(audioData, fftN);

    ofFloatColor c;

    fluid.draw();
    ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    fbo.draw(0, 0);
    ofDisableBlendMode();

    ofPushMatrix();
    ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
    //ofEnableBlendMode(OF_BLENDMODE_MULTIPLY);
    ofEnableAlphaBlending();
    ofSetColor(255, 70);
    //strings.draw();
    ofSetColor(255, 255);
    //ofDisableBlendMode();
    for (auto& p : y)
    {
        //ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 0.1));
        //ofDrawCircle(p, 5 * 0.1f);

        float radius = 10;
        if(toggleColor)
            ofSetColor(ofFloatColor::fromHsb(0, 1, 1, ofMap(feat_matrix.at(count).at(sliderChannel), 0, sliderUpperLimit, 0, 0.2f, true)));
        else
            if (count == sampleIndex)
            {
                //radius = 10;
                //ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 1));
                ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 0.2f));
            }
            else
                ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 0.2f));

        ofSetColor(ofFloatColor(1, 0.25f));
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
        //newPos *= ofMap(coeff, 0, 1, 0.5f, 1.5f);
        //ofDrawCircle(newPos, radius);

        //softPoint.draw(newPos, radius * 2, radius * 2);
        //if(count > 0)
        //    ofLine(pPrev, newPos);
        //pPrev = newPos;

        if (count == sampleIndex)
        {
            interpolatedSample.x = interpolatedSample.x * 0.9f + newPos.x * 0.1f;
            interpolatedSample.y = interpolatedSample.y * 0.9f + newPos.y * 0.1f;
            c = ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 1);
        }
        count++;
    }

    ofSetColor(c);
    //ofDrawCircle(interpolatedSample * 2, 5);
    //softPoint.draw(interpolatedSample, 10 * 2, 10 * 2);
    ofPopMatrix();

    //fftLive.draw(10, 30);

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
