//
//  Lights.cpp
//  Trae
//
//  Created by Ole Kristensen on 28/03/14.
//
//

#include "Lights.h"


void Lights::setup() {
    
    name = "Lights";
    oscAddress = "/lights";
    
    mainTimeline->addPage(name);

    flyLightColor = mainTimeline->addColors("Fly Light Color");
    flyLightAttenuation = mainTimeline->addCurves("Fly Light Attenuation", ofRange(0.001,5.0));
    flyLightPosX = mainTimeline->addCurves("Fly Light Pos X", ofRange(-3.0, 3.0));
    flyLightPosY = mainTimeline->addCurves("Fly Light Pos Y", ofRange(-3.0, 3.0));
    flyLightPosZ = mainTimeline->addCurves("Fly Light Pos Z", ofRange(-3.0, 3.0));
    flyLightPosNoise = mainTimeline->addCurves("Fly Light Pos Noise", ofRange(0.0, 1.0));
    flyLightPosNoiseSpeed = mainTimeline->addCurves("Fly Light Pos Noise Speed", ofRange(-1.0, 1.0));

    skyLightColor = mainTimeline->addColors("Sky Light Color");
    skyLightAttenuation = mainTimeline->addCurves("Sky Light Attenuation", ofRange(0.001,5.0));
    
    material.diffuseColor = ofVec4f(0.9, 0.9, 0.9, 1.0);
    material.specularColor = ofVec4f(0.0, 0.0, 0.0, 0.0);
    material.specularShininess = 0.5;
    
    flyLight.setNormalisedBrightness(1.0);
    flyLight.setAttenuation(1.0/2.);
    flyLight.setTemperature(4200);
    
    skyLight.setNormalisedBrightness(1.0);
    skyLight.setAttenuation(1./10.);
    skyLight.setTemperature(10000);

}

void Lights::begin(){
    ofxOlaShaderLight::begin();
    ofxOlaShaderLight::setMaterial(material);
    
    flyLight.setDiffuseColor(flyLightColor->getColor());
    flyLight.setAttenuation(1./flyLightAttenuation->getValue());
    
    skyLight.setDiffuseColor(skyLightColor->getColor());
    skyLight.setAttenuation(1./skyLightAttenuation->getValue());
    
}

void Lights::end(){
    ofxOlaShaderLight::end();
}

void Lights::setMaterial(ofxOlaShaderLight::Material m){
    material = m;
}

void Lights::draw(int _surfaceId) {
    
    bool lightWasEnabled = ofxOlaShaderLight::isEnabled();

    if(lightWasEnabled){
        ofxOlaShaderLight::end();
    }
    ofDrawSphere(flyLight.getGlobalPosition(), 0.01);
    if(lightWasEnabled){
        ofxOlaShaderLight::begin();
    }
}

void Lights::update() {
    
    float flyTime = mainTimeline->getCurrentTime() * flyLightPosNoiseSpeed->getValue();
    
    float zPos =ofSignedNoise(0,0,flyTime);
    float reduction = fmaxf(0,ofMap(zPos, 1, -1, 0.0, 1));
    reduction = pow(reduction, 3);
    
    ofxUISlider * wallPosX = (ofxUISlider*) gui->getWidget("Wall X");
    ofxUISlider * wallPosY = (ofxUISlider*) gui->getWidget("Wall Y");
    ofxUISlider * wallPosZ = (ofxUISlider*) gui->getWidget("Wall Z");
    
    ofVec3f camPosWall = ofVec3f(
                                 wallPosX->getValue(),
                                 wallPosY->getValue(),
                                 wallPosZ->getValue()
                                 );
    
    ofVec3f posNoise(
                     ofMap(reduction, 0,1,ofSignedNoise(flyTime), camPosWall.x),
                     ofMap(reduction, 0,1,ofSignedNoise(0,flyTime), camPosWall.y),
                     2.2*zPos
                     );
    
    ofVec3f pos = (ofVec3f(
                          flyLightPosX->getValue(),
                          flyLightPosY->getValue(),
                          flyLightPosZ->getValue()
                          )*(1.0-flyLightPosNoise->getValue())) + (posNoise*(flyLightPosNoise->getValue()));
    
    flyLight.setGlobalPosition(pos);
    skyLight.setGlobalPosition(0,-1,-1);
    
    ofxOlaShaderLight::update();
}

void Lights::setGui(ofxUICanvas * gui, float width){
    this->gui = gui;
    ContentScene::setGui(gui, width);
}

void Lights::receiveOsc(ofxOscMessage * m, string rest) {

}

void Lights::guiEvent(ofxUIEventArgs &e)
{
    string name = e.getName();
	int kind = e.getKind();
	//cout << "got event from: " << name << endl;
}