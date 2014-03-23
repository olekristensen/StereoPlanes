//
//  deferredScene.h
//  Trae
//
//  Created by Johan Bichel Lindegaard on 22/03/14.
//
//

#pragma once
#include "ofMain.h"
#include "ContentScene.h"

class DeferredScene : public ContentScene {
    
    struct Box {
        ofVec3f pos;
        float size;
        float angle;
        float axis_x;
        float axis_y;
        float axis_z;
        
        Box(ofVec3f pos=ofVec3f(0.0f, 0.0f, 0.0f), float angle=0.0f, float ax=0.0f, float ay=0.0f, float az=0.0f, float size=2.0f) :
        pos(pos),
        size(size),
        angle(angle),
        axis_x(ax),
        axis_y(ay),
        axis_z(az)
        {}
    };
    
    static const int skNumBoxes = 50;
    
public:
    void draw(int _surfaceId);
    
    void geometryPass(int _surfaceId);
    
    void update();
    void setup();
    void makeTrees();
    
    void setGui(ofxUICanvas * gui, float width);
    void guiEvent(ofxUIEventArgs &e);
    void receiveOsc(ofxOscMessage * m, string rest);
    
    float zPos;
    
    bool regrow;
    bool hasRegrown;
    //vector<Branch*> trees;
    
    ////
    ofImage m_texture;
    vector<Box> m_boxes;
    ofVbo  m_boxVbo;
    int    m_numBoxVerts;
    
};