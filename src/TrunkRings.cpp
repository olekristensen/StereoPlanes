//
//  TrunkRings.cpp
//  Trae
//
//  Created by Johan Bichel Lindegaard on 27/03/14.
//
//

#include "TrunkRings.h"
#include "ofxOlaShaderLight.h"

void TrunkRings::setup() {
    
    name = "Trunk Rings";
    oscAddress = "/trunkrings";
    
    mainTimeline->addPage(name);
    
    tlStartRing = mainTimeline->addCurves("Start");
    
    tlRadius = mainTimeline->addCurves("Radius");
    tlSpeed = mainTimeline->addCurves("Speed");
    tlNoise = mainTimeline->addCurves("Noise");
    
    tlKnockover = mainTimeline->addCurves("Knockover");
    tlKnockover->setValueRangeMax(-90);
    tlKnockover->setValueRangeMin(0);
    
    tlRotateZ = mainTimeline->addCurves("Rotate Z");
    tlRotateZ->setValueRangeMax(360);
    tlRotateZ->setValueRangeMin(0);
    
    tlRotateY = mainTimeline->addCurves("Rotate Y");
    tlRotateY->setValueRangeMax(360);
    tlRotateY->setValueRangeMin(0);
    
    center = new Ring();
    center->p_tlStart = tlStartRing;
    center->setup();
    
    numRings = tlStartRing->getKeyframes().size();
    center->make(numRings);
    
    //rings = center->getChildren();
    
    //shader.setGeometryInputType(GL_POINTS);
	//shader.setGeometryOutputType(GL_TRIANGLES);
	//shader.setGeometryOutputCount(5);
	//shader.load("shaders/vert.glsl", "shaders/frag.glsl", "shaders/geom.glsl");
    
    material.diffuseColor = ofVec4f(1.0,1.0,1.0,1.0);
    
}

void TrunkRings::draw(int _surfaceId) {
    
    ofxOlaShaderLight::setMaterial(material);
    
    ofEnableSmoothing();
    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        
        
        ofPushMatrix();
        ofTranslate(1, 1);
        ofRotateX(tlKnockover->getValue());
        ofTranslate(-1, -1);
        
        ofRotateZ(tlRotateZ->getValue());
        ofRotateY(tlRotateY->getValue());
        
        center->drawActiveRings(time);
        
        ofPopMatrix();
        
        //tlStartFlags->get
        
            //center->drawExpand(ofMap(time, 0, 30, 0, float(center->step)));
        
            //center->draw(ofMap(time, 30, 120, 0, float(center->step)));
        
        /*for(int i=0;i<rings.size();i++) {
            rings[i]->draw();
        }*/
        
        
    }

}

void TrunkRings::update() {
    
    if(numRings != tlStartRing->getKeyframes().size()) {
        numRings =  tlStartRing->getKeyframes().size();
        
        /*center = new Ring();
        center->p_tlStart = tlStartRing;
        center->setup();*/
        
        center->make(numRings);
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