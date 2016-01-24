#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxAutoReloadedShader.h"

#include "Math/MathUtils.h"

#include "Utils/Cameras/ofxFirstPersonCamera.h"
#include "Utils/DrawingHelpers.h"
#include "Utils/ofTrueTypeFontExt.h"
#include "Utils/FboPingPong.h"

#include "ParticleSystemGPU.h"

#include "ofxOsc.h"
#include "ofxFFTLive.h"

class ofApp : public ofBaseApp
{
	public:
		
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
	
		float					time;
		float					timeStep;
	
		ParticleSystemGPU		particles;
	
		ofxFirstPersonCamera	camera;
	
		ofTrueTypeFontExt		fontSmall;
	
		bool					drawGui;

        ofxOscReceiver receiver;
        float prevBv;

        ofSoundStream* soundStream;
        ofxFFTLive fftLive;

        ofFbo fbo;
        ofImage image;
        int count;
};