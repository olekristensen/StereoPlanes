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


// Todo: rename StereoContentScene
class ContentScene {
    
public:
    
    int index;
    string indexStr;
    string name = "untitled_scene";
    string oscAddress = "/default";
    
    bool enabled;
    float time;

    ContentScene() {
    }
    
    virtual ~ContentScene(){}
    
    virtual void exit(){}
    
    virtual void receiveOsc(ofxOscMessage * m, string rest) {};
    
    virtual void setGui(ofxUICanvas * gui, float width){
        
        gui->addWidgetDown(new ofxUILabel(name, OFX_UI_FONT_SMALL));
        gui->addWidgetDown(new ofxUILabel("OSC Address: " + oscAddress, OFX_UI_FONT_SMALL));
        gui->addSpacer(width, 1);
        gui->addToggle(indexStr+"Enabled", &enabled);
    }
    
    virtual void guiEvent(ofxUIEventArgs &e) {};
    
    void setupScene(int _index) {
        index = _index;
        indexStr = "["+ ofToString(_index) + "] ";
        
        setup();
    }
    
    void updateScene() {
        if(enabled) {
            update();
        }
    }
    
    void lightPassScene(int _surfaceId=0){
        if(enabled) {
            lightPass(_surfaceId);
        }
    }
    
    void geometryPassScene(int _surfaceId=0){
        if(enabled) {
            geometryPass(_surfaceId);
        }
    }
    
    void drawScene(int _surfaceId) {
        if(enabled) {
            glPushMatrix();{
                ofPushStyle();{
                    ofPushMatrix();{
                        
                        draw(_surfaceId);
                        
                    }ofPopMatrix();
                }ofPopStyle();
            }glPopMatrix();
        }
    }
    
    void sceneDebugDraw(int _surfaceId) {
        if(enabled) {
            debugDraw(_surfaceId);
        }
    }
    
    void parseSceneOsc(ofxOscMessage * m){
        
        vector<string> adrSplit = ofSplitString(m->getAddress(), "/");
        string rest = ofSplitString(m->getAddress(), "/"+adrSplit[1])[1];
        //cout<<adrSplit[1]<<"   "<<rest<<endl;
        //cout << adrSplit[0] << " " << adrSplit[1] << " " <<adrSplit[2] << endl;
        
        if(adrSplit[1] == "scene"+ofToString(index) || "/"+adrSplit[1] == oscAddress) {
            if(rest == "/enable/x" ) {
                enabled = m->getArgAsInt32(0);
            }
            receiveOsc(m, rest);
        }
    }
    
protected:
    virtual void setup(){}
    virtual void update(){}
    
    virtual void lightPass(int _surfaceId=0){};
    
    virtual void geometryPass(int _surfaceId=0){}
    
    virtual void draw(int _surfaceId=0){}
    
    virtual void debugDraw(int _surfaceId=0){}
    
};