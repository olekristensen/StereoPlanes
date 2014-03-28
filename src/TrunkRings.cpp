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
    center->make(20);
    
    
}

void TrunkRings::draw(int _surfaceId) {
    
    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        
        ofPushMatrix();
        
        //ofVec3f pos;
        
       // float noiseDisplace = ofNoise(time)/20;
        
        /*if(time<4) {
            // grow straight up for first 4 seconds
            pos = ofVec3f(noiseDisplace, ofMap(time,0,4,1,0.0) - noiseDisplace,0);
        
        } else {*/
        
        
        
            // start
            
            //pos = ofVec3f(noiseDisplace + sin(time)*ringRadius->getValue(), cos(time)*ringRadius->getValue() - noiseDisplace, 0);
            
        
        //}
        
        /*for(int i=1;i<points.size()&&i<time;i++) {
            
            ofLine(points[i], points[i-1]);
            
        }*/
        
        
        //ofCircle(sin(time), sin(time), 0.2);
        
        //ofCircle(pos, 0.02);
        
        /*for(int i=0; i<rings.size(); i++){
            rings[i]->draw();
        }*/
        
        /*for (int i=1; i<TAIL_LENGTH; i++) {
            ringHistory[i-1] = ringHistory[i];
        }
        ringHistory[TAIL_LENGTH-1] = pos;
        
        
        ofMesh line; // declaring a new ofMesh object with which we're drawing the motion path created by summing the vertical and horizontal oscillators
        line.setMode(OF_PRIMITIVE_LINE_STRIP);
        for (int i=0; i<TAIL_LENGTH; i++) {
            line.addColor(ofFloatColor(0.8f,0.8f,0.8f, 0.5f + 0.5f * i/float(TAIL_LENGTH) ));
            line.addVertex(ringHistory[i]);
        }*/
        
        //line.draw();
        
        //for(int i=0; i<)
        
        //int long millis = mainTimeline->getCurrentTimeMillis();
        //ofCircle(xPos->getValueAtTimeInMillis(millis), yPos->getValueAtTimeInMillis(millis), 0.2);
        
        
        center->draw();
        
        ofPopMatrix();
        
    }
    
}

void TrunkRings::update() {
    
    
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