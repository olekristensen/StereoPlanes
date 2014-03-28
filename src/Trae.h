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
    
    bool regrow;
    bool hasRegrown;
    vector<ofxProcTree::Tree*> trees;
    
    float drawScale = 1.0;
    ofVec3f drawTranslation;
    ofVec3f drawRotation;
};
