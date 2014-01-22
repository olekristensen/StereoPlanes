//
//  ContentScene.cpp
//  StereoPlanes
//
//  Created by Johan Bichel Lindegaard on 21/01/14.
//
//

#pragma once
#include "ofMain.h"
#include "ofxUI.h"
#include "ofxOsc.h"

class ContentScene {
    
public:
    
    int index;
    string name = "untitled_scene";
    string oscAddress = "/default";
    
    // bool syphonDirect;
    ofParameterGroup parameters;
    bool enabled = true;
    
    
    ContentScene() {
    }
    
    virtual ~ContentScene(){}
    
    virtual void setup(){}
    virtual void update(){}
    virtual void draw(int _surfaceId=0){}
    virtual void debugDraw(int _surfaceId=0) {}
    virtual void exit(){}
    virtual void receiveOsc(ofxOscMessage * m, string rest) {};
    
    virtual void setGui(ofxUICanvas * gui, float width){
        
        string i = "["+ ofToString(index) + "] ";
        
        gui->addWidgetDown(new ofxUILabel(name, OFX_UI_FONT_SMALL));
        gui->addWidgetDown(new ofxUILabel("OSC Address: " + oscAddress, OFX_UI_FONT_SMALL));
        gui->addSpacer(width, 1);
        gui->addToggle(i+"Enabled", &enabled);
        
    }
    
    
    void setupScene() {
        setup();
    }
    
    void updateScene() {
        if(enabled) {
            update();
        }
    }
    
    void drawScene(int _surfaceId=0) {
        if(enabled) {
            
            glPushMatrix();ofPushMatrix();ofPushStyle();
            draw(_surfaceId);
            ofPopStyle();ofPopMatrix();glPopMatrix();
            
            glPushMatrix();ofPushMatrix();ofPushStyle();
            debugDraw(_surfaceId);
            ofPopStyle();ofPopMatrix();glPopMatrix();
            
        }
    }
    
    void parseSceneOsc(ofxOscMessage * m){
        
        vector<string> adrSplit = ofSplitString(m->getAddress(), "/");
        string rest = ofSplitString(m->getAddress(), "/"+adrSplit[1])[1];
        //cout<<adrSplit[1]<<"   "<<rest<<endl;
        
        if(adrSplit[1] == "scene"+ofToString(index) || "/"+adrSplit[1] == oscAddress) {
            
            if(rest == "/enable/x" ) {
                enabled = m->getArgAsInt32(0);
            }
            
            receiveOsc(m, rest);
        }
    }
    
    
};