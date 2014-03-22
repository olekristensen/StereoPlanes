#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxOscReceiver.h"
#include "StereoPlane.h"
#include "ContentScene.h"
#include "ofxUI.h"
#include "Trae.h"
#include "ofxUbo.h"
#include "ofxOlaShaderLight.h"

class shaderLight : public ofNode {
    
};


class testApp : public ofBaseApp
{
public:
	
	void setup();
	void update();
	void draw();
	
    void drawFly();
    
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    
    ofMatrix3x3 calcNormalMatrix(ofMatrix4x4 matrix);
    
    void exit();

    ofxSyphonServer sbsOutputServer;
    
    ofxOscReceiver oscReceiver;
//    ofxOscSender oscSender;
    
    ofVec3f camPosWall;
    float eyeSeperation;
    
    float dancerEllipseSize;
    ofVec2f dancerPos;
    
    bool drawFBOs = false;
    bool drawChessboards = false;
    bool drawGrids = false;
    
    float aspect = 1.0;
    
    ofxUIScrollableCanvas *gui;
    void guiEvent(ofxUIEventArgs &e);
    
    ofFbo fbo;
    StereoPlane * wall;
    StereoPlane * activePlane;
    int activePlaneIndex;
    
    void drawFloor();
    vector<StereoPlane *> planes;
    ofxXmlSettings settings;
    
    float dancerHeight;
    
    void drawScenes(int _surfaceId=0);
    
    // Scenes
    Trae * trae;
    vector<ContentScene*> contentScenes;
    
    // Lights
    
    ofxOlaShaderLight flyLight;
    ofxOlaShaderLight moonLight;
    
    ofxOlaShaderLight::Material white;
    
    vector<ofxOlaShaderLight*> randomLights;
    
    float vertexNoise;
    
    // Time
    
    float time = 0;;
    float speed;
    
};