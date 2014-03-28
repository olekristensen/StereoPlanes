//
//  TrunkRings.h
//  Trae
//
//  Created by Johan Bichel Lindegaard on 27/03/14.
//
//

#pragma once
#include "ofMain.h"
#include "ContentScene.h"

#define WAVEFORM_HISTORY 800
#define TAIL_LENGTH 4000

// Todo: Scars, Knaster, Fill
// Draw modes: Expand and Line grow
// Periodic small and large distance between lines

class Ring {
    
    void addRing(){
        child = new Ring();
        child->setup(this);
    }

public:
    float radius = 0.01;
    int   resolution = 200;
    float seed;
    int   step = 0;
    float variance = 4;
    float scale = 60;
    
    Ring * parent = NULL;
    Ring * child = NULL;
    
    vector<ofVec3f> points;
    ofMesh mesh;
    
    vector<Ring *> childRings;
    
    void setup() {
        setup(NULL);
        seed = ofRandom(0,40);
    };
    
    void setup(Ring * _parent) {
        parent = _parent;
        
        if(parent){
            seed = parent->seed;
            radius = parent->radius += ofRandom(0.01, 0.06);
        }
        
        
        for(int i=0; i<resolution; i++) {
            float nangle = ofMap(i,0,resolution,0,TWO_PI);
            ofVec3f pos = ofVec3f(sin(nangle)*radius, cos(nangle)*radius, 0);
            points.push_back(pos);
        }
        
        for(int i=0; i<points.size(); i++) {
            
            float nangle = ofMap(i,0,points.size(),0,TWO_PI);
            
            //float noiseDisplace = ofSignedNoise(nangle * variance)/scale;
            
            //ofVec3f noiseDisplace = ofVec3f(ofSignedNoise(radius * (nangle+PI) * variance) / scale,
            //                                ofSignedNoise(radius * nangle * variance) / scale,0);
            
            float noiseDisplace = ofNoise(points[i].x+seed, points[i].y+seed, points[i].z+seed);
            
            //float noiseDisplace = ofNoise( nangle / ofMap(radius, 0, 1, 400, 0.2))/ 40;
            //ofVec3f pos = ofVec3f(sin(nangle)*radius, cos(nangle)*radius, 0);
            points[i] += noiseDisplace*0.1;
            
            /*if(parent){
                pos = parent->points[i+1].interpolate(pos, 80);
            }*/
            
            //points.push_back(pos);
            
        }

    }
    
    void make(int steps){
        if(steps > 1){
            delete(child);
            step = steps;
            
            addRing();
            child->make(steps-1);
            
        }
        
    }
    
    void draw(float percent=100) {
        
        mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
        for (int i=0; i<points.size(); i++) {
            
            //mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f, 0.5f + 0.5f * i/float(points.size()) ));
            mesh.addVertex(points[i]);
            
            /*if(parent){
                ofLine(points[i], parent->points[i]);
            }*/
            
        }
        ofSetLineWidth(10);
        mesh.draw();
        
        if(child) {
            child->draw();
        }
        
    }
    
};


class TrunkRings : public ContentScene {
    
public:
    void draw(int _surfaceId);
    void update();
    void setup();
    
    void setGui(ofxUICanvas * gui, float width);
    void guiEvent(ofxUIEventArgs &e);
    void receiveOsc(ofxOscMessage * m, string rest);
    
    vector<ofVec3f> points;
    
    int seed;
    
    //ofxEasingQuad 	easingquad;
    ofxTLSwitches * ringSwitches;
    
    ofxTLCurves * ringRadius;
    //ofxTLCurves * yPos;
    
    ofVec3f ringHistory [TAIL_LENGTH];
    
    vector<Ring *> rings;
    
    Ring * center;
    
    
};
