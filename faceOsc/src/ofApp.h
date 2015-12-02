#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxFaceTracker.h"
#include "ofxOsc.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
    void draw();
    void exit();
    void keyPressed(int key);
	
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
		
	ofEasyCam easyCam;

    ofxOscReceiver receiver;
    ofxOscSender sender;

    ofVec2f state;
    float eegState;

    float openness;
    float bend;
    float dir;
};
