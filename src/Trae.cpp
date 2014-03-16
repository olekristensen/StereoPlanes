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
    warmlight.setDiffuseColor(ofColor::orange);
    warmlight.setPosition(0, 0.5, 0);
    warmlight.setAmbientColor(ofColor::black);
    ofSetGlobalAmbientColor(ofColor::black);
    coldlight.setPointLight();
    coldlight.setDiffuseColor(ofColor(33,33,33));
    coldlight.setPosition(0, -2, -2);
    
    makeTrees();
}


void Trae::draw(int _surfaceId) {
    
    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        
        ofPushMatrix();

        ofSetSmoothLighting(true);
        
        ofTranslate(ofPoint(0.,0,1.0));

        warmlight.enable();
        //coldlight.enable();
        
        ofColor fire = ofColor::orange;
        
        warmlight.setDiffuseColor(fire.lerp(ofColor(64,24,0), ofNoise(ofGetElapsedTimef())));

        ofRotateY(ofGetElapsedTimef()*10);

        //ofDrawGrid();
        
        ofSetColor(255,255,255);
        
        for (std::vector<Branch*>::iterator it = trees.begin() ; it != trees.end(); ++it) {
            Branch *b = *(it);
            b->draw();
        }

        warmlight.disable();
        coldlight.disable();
        
        ofPopMatrix();
        
    }
    
}

void Trae::update() {
    if (regrow & !hasRegrown) {
        makeTrees();
        hasRegrown = true;
    }
    if (!regrow){
        hasRegrown = false;
    }
}

void Trae::makeTrees(){
    
    for (std::vector<Branch*>::iterator it = trees.begin() ; it != trees.end(); ++it) {
        Branch *b = *(it);
        delete b;
    }
    trees.clear();
    
    for (int i = 0; i < 3; i++) {
        Branch * b = new Branch();
        float bWidth = ofRandom(0.03,0.075);
        float bHeight = ofRandom(0.5,0.75);
        b->set(bWidth,bHeight);
        b->setPosition(ofRandom(-.5,.5),1-(bHeight*.5),ofRandom(-.5,.5));
        b->setResolutionRadius(4);
        b->setResolutionHeight(1);
        b->make(8);
        trees.push_back(b);
    }
}

void Trae::setGui(ofxUICanvas * gui, float width){
    ContentScene::setGui(gui, width);
    gui->addButton("Regrow", &regrow);
}

void Trae::receiveOsc(ofxOscMessage * m, string rest) {
}

void Trae::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.getName();
	int kind = e.getKind();
	//cout << "got event from: " << name << endl;
    
}

