#pragma once

#include "ofMain.h"
#include "ofxGui.h"

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
        vector<ofVec2f> yNew;
        vector<vector<float> > feat_matrix;
        vector<float> alphas;
        ofImage softPoint;

        ofVec2f minXY, maxXY;

        ofxPanel gui;
        ofxIntSlider sliderChannel;
        ofxFloatSlider sliderUpperLimit;
        ofxToggle toggleColor;
        ofxIntSlider sliderFrame;
        ofxIntSlider refreshSec;
        ofxToggle mouseDebug;

        ofVec2f sample;
        ofVec2f samplePrev;
        ofVec2f newPosPrev;
        int sampleIndex;

        ofVboMesh strings;
        bool drawGui;
};
