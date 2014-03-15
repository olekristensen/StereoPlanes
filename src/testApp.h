#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxOscReceiver.h"
#include "StereoPlane.h"
#include "VoronoiWall.h"
#include "BoxFloor.h"
#include "WireMesh.h"
#include "Lines.h"
#include "Voro3D.h"
#include "ContentScene.h"
#include "TestScene.h"
#include "ofxUI.h"
#include "AttractorControl.h"
#include "Trae.h"


class testApp : public ofBaseApp
{
public:
	
	void setup();
	void update();
	void draw();
	
	void drawScene1();
    void drawScene2();
    
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    
    void exit();

    ofxSyphonServer sbsOutputServer;
    
    bool showGrid;

    ofxOscReceiver oscReceiver;
//    ofxOscSender oscSender;
    
    ofVec3f camPosWall;
    float eyeSeperation;
    
    float dancerEllipseSize;
    ofVec2f dancerPos;
    
    bool hideGUI = false;
    bool hideMonitor = true;
    
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
    TestScene * testScene;
    VoronoiWall * voronoiWall;
    BoxFloor * boxFloor;
    AttractorControl * attractorControl;
    WireMesh * wireMesh;
    Voro3D * voro3d;
    Trae * trae;
    
    vector<ContentScene*> contentScenes;
    
};