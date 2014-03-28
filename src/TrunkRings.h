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
// Have all rings keep growing 

class Ring {
    
    void addRing(){
        child = new Ring();
        child->setup(this);
    }

public:
    float radius = 0.01;
    int   resolution = 140;
    float seed;
    int   step = 0;
    float variance = 4;
    float scale = 60;
    
    Ring * parent = NULL;
    Ring * child = NULL;
    
    vector<ofVec3f> points;
    
    vector<Ring *> allChildren;
    
    void setup() {
        seed = ofRandom(0,10.0);
        setup(NULL);
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
            float noiseDisplace = ofNoise(points[i].x+seed, points[i].y+seed, points[i].z+seed);

            points[i] += noiseDisplace*0.15;
            
        }

    }
    
    void make(int steps){
        if(steps > 1){
            delete(child);
            step = steps;
            
            addRing();
            child->make(steps-1);
            
        }
        
        //allChildren = getChildren();
        
    }
    
    /*void drawPercentage(float percent) {
        
    }*/

    
    void draw(float steps) {
        
        if(steps > 1){
            ofMesh mesh;
            mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
            for (int i=0; i<points.size(); i++) {
            
                mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f, 0.8f));
                mesh.addVertex(points[i]);
                

            }
            ofSetLineWidth(40);
            mesh.draw();
        
            if(child) {
                child->draw(steps-1);
            }
            
        } else {
            if(steps>0) {
                ofMesh mesh;
                int left = float(resolution) * float(steps);
                mesh.setMode(OF_PRIMITIVE_LINE_STRIP);

                for (int i=0; i<left; i++) {
                    
                    mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f, ofMap(i, 0, left, 1.0f, 0.0f)));
                    mesh.addVertex(points[i]);
                    
                }
                
                ofSetLineWidth(40);
                mesh.draw();
                
                //ofNoFill();
                //ofCircle(points[left],0.01);
            }
            
        }
    }
    
    void drawExpand(float steps) {
        
        if(steps > 1){
            ofMesh mesh;
            mesh.setMode(OF_PRIMITIVE_LINE_LOOP);
            for (int i=0; i<points.size(); i++) {
                
                mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f, 1.0f));
                mesh.addVertex(points[i]);
                
            }
            ofSetLineWidth(40);
            mesh.draw();
            
            if(child) {
                child->drawExpand(steps-1);
            }
            
        } else {
            if(steps>0) {
                ofMesh mesh;
                float left = float(resolution) * float(steps);
                
                float percent = left/resolution*1.0;
                //cout<<percent<<endl;
                mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
                
                for (int i=0; i<points.size(); i++) {
                    
                    mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f, ofMap(percent, 1, 0, 1.0f, 0.0f)));
                    if(parent) {
                        mesh.addVertex(points[i].interpolated(parent->points[i], 1-percent));
                    } else {
                        mesh.addVertex(points[i].interpolated(ofVec3f(0,0,0), 1-percent));
                    }
                    
                }
                
                ofSetLineWidth(40);
                mesh.draw();
                
                //ofNoFill();
                //ofCircle(points[left],0.01);
            }
            
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
