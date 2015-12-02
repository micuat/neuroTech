#include "ofApp.h"

#include "ofxPubSubOsc.h"

using namespace ofxCv;

void ofApp::setup() {
	ofSetVerticalSync(true);
	cam.initGrabber(640, 360);
	tracker.setup();

    //receiver.setup(8000);

    eegState = 0.5f;

    ofxPublishOsc("localhost", 8100, "/face/jaw", openness);
    ofxPublishOsc("localhost", 8100, "/face/bend", bend);
    ofxPublishOsc("localhost", 8100, "/face/direction", dir);
    openness = 0;
    bend = 0;
    dir = 0;
}

void ofApp::update() {
	cam.update();
	if(cam.isFrameNew()) {
		tracker.update(toCv(cam));
        openness = ofMap(tracker.getGesture(ofxFaceTracker::Gesture::JAW_OPENNESS), 22, 25, 0, 1, true);
        bend = ofMap(tracker.getGesture(ofxFaceTracker::Gesture::MOUTH_WIDTH), 15, 11, 0, 1, true);
        dir = ofMap(tracker.getOrientation().y, -0.3f, 0.3f, -1, 1, true);
        ofLogError() << openness << " " << bend << " " << dir;
	}
/*    while (receiver.hasWaitingMessages()) {
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);

        // check for mouse moved message
        if (m.getAddress() == "/muse/classifier/1") { // close eyes
            state.x = ofClamp(state.x + 0.1f, 0, 1);

            eegState = ofClamp(eegState - 0.1f, 0, 1);
        }
        if (m.getAddress() == "/muse/classifier/2") { // stare
            state.x = ofClamp(state.x - 0.1f, 0, 1);
            state.y = ofClamp(state.y - 0.1f, 0, 1);

            if(eegState < 0.5f)
                eegState = ofClamp(eegState + 0.1f, 0, 0.5f);
            else
                eegState = ofClamp(eegState - 0.1f, 0.5f, 1);
        }
        if (m.getAddress() == "/muse/classifier/3") { // distract
            state.y = ofClamp(state.y + 0.1f, 0, 1);

            eegState = ofClamp(eegState + 0.1f, 0, 1);
        }
    }*/
}

void ofApp::draw() {
	ofSetColor(255);
    ofScale(2, 2);
	cam.draw(0, 0);

    //ofDrawCircle(state * 100, 20);
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);

    ofSetColor(ofFloatColor::fromHsb(eegState * 0.5f, 1, 1));
	if(tracker.getFound()) {
		tracker.draw();
        return;

		ofMesh objectMesh = tracker.getObjectMesh();
		ofMesh meanMesh = tracker.getMeanObjectMesh();
        ofPushMatrix();
        ofTranslate(tracker.getImageFeature(ofxFaceTracker::LEFT_EYEBROW)[3] + ofVec2f(0, -30));
        ofRotate(-80);
        ofScale(1, 0.75f);
        ofCircle(ofVec2f(), 20);
        ofPopMatrix();
        ofPushMatrix();
        auto eye = tracker.getImageFeature(ofxFaceTracker::RIGHT_EYEBROW);
        ofTranslate(eye[eye.size() - 3] + ofVec2f(0, -30));
        ofRotate(80);
        ofScale(1, 0.75f);
        ofCircle(ofVec2f(), 20);
        ofPopMatrix();

        /*
		ofSetupScreenOrtho(640, 480, -1000, 1000);
		ofTranslate(100, 100);
		ofScale(5,5,5);
		cam.getTextureReference().bind();
		meanMesh.draw();
		cam.getTextureReference().unbind();
        */
	}
}

void ofApp::exit() {
}

void ofApp::keyPressed(int key) {
    if (key == 'r') {
        tracker.reset();
    }
}
