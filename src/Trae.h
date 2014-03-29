//
//  Trae.h
//  Trae
//
//  Created by Ole Kristensen on 15/03/14.
//
//

#pragma once
#include "ContentScene.h"
#include "ofMain.h"
#include "ofxProcTree.h"
#include "ofxOlaShaderLight.h"
#include "ofxTLCameraTrack.h"

class Trae : public ContentScene {
    
public:
    void draw(int _surfaceId);
    void update();
    void setup();
    void makeTrees();
    
    void setGui(ofxUICanvas * gui, float width);
    void guiEvent(ofxUIEventArgs &e);
    void receiveOsc(ofxOscMessage * m, string rest);
    
    float zPos;
    
    ofxTLSwitches * regrow;
    ofxTLCurves * progress;
    ofxTLColorTrack * treeColor;
    ofxTLColorTrack * groundColor;
    ofxTLCameraTrack* cameraTrack;
	ofEasyCam cam;
    
    bool addCameraKeyFrame;
    bool hasRegrown;
    
    ofxOlaShaderLight::Material treeMaterial;
    ofxOlaShaderLight::Material groundMaterial;
    
    vector<ofxProcTree*> trees;
    ofVboMesh particles;

    float drawScale = 1.0;
};
