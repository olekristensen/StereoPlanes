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
    
    tlRadius = mainTimeline->addCurves("Radius");
    tlStartFlags = mainTimeline->addFlags("Start");
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
    center->flags = tlStartFlags;
    center->setup();
    center->make(100);
    //rings = center->getChildren();
    
    shader.setGeometryInputType(GL_POINTS);
	shader.setGeometryOutputType(GL_LINES);
	shader.setGeometryOutputCount(2);
	shader.load("shaders/vert.glsl", "shaders/frag.glsl", "shaders/geom.glsl");

    
}

void TrunkRings::draw(int _surfaceId) {
    
    bool wasLightEnabled = ofxOlaShaderLight::isEnabled();
    
    if(wasLightEnabled) ofxOlaShaderLight::end();
    
    ofEnableSmoothing();
    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        
        ofPushMatrix();
        
        shader.begin();
        
        shader.setUniform1f("thickness", 50);
        shader.setUniform3f("lightDir", sin(ofGetElapsedTimef()/10), cos(ofGetElapsedTimef()/10), 0);
        
        shader.setAttribute4f(shader.getAttributeLocation("color"), 1, 1, 1, 1);
        /*GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
        glEnableVertexAttribArray(colAttrib);
        glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE,
                              5 * sizeof(float), (void*) (2 * sizeof(float)));*/
        
        ofTranslate(1, 1);
        ofRotateX(tlKnockover->getValue());
        ofTranslate(-1, -1);
        
        ofRotateZ(tlRotateZ->getValue());
        ofRotateY(tlRotateY->getValue());
        
        center->drawActiveRings(time);
        
        shader.end();
        //tlStartFlags->get
        
            //center->drawExpand(ofMap(time, 0, 30, 0, float(center->step)));
        
            //center->draw(ofMap(time, 30, 120, 0, float(center->step)));
        
        /*for(int i=0;i<rings.size();i++) {
            rings[i]->draw();
        }*/
        
        ofPopMatrix();        
    }
    if(wasLightEnabled) ofxOlaShaderLight::begin();
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