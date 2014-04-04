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
    
    //tlRadius = mainTimeline->addCurves("Radius");
    
    
    tlY = mainTimeline->addCurves("Y");
    tlY->setValueRangeMax(-1);
    tlY->setValueRangeMin(1);
    
    tlZ = mainTimeline->addCurves("Z");
    tlZ->setValueRangeMax(-2);
    tlZ->setValueRangeMin(9);
    
    tlX = mainTimeline->addCurves("X");
    tlX->setValueRangeMax(-1);
    tlX->setValueRangeMin(1);
    
    //tlNoise = mainTimeline->addCurves("Noise");
    
    tlKnockover = mainTimeline->addCurves("Knockover");
    tlKnockover->setValueRangeMax(-360);
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
    
    material.diffuseColor = ofVec4f(1.0,1.0,1.0,1.0);
    
    
}

void TrunkRings::draw(int _surfaceId) {
    
    ofxOlaShaderLight::setMaterial(material);
    
    ofEnableSmoothing();
    if(_surfaceId == 0) {
        
        if(numRings > 1) {
            ofPushMatrix();
            
            //fxaa.setUniformTexture("bgl_RenderedTexture", fbo.getTextureReference(0), 0);
            //fxaa.setUniform1f("bgl_RenderedTextureWidth", fbo.getWidth());
            //fxaa.setUniform1f("bgl_RenderedTextureHeight", fbo.getHeight());
            
            //glEnable(GL_POLYGON_SMOOTH);
            //glEnable(GL_LINE_SMOOTH);
            //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
            //glColor3f(1,1,1);
            ofTranslate(tlX->getValue(), tlY->getValue(), tlZ->getValue());
            
            ofRotateX(tlKnockover->getValue());
            ofRotateZ(tlRotateZ->getValue());
            ofRotateY(tlRotateY->getValue());
            
            center->drawActiveRings(time);
            
            glDisable(GL_POLYGON_SMOOTH);
            
            
            ofPopMatrix();
        }
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