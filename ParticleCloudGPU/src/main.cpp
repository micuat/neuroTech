#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main()
{
	//ofSetCurrentRenderer( ofGLProgrammableRenderer::TYPE );
	//ofSetOpenGLVersion( 3, 2 );
	ofSetupOpenGL(1280,800, OF_WINDOW);
	ofRunApp( new ofApp());

}
