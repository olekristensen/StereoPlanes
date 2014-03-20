#include "ofMain.h"
#include "testApp.h"
//#include <ofAppGlutWindow.h>
// import the fancy new renderer
#include "ofGlProgrammableRenderer.h"

//========================================================================
int main( ){    
    
//    ofAppGlutWindow window;
//    window.setGlutDisplayString("rgb alpha double samples>=8 depth");
    ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
    ofSetupOpenGL(300+(1280/2), 850, OF_WINDOW);
    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new testApp());

}
