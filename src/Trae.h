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

class TreeNode : public ofNode {
public:
    float size = 1.0;
    ofxOlaShaderLight::Material material;
    ofxProcTreeBranch::Properties properties;;
};

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
    ofxTLCurves * noiseFront;
    ofxTLCurves * noiseBack;
    ofxTLCurves * noiseSpeed;
    ofxTLColorTrack * treeColor;
    ofxTLColorTrack * groundColor;
    ofxTLCameraTrack* cameraTrack;
	ofEasyCam cam;
    
    bool addCameraKeyFrame;
    bool hasRegrown;
    
    ofxOlaShaderLight::Material treeMaterial;
    ofxOlaShaderLight::Material groundMaterial;
    
    vector<ofxProcTree*> trees;
    
    vector<TreeNode*> treeNodes;
    
    ofxOlaShaderLight::NoisePoints noisePoints;
    
    float drawScale = 1.0;
    
    float noiseTimeElapsed;
};
