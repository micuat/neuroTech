#include "ofApp.h"

//-----------------------------------------------------------------------------------------
//
void ofApp::setup()
{
	//ofSetLogLevel( OF_LOG_VERBOSE );
	
	ofSetFrameRate( 15 );
	
	fontSmall.loadFont("Fonts/DIN.otf", 8 );
	
	ofxGuiSetDefaultWidth( 300 );

	int texSize = 128;
	particles.init( texSize ); // number of particles is (texSize * texSize)
	
	// Give us a starting point for the camera
	camera.setNearClip(0.01f);
	camera.setPosition( 0, 0, 1.5f );
	camera.setMovementMaxSpeed( 0.01f );

	time = 0.0f;
	timeStep = 1.0f / 60.0f;
	
	drawGui = false;

    //receiver.setup(8000);
    receiver.setup(9999);
    particles.spawnState = 1;

    //soundStream = new ofSoundStream();
    //soundStream->listDevices();
    //soundStream->setDeviceID(4);
    //soundStream->setup(&fftLive, 0, 1, 44100, fftLive.getBufferSize(), 4);
    fftLive.setMirrorData(false);
    fftLive.setup();
    //fftLive.soundStream = soundStream;

    fbo.allocate(1920, 1080, GL_RGBA);
    image.allocate(1920, 1080, OF_IMAGE_COLOR_ALPHA);
    count = 0;
}


//-----------------------------------------------------------------------------------------
//
void ofApp::update()
{
    fftLive.update();
    const int fftN = 16;
    particles.fftPeaks = new float[fftN];
    fftLive.getFftPeakData(particles.fftPeaks, fftN);

	// Update time, this let's us hit space and slow down time, even reverse it.
	if( ofGetKeyPressed(' ') ) { timeStep = ofLerp( timeStep, ofMap( ofGetMouseX(), 0, ofGetWidth(), -(1.0f/60.0f), (1.0f/60.0f) ), 0.1f );}
	else { timeStep = ofLerp( timeStep, 1.0f / 60.0f, 0.1f ); }
	time += timeStep;

    if (ofGetKeyPressed('p'))
    {
        particles.spawnState = 1;
    }
    else
    {
        particles.spawnState = 0;
    }

    if (ofGetKeyPressed('b'))
    {
        particles.lastBang = ofGetElapsedTimef();
    }
    while (receiver.hasWaitingMessages()) {
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);

        // check for mouse moved message
        /*
        if (m.getAddress() == "/muse/classifier/open") {
            particles.spawnState = 0;
        }
        if (m.getAddress() == "/muse/classifier/close") {
            particles.spawnState = 1;
        }
        */

        if (m.getAddress() == "/popol/bang") {
            particles.lastBang = ofGetElapsedTimef();
        }
        if (m.getAddress() == "/muse/classifier/close") {
            particles.spawnState = 1;
        }

    }
}




//-----------------------------------------------------------------------------------------
//
void ofApp::draw()
{
    ofBackground(0);
    fbo.begin();

    float timeDiff = ofMap(ofGetElapsedTimef() - particles.lastBang, 0, particles.bangTime, 0, 1);
    //if(timeDiff < 1)
    //ofBackgroundGradient(ofColor(ofMap(timeDiff, 0, 0.5, 255, 240, true)), ofColor(0, 0, 0), OF_GRADIENT_CIRCULAR);
    //else
        ofBackground(0, 0);

	particles.update( time, timeStep );
	
    //camera.rotateAround(ofGetElapsedTimef() * 0.01f, ofVec3f(0, 1, 0), ofVec3f(0, 0.4f, 0));
    camera.lookAt(ofVec3f(0, 0, 0));
	camera.begin();
	
		// draw a grid on the floor
		ofSetColor( ofColor(60) );
		ofPushMatrix();
			ofRotate(90, 0, 0, -1);
			//ofDrawGridPlane( 0.5, 12, false );
		ofPopMatrix();
		 
		ofSetColor( ofColor::white );
	
        particles.draw( &camera );

	camera.end();
	
	ofDisableDepthTest();
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	ofSetColor( ofColor::white );
	
    fbo.end();
    fbo.draw(0, 0, ofGetWidth(), ofGetHeight());

    if (particles.recording)
    {
        fbo.readToPixels(image.getPixels());
        image.save(ofToDataPath("screenshots/" + ofToString(count, 3, '0') + ".png"));
        count++;
    }

    int size = 196;
	//particles.particleDataFbo.source()->getTextureReference(0).draw( 0,	 0, size, size );

	if( drawGui )
	{
		particles.gui.draw();
	}
	
	ofDisableDepthTest();
	//fontSmall.drawStringShadowed(ofToString(ofGetFrameRate(),2), ofGetWidth()-35, ofGetHeight() - 6, ofColor::whiteSmoke, ofColor::black );
}

//-----------------------------------------------------------------------------------------
//
void ofApp::keyPressed(int key)
{
	if( key == OF_KEY_TAB )
	{
		drawGui = !drawGui;
        if(drawGui)
            ofShowCursor();
        else
            ofHideCursor();
	}
	else if( key == 'f' )
	{
		ofToggleFullscreen();
	}
	else if( key == OF_KEY_LEFT  )
	{
	}
	else if( key == OF_KEY_RIGHT  )
	{
	}
}
