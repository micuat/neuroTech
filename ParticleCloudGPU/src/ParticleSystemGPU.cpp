//
//  ParticleSystemGPU.cpp
//  ParticlesGPU
//
//  Created by Andreas Müller on 11/01/2015.
//
//

#include "ParticleSystemGPU.h"
#include "ofxPubSubOsc.h"

//-----------------------------------------------------------------------------------------
//
void ParticleSystemGPU::init( int _texSize )
{
	string xmlSettingsPath = "Settings/Main.xml";
	gui.setup("Main", xmlSettingsPath);
    gui.add(label.setup("OSC", "/axon/particles + name"));
    gui.add(label2.setup("Port", "9990"));
	gui.add(particleMaxAge.set("/age", 10.0f, 0.0f, 20.0f));
	gui.add(particleSpawnFreq.set("/spawn/spatial", 8.0f, 1.0f, 16.0f));
	gui.add(noiseMagnitude.set("/noise/magnitude", 0.075, 0.01f, 2.0f));
	gui.add(noisePositionScale.set("/noise/spatial", 1.5f, 0.01f, 10.0f));
	gui.add(noiseTimeScale.set("/noise/time", 1.0 / 4000.0, 0.001f, 1.0f));
	gui.add(noisePersistence.set("/noise/persistence", 0.2, 0.001f, 1.0f));
    gui.add(baseSpeed.set("/wind", ofVec3f(0.5, 0, 0), ofVec3f(-2, -2, -2), ofVec3f(2, 2, 2)));
    gui.add(baseSpeedBang.set("/wind/bang", ofVec3f(2.5, 0, 0), ofVec3f(-2, -2, -2), ofVec3f(2, 2, 2)));
    gui.add(startColor.set("/color/start", ofColor::white, ofColor(0,0,0,0), ofColor(255,255,255,255)));
	gui.add(endColor.set("/color/end", ofColor(0,0,0,0), ofColor(0,0,0,0), ofColor(255,255,255,255)));
	gui.add(particleSize.set("/size/max", 0.01, 0.0001f, 0.05f));
	gui.add(particleSizeMin.set("/size/min", 0.001, 0.0001f, 0.05f));
	gui.add(stringTheory.set("/string", 0.2f, 0.0001f, 1.0f));
    gui.add(bangTime.set("/bang/duration", 0.5f, 0.0001f, 1.0f));
    gui.add(griding.set("/grid", 0, 0, 1));
    gui.add(cylindering.set("/cylinder", 0, 0, 1));
    gui.add(triangles.set("/triangles", false));
    gui.add(spawning.set("/spawn", true));
    gui.add(scaling.set("/scaling", 1, 0.01f, 1));
    gui.add(fftThreshold.set("/fft/threshold", 1, 0, 2));
    gui.add(fftChannel.set("/fft/channel", 10, 0, 15));
    gui.add(recording.set("record", false));
    //gui.add( twistNoiseTimeScale.set("Twist Noise Time Scale", 0.01, 0.0f, 0.5f) );
	//gui.add( twistNoisePosScale.set("Twist Noise Pos Scale", 0.25, 0.0f, 2.0f) );
	//gui.add( twistMinAng.set("Twist Min Ang", -1, -5, 5) );
	//gui.add( twistMaxAng.set("Twist Max Ang", 2.5, -5, 5) );
	
	gui.loadFromFile( xmlSettingsPath );
	
    ofxSubscribeOsc(9990, "/axon/particles/bang", [&](ofxOscMessage& m) {
        lastBang = ofGetElapsedTimef();
    });
    auto f = [](ofParameter<float>& p) {
        ofxSubscribeOsc(9990, "/axon/particles" + p.getName(), [&](ofxOscMessage& m) {
            p = ofMap(m.getArgAsInt(0), 0, 127, p.getMin(), p.getMax());
        });
    };
    f(particleMaxAge);
    f(particleSpawnFreq);
    f(noiseMagnitude);
    f(noisePositionScale);
    f(noiseTimeScale);
    f(noisePersistence);
    f(particleSize);
    f(particleSizeMin);
    f(stringTheory);
    f(bangTime);
    f(griding);
    f(cylindering);
    f(scaling);
    f(fftThreshold);
    auto fv = [](ofParameter<ofVec3f>& p) {
        ofxSubscribeOsc(9990, "/axon/particles" + p.getName() + "/x", [&](ofxOscMessage& m) {
            ofVec3f v = p.get();
            v.x = ofMap(m.getArgAsInt(0), 0, 127, p.getMin().x, p.getMax().x);
            p.set(v);
        });
        ofxSubscribeOsc(9990, "/axon/particles" + p.getName() + "/y", [&](ofxOscMessage& m) {
            ofVec3f v = p.get();
            v.y = ofMap(m.getArgAsInt(0), 0, 127, p.getMin().y, p.getMax().y);
            p.set(v);
        });
        ofxSubscribeOsc(9990, "/axon/particles" + p.getName() + "/z", [&](ofxOscMessage& m) {
            ofVec3f v = p.get();
            v.z = ofMap(m.getArgAsInt(0), 0, 127, p.getMin().z, p.getMax().z);
            p.set(v);
        });
    };
    fv(baseSpeed);
    fv(baseSpeedBang);
    auto fc = [](ofParameter<ofColor>& p) {
        ofxSubscribeOsc(9990, "/axon/particles" + p.getName() + "/r", [&](ofxOscMessage& m) {
            ofColor c = p.get();
            c.r = ofMap(m.getArgAsInt(0), 0, 127, p.getMin().r, p.getMax().r);
            p.set(c);
        });
        ofxSubscribeOsc(9990, "/axon/particles" + p.getName() + "/g", [&](ofxOscMessage& m) {
            ofColor c = p.get();
            c.g = ofMap(m.getArgAsInt(0), 0, 127, p.getMin().g, p.getMax().g);
            p.set(c);
        });
        ofxSubscribeOsc(9990, "/axon/particles" + p.getName() + "/b", [&](ofxOscMessage& m) {
            ofColor c = p.get();
            c.b = ofMap(m.getArgAsInt(0), 0, 127, p.getMin().b, p.getMax().b);
            p.set(c);
        });
        ofxSubscribeOsc(9990, "/axon/particles" + p.getName() + "/a", [&](ofxOscMessage& m) {
            ofColor c = p.get();
            c.a = ofMap(m.getArgAsInt(0), 0, 127, p.getMin().a, p.getMax().a);
            p.set(c);
        });
    };
    fc(startColor);
    fc(endColor);

	// To use a texture with point sprites it will need to be created as a GL_TEXTURE_2D and not a GL_TEXTURE_RECTANGLE
	// This way it has texture coordinates in the range 0..1 instead of 0..imagewith
	ofDisableArbTex();
	particleImage.loadImage( "Textures/Soft64.png" );
    particleImage.resize(16, 16);
	ofEnableArbTex();

	// Load shaders
#ifdef TARGET_OPENGLES
	particleUpdate.load("Shaders/Particles/GLES/Update");
	particleDrawUnsorted.load("Shaders/Particles/GLES/DrawUnsorted");
#else
	particleUpdate.load("Shaders/Particles/GL2/Update");
	particleDrawUnsorted.load("Shaders/Particles/GL2/DrawUnsorted");
#endif


	// Set how many particles we are going to have, this is based on data texture size
	textureSize = 128;
	numParticles = textureSize * textureSize;

	// Allocate buffers
	ofFbo::Settings fboSettings;
	fboSettings.width  = textureSize;
	fboSettings.height = textureSize;
	
	// We can create several color buffers for one FBO if we want to store velocity for instance,
	// then draw to them simultaneously from a shader using gl_FragData[0], gl_FragData[1], etc.
	fboSettings.numColorbuffers = 1;
	
	fboSettings.useDepth = false;
	fboSettings.internalformat = GL_RGBA32F;	// Gotta store the data as floats, they won't be clamped to 0..1
	fboSettings.textureTarget = GL_TEXTURE_2D;
	fboSettings.wrapModeHorizontal = GL_CLAMP_TO_EDGE;
	fboSettings.wrapModeVertical = GL_CLAMP_TO_EDGE;
	fboSettings.minFilter = GL_NEAREST; // No interpolation, that would mess up data reads later!
	fboSettings.maxFilter = GL_NEAREST;
	
	ofDisableTextureEdgeHack();
	
		particleDataFbo.allocate( fboSettings );
	
	ofEnableTextureEdgeHack();
	
	//	Buffer 1: XYZ pos, W age
	//	Buffer 2: XYZ vel

	// Initialise the starting and static data
	ofVec4f* startPositionsAndAge = new ofVec4f[numParticles];
	
	// We also create a vbo that has the texture coordinate for each particle's data
	particlePoints.setMode( OF_PRIMITIVE_POINTS );
	
	int tmpIndex = 0;
	for( int y = 0; y < textureSize; y++ )
	{
		for( int x = 0; x < textureSize; x++ )
		{
			float size = 0;
			ofVec3f pos (MathUtils::randomPointOnSphere() * 0.1);
			pos.set( ofRandom(-1,1), ofRandom(0,2), ofRandom(-1,1) );
            pos *= 0.001f;
			float startAge = ofRandom( particleMaxAge );
			
			startPositionsAndAge[tmpIndex] = ofVec4f( pos.x, pos.y, pos.z, startAge );
			
			ofVec2f texCoord;
			texCoord.x = ofMap( x + 0.5f,	0, textureSize,	0.0f, 1.0f ); // the original source has a  '+ 0.5' in it, to get the ceil?
			texCoord.y = ofMap( y + 0.5f,	0, textureSize,	0.0f, 1.0f );
			
			particlePoints.addVertex( ofVec3f(x,y,0) ); // this cuould be 0,0,0 as we won't use it, but put somehting in here so you can draw it without a shader to confirm it draws
			particlePoints.addTexCoord( texCoord );
			tmpIndex++;
		}
	}
    particlePointsEast = particlePoints;
    particlePointsSouth = particlePoints;
    particlePointsWest = particlePoints;
    particlePointsNorth = particlePoints;
    particlePointsTri = particlePoints;
    for (int y = 0; y < textureSize; y++)
    {
        for (int x = 0; x < textureSize - 2; x++)
        {
            particlePointsEast.addIndex(y * textureSize + x);
        }
    }
    for (int x = 0; x < textureSize; x++)
    {
        for (int y = 0; y < textureSize - 2; y++)
        {
            particlePointsSouth.addIndex(y * textureSize + x);
        }
    }
    for (int y = 0; y < textureSize; y++)
    {
        for (int x = 1; x < textureSize - 1; x++)
        {
            particlePointsWest.addIndex(y * textureSize + x);
        }
    }
    for (int x = 0; x < textureSize; x++)
    {
        for (int y = 1; y < textureSize - 1; y++)
        {
            particlePointsNorth.addIndex(y * textureSize + x);
        }
    }
    for (int y = 0; y < textureSize - 1; y++)
    {
        for (int x = 0; x < textureSize - 1; x++)
        {
            particlePointsTri.addIndex(y * textureSize + x);
            particlePointsTri.addIndex((y+1) * textureSize + x);
            particlePointsTri.addIndex(y * textureSize + x+1);
            particlePointsTri.addIndex(y * textureSize + x+1);
            particlePointsTri.addIndex((y+1) * textureSize + x);
            particlePointsTri.addIndex((y+1) * textureSize + x+1);
        }
    }

	// Upload it to the source texture
	particleDataFbo.source()->getTextureReference(0).loadData( (float*)&startPositionsAndAge[0].x,	 textureSize, textureSize, GL_RGBA );

    lastBang = -100;
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemGPU::update( float _time, float _timeStep )
{
    if (fftPeaks != NULL)
    {
        //ofLogError() << fftPeaks[fftChannel];
        if(fftPeaks[fftChannel] > fftThreshold)
            lastBang = ofGetElapsedTimef();
    }
    float timeDiff = ofMap(ofGetElapsedTimef() - lastBang, 0, bangTime, 0, 1);

    ofEnableBlendMode( OF_BLENDMODE_DISABLED ); // Important! We just want to write the data as is to the target fbo
	
	particleDataFbo.dest()->begin();
	
		particleDataFbo.dest()->activateAllDrawBuffers(); // if we have multiple color buffers in our FBO we need this to activate all of them
		
		particleUpdate.begin();
	
			particleUpdate.setUniformTexture( "u_positionAndAgeTex",	particleDataFbo.source()->getTextureReference(0), 0 );
			//particleUpdate.setUniformTexture( "velocityTex",		particleDataFbo.source()->getTextureReference(1), 1 );
			
			particleUpdate.setUniform1f("u_time", _time );
			particleUpdate.setUniform1f("u_timeStep", _timeStep );
			
			particleUpdate.setUniform1f("u_particleMaxAge", particleMaxAge );
			
			particleUpdate.setUniform1f("u_particleSpawnFreq", particleSpawnFreq );

			particleUpdate.setUniform1f("u_noisePositionScale", noisePositionScale );
			particleUpdate.setUniform1f("u_noiseTimeScale", noiseTimeScale );
			particleUpdate.setUniform1f("u_noisePersistence", noisePersistence );
			particleUpdate.setUniform1f("u_noiseMagnitude", noiseMagnitude );
            if (timeDiff < 1)
            {
                float x = ofMap(timeDiff, 0, 1, baseSpeedBang.get().x, baseSpeed.get().x);
                float y = ofMap(timeDiff, 0, 1, baseSpeedBang.get().y, baseSpeed.get().y);
                float z = ofMap(timeDiff, 0, 1, baseSpeedBang.get().z, baseSpeed.get().z);
                particleUpdate.setUniform3f("u_wind", x, y, z);
            }
            else
            {
                particleUpdate.setUniform3f("u_wind", baseSpeed.get().x, baseSpeed.get().y, baseSpeed.get().z);
            }
			
            particleUpdate.setUniform1i("u_spawnParticles", spawnState || spawning);

            particleUpdate.setUniform1f("u_griding", griding);
            particleUpdate.setUniform1f("u_cylindering", cylindering);

            particleUpdate.setUniform1f("u_scaling", scaling);

            particleDataFbo.source()->draw(0,0);
		
		particleUpdate.end();
		
	particleDataFbo.dest()->end();
	
	particleDataFbo.swap();
}

//-----------------------------------------------------------------------------------------
//
void ParticleSystemGPU::draw( ofCamera* _camera )
{
    ofFloatColor particleStartCol = startColor.get();
	ofFloatColor particleEndCol = endColor.get();

    float timeDiff = ofMap(ofGetElapsedTimef() - lastBang, 0, bangTime, 0, 1);
   
    ofSetColor( ofColor::white );
	//ofEnableBlendMode( OF_BLENDMODE_ADD );
	ofEnableBlendMode( OF_BLENDMODE_ALPHA );
	
	ofEnablePointSprites();
	
	particleDrawUnsorted.begin();

		particleDrawUnsorted.setUniform2f("u_resolution", particleImage.getWidth(), particleImage.getHeight() );

		particleDrawUnsorted.setUniformTexture("u_particleImageTexture", particleImage.getTextureReference(), 0 );
		particleDrawUnsorted.setUniformTexture("u_particleDataTexture", particleDataFbo.source()->getTextureReference(), 1 );
		
		particleDrawUnsorted.setUniformMatrix4f("u_viewMatrix", _camera->getModelViewMatrix() );
		particleDrawUnsorted.setUniformMatrix4f("u_projectionMatrix", _camera->getProjectionMatrix() );
		particleDrawUnsorted.setUniformMatrix4f("u_modelViewProjectionMatrix", _camera->getModelViewProjectionMatrix() );

		particleDrawUnsorted.setUniform1f("u_particleMaxAge", particleMaxAge );
	
		particleDrawUnsorted.setUniform1f("u_screenWidth", ofGetWidth() );

        if (timeDiff < 1)
        {
            particleDrawUnsorted.setUniform1f("u_particleDiameter", ofMap(timeDiff, 0.5f, 1, particleSize, particleSizeMin, true));
            ofFloatColor c = particleStartCol.lerp(particleEndCol, timeDiff);
            particleDrawUnsorted.setUniform4fv("u_particleStartColor", particleStartCol.v);
            particleDrawUnsorted.setUniform4fv("u_particleEndColor", c.v);
            particleDrawUnsorted.setUniform1f("u_stringTheory", ofMap(timeDiff, 0.5f, 1, 0, stringTheory, true));
        }
        else
        {
            particleDrawUnsorted.setUniform1f("u_particleDiameter", particleSizeMin);
            particleDrawUnsorted.setUniform4fv("u_particleStartColor", particleStartCol.v);
            particleDrawUnsorted.setUniform4fv("u_particleEndColor", particleEndCol.v);
            particleDrawUnsorted.setUniform1f("u_stringTheory", stringTheory);
        }


        particleDrawUnsorted.setUniform1i("u_meshMode", particlePoints.getMode()); // 3: lines 6: points
        particleDrawUnsorted.setUniform1i("u_stringDirection", 0); // 0: East 1: South 2: West 3: North
        
        particlePoints.draw();
        {
            particlePointsEast.setMode(OF_PRIMITIVE_LINES);
            particlePointsSouth.setMode(OF_PRIMITIVE_LINES);
            particlePointsWest.setMode(OF_PRIMITIVE_LINES);
            particlePointsNorth.setMode(OF_PRIMITIVE_LINES);

            particleDrawUnsorted.setUniform1i("u_meshMode", particlePointsEast.getMode()); // 3: lines 6: points
            particleDrawUnsorted.setUniform1i("u_stringDirection", 0); // 0: East 1: South 2: West 3: North

            particlePointsEast.draw();

            particleDrawUnsorted.setUniform1i("u_meshMode", particlePointsSouth.getMode()); // 3: lines 6: points
            particleDrawUnsorted.setUniform1i("u_stringDirection", 1);

            particlePointsSouth.draw();

            particleDrawUnsorted.setUniform1i("u_meshMode", particlePointsWest.getMode()); // 3: lines 6: points
            particleDrawUnsorted.setUniform1i("u_stringDirection", 2);

            particlePointsWest.draw();

            particleDrawUnsorted.setUniform1i("u_meshMode", particlePointsNorth.getMode()); // 3: lines 6: points
            particleDrawUnsorted.setUniform1i("u_stringDirection", 3);

            particlePointsNorth.draw();
        }
        if (triangles)
        {
            particlePointsTri.setMode(OF_PRIMITIVE_TRIANGLES);
            particleDrawUnsorted.setUniform1i("u_meshMode", particlePointsTri.getMode()); // 3: lines 6: points
            particleDrawUnsorted.setUniform1i("u_stringDirection", 0); // 0: East 1: South 2: West 3: North

            particlePointsTri.draw();
        }

	particleDrawUnsorted.end();

	ofDisablePointSprites();
}