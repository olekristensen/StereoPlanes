//
//  Trae.cpp
//  Trae
//
//  Created by Ole Kristensen on 15/03/14.
//
//

#include "Trae.h"

void Trae::setup() {
    
    name = "Trae Scene";
    oscAddress = "/trae";
    warmlight.setPointLight();
    warmlight.setDiffuseColor(ofColor::wheat);
    warmlight.setPosition(-1, 0.5, -1);
    coldlight.setPointLight();
    coldlight.setDiffuseColor(ofColor::white);
    coldlight.setPosition(1, -0.5, -1);
    
    trae.set(0.05,0.5);
    trae.setPosition(0,0.75,0);
    trae.setResolutionRadius(20);
    trae.make(5);
}


void Trae::draw(int _surfaceId) {
    
    ofBackground (0);
    ofSetSmoothLighting(true);
    
    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        
        warmlight.enable();
        coldlight.enable();
        
/*        // ofSetColor(0,255,0,250);
        // FIXME: there is an alpha problem here, the box can't be seen through the rect if drawn
        // ofRect(-1, -1, 2, 2);
        
        ofSetColor(255,255,255);
        //ofDrawGrid(1);
        
        ofPushMatrix();
        glTranslatef(0.,0.1,1.);
        ofRotateY(ofGetElapsedTimef()*10);
        glTranslatef(0.,0,.5);
        ofDrawBox(0.5,1.8,0.5);
        ofPopMatrix();
        ofPushMatrix();
        glTranslatef(0.,0,1.);
        ofRotateX(-90);
        glTranslatef(0,0,1);
        ofEllipse(0,0,.5,.5);
        
        ofPopMatrix();
*/
        ofSetColor(255,255,255);
        ofPushMatrix();
        glTranslatef(0.,0,.5);

        ofRotateY(ofGetElapsedTimef()*10);
        trae.draw();

        ofPopMatrix();
        warmlight.disable();
        coldlight.disable();
        
    }
    
}

void Trae::update() {
}


void Trae::setGui(ofxUICanvas * gui, float width){
    ContentScene::setGui(gui, width);
    
    
}

void Trae::receiveOsc(ofxOscMessage * m, string rest) {
}

void Trae::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.getName();
	int kind = e.getKind();
	//cout << "got event from: " << name << endl;
    
}

