#pragma once

#include "ofMain.h"
#include "ofxFFTLive.h"
#include "ofxGui.h"
#include "ofxFluid.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

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
		
        vector<ofVec2f> y;
        vector<vector<float> > feat_matrix;
        vector<float> alphas;
        ofImage softPoint;

        ofVec2f minXY, maxXY;

        ofSoundStream* soundStream;
        ofxFFTLive fftLive;

        ofxPanel gui;
        ofxIntSlider sliderChannel;
        ofxFloatSlider sliderUpperLimit;
        ofxToggle toggleColor;
        ofxIntSlider sliderFrame;
        ofxIntSlider sliderUpperFft;
        ofxIntSlider sliderLowerFft;
        ofxFloatSlider fluidDissipation;
        ofxFloatSlider velocityDissipation;
        ofxFloatSlider velocityCoeff;
        ofxFloatSlider fluidTemp;
        ofxIntSlider refreshSec;

        const int fftN = 64;

        ofVec2f sample;
        ofVec2f samplePrev;
        ofVec2f interpolatedSample;
        int sampleIndex;

        ofxFluid fluid;
};
