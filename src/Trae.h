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
        b->setParent(*this);
        b->set(this->getRadius()*.65,this->getHeight()*ofRandom(0.5,0.85));
        b->setResolutionRadius(this->getResolutionRadius());
        b->setResolutionHeight(this->getResolutionHeight());
        b->boom(-this->getHeight()*.5);
        b->rotate(ofRandom(-75,75), ofVec3f(1,0,0));
        b->rotate(ofRandom(-180,180), ofVec3f(0,1,0));
        b->boom(-b->getHeight()*.5);
        branches.push_back(b);
    }
public:

    void make(int steps){
        if(steps > 1){
            for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
                Branch *b = *(it);
                delete b;
            }
            branches.clear();
            int numberBranches = ofRandom(2,4);
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
        ofColor c = ofGetStyle().color;
        this->ofCylinderPrimitive::draw();
//        ofSetColor(c.r, c.g, c.b, c.a*0.85);
        ofPushStyle();
        for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
            Branch *b = *(it);
            b->draw();
        }
        ofPopStyle();
    }

    
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
    
    ofLight warmlight;
    ofLight coldlight;
    
    bool regrow;
    bool hasRegrown;
    vector<Branch*> trees;
    
};
