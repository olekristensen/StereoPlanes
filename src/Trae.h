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
    vector<Branch*> branches;
    
    void addBranch(){
        Branch * b = new Branch();
        b->set( this->getRadius()*0.85, this->getHeight()*ofRandom(0.5,1.3) );
        b->setResolutionRadius(this->getResolutionRadius());
        b->move(0,-this->getHeight(),0);
        b->setOrientation(ofVec3f(ofRandom(-45,45),ofRandom(-45,45),ofRandom(-45,45)));
        b->setParent(*this);
        branches.push_back(b);
    }
public:

    void make(int steps){
        if(steps > 1){
            int numberBranches = ofRandom(2,5);
            for(int i = 0; i < numberBranches; i++){
                addBranch();
            }
            for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
                Branch *b = *(it);
                b->make(steps-1);
            }
        }
    }
    
    void draw(){
        this->ofCylinderPrimitive::draw();
        for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
            Branch *b = *(it);
            b->draw();
        }
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
    
    Branch trae;
    
};
