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
    
    mainTimeline->addPage(name);
    
    tlRadius = mainTimeline->addCurves("Radius");
    tlStartFlags = mainTimeline->addFlags("Start");
    tlSpeed = mainTimeline->addCurves("Speed");
    tlNoise = mainTimeline->addCurves("Noise");
    
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
        
        
        center->drawActiveRings(time);
        
        //tlStartFlags->get
        
            //center->drawExpand(ofMap(time, 0, 30, 0, float(center->step)));
        
            //center->draw(ofMap(time, 30, 120, 0, float(center->step)));
        
        /*for(int i=0;i<rings.size();i++) {
            rings[i]->draw();
        }*/
        
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