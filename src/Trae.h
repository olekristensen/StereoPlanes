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

class Branch : public ofCylinderPrimitive {
    vector<Branch> branches;
    
    void draw(){
        this->ofCylinderPrimitive::draw();
        for (std::vector<Branch>::iterator it = branches.begin() ; it != branches.end(); ++it) {
            Branch b = *(it);
            b.draw();
        }
    }
    
    void addBranch(){
        Branch b;
        b.set( this->getRadius()*0.95, this->getHeight()*0.85 );
        
    }
    
};

class Trae : public ContentScene {
    
public:
    void draw(int _surfaceId);
    void update();
    void setup();
    
    void setGui(ofxUICanvas * gui, float width);
    void guiEvent(ofxUIEventArgs &e);
    void receiveOsc(ofxOscMessage * m, string rest);
    
    ofLight warmlight;
    ofLight coldlight;
    
};
