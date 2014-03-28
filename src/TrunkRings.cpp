//
//  TrunkRings.cpp
//  Trae
//
//  Created by Johan Bichel Lindegaard on 27/03/14.
//
//

#include "TrunkRings.h"

void TrunkRings::setup() {
    
    name = "Trunk Rings";
    oscAddress = "/trunkrings";
    
    
    //line.addVertex(ofVec3f(0,0,0));
    //ringPath.push_back
    
    //tweenquad.setParameters(8,easingquad,ofxTween::easeOut,0,ofGetWidth()-100,duration,delay);
    
    mainTimeline->addPage(name);
    ringSwitches = mainTimeline->addSwitches("Ring");
    
    ringRadius = mainTimeline->addCurves("Radius");
    //yPos = mainTimeline->addCurves("Y Position");
    
    
    //xPos->setValueRangeMin(-1);
    //xPos->setValueRangeMax(1);
    
    //yPos->setValueRangeMin(-1);
    //yPos->setValueRangeMax(1);
    // todo: reestablish pos history for a given time in
    
    //yPos->getKeyframes(
    
    center = new Ring();
    center->setup();
    center->make(100);
    //rings = center->getChildren();
    
    
    
}

void TrunkRings::draw(int _surfaceId) {
    
    ofEnableSmoothing();
    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        
        ofPushMatrix();
        
        if(time<30) {
            center->drawExpand(ofMap(time, 0, 30, 0, float(center->step)));
        } else {
        
            center->draw(ofMap(time, 30, 120, 0, float(center->step)));
        }
        
        
        /*for(int i=0;i<rings.size();i++) {
            rings[i]->draw();
        }*/
        
        ofPopMatrix();
        
    }
    
}

void TrunkRings::update() {
    if(time > 140) {
    }
    
}


void TrunkRings::setGui(ofxUICanvas * gui, float width){
    ContentScene::setGui(gui, width);
    
}

void TrunkRings::receiveOsc(ofxOscMessage * m, string rest) {
}

void TrunkRings::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.getName();
	int kind = e.getKind();
	//cout << "got event from: " << name << endl;
    
}