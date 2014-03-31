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
    ofxTLFlags * flags;
    
    float radius = 0.01;
    int   resolution = 440;
    float seed;
    int   step = 0;
    float variance = 4;
    float scale = 60;
    float startangle;
    
    float growDuration = 5; // How many seconds to draw one ring
    float start = 2;
    
    Ring * parent = NULL;
    Ring * child = NULL;
    
    vector<ofVec3f> points;
    ofVbo vbo;
    ofxTLFlag * flag;
    
//    vector<Ring *> allChildren;
    
    void setup() {
        seed = ofRandom(0,10.0);
        setup(NULL);
    };
    
    void setup(Ring * _parent) {
        parent = _parent;
        
        startangle = ofRandom(0, TWO_PI);
        
        if(parent){
            flags = parent->flags;
            seed = parent->seed;
            radius = parent->radius += ofRandom(0.01, 0.06);
            start = ofRandom(parent->start-1,parent->start+3);
            growDuration = ofRandom(10,20);
        }
        
        for(int i=0; i<resolution-1; i++) {
            float nangle = ofMap(i,0,resolution,0,TWO_PI) + startangle;
            ofVec3f pos = ofVec3f(sin(nangle)*radius, cos(nangle)*radius, 0);
            points.push_back(pos);
        }
        
        for(int i=0; i<points.size(); i++) {
            float nangle = ofMap(i,0,points.size(),0,TWO_PI);
            float noiseDisplace = ofNoise(points[i].x+seed, points[i].y+seed, points[i].z+seed);

            points[i] += noiseDisplace*0.25;
        }
        
        points.push_back(points[0]);
        
        
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
    
    // Draw child n p percent complete
    void drawStep(int n, float p) {
        if(step == n) {
            
        } else if(child) {
            child->drawStep(n, p);
        }
    }
    
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
                mesh.draw();
                
                ofNoFill();
                ofCircle(points[left],0.01);
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
                
                
                mesh.draw();
                
                //ofNoFill();
                //ofCircle(points[left],0.01);
            }
            
        }
    }
    
    
    void drawGrowAll(float percent) {
    }
    
    void drawAsMesh(float p) {
        p = ofClamp(p, 0, 1);
        
        ofMesh mesh;
        int left = resolution * p;
        //cout<<p<<endl;
        mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
        for (int i=0; i<left; i++) {
            mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f,1.0f));
            mesh.addVertex(points[i]);
        }
        
        mesh.draw();
        //vbo.setMesh(mesh, GL_STREAM_DRAW);
        
        //ofNoFill();
        //ofCircle(points[left],0.01);
        
    }
    
    void drawActiveRings(float seconds) {
        
        
        //cout<<seconds;
        
        if(seconds > start) {
            
            //cout<<"  draw";
            
            float elapsed = (seconds-start);
            float p = elapsed/growDuration;
            //cout<<"  elapsed:  " <<elapsed;
            //cout<<"  p:  " <<p;
            
            //if(p<1) {
                drawAsMesh(p);
            //} else {
                
            //}
            
            //vbo.draw(OF_PRIMITIVE_LINE_STRIP, 0, vbo.getNumVertices());
            
        }
        
        //cout<<endl;
        
        if(child) {
            child->drawActiveRings(seconds);
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
    
    //vector<ofVec3f> points;
    ofShader shader;
    
    int seed;
    
    ofxTLFlags * tlStartFlags;
    ofxTLCurves * tlRadius;
    ofxTLCurves * tlSpeed;
    ofxTLCurves * tlNoise;
    
    ofxTLCurves * tlKnockover;
    ofxTLCurves * tlRotateZ;
    ofxTLCurves * tlRotateY;
    
    //ofVec3f ringHistory [TAIL_LENGTH];
    
    vector<Ring *> rings;
    
    Ring * center;
    
    
};
