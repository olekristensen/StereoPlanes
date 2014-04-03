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
#include "ofxMeshUtils.h"
#include "ofxOlaShaderLight.h"

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
    int   resolution = 440;
    float seed;
    int   step = -1; // first one is just the controller we dont draw it
    float variance = 4;
    float scale = 60;
    float startangle;
    
    float radiusVariance = 0.005;
    
    float growDuration = 5; // How many seconds to draw one ring
    float start = 2;
    
    Ring * parent = NULL;
    Ring * child = NULL;
    
    vector<ofVec3f> points;
    ofVbo vbo;

    ofxTLCurves * p_tlStart;
    
//    vector<Ring *> allChildren;
    
    void setup() {
        seed = ofRandom(0,10.0);
        setup(NULL);
    };
    
    void setup(Ring * _parent) {
        parent = _parent;
        startangle = ofRandom(0, TWO_PI);
        
        if(parent){
            
            step = parent->step + 1;
            p_tlStart = parent->p_tlStart;
            seed = parent->seed;
            radius = parent->radius + ofRandom(0.01, 0.06);
            //start = ofRandom(parent->start-1,parent->start+3);
            
            //growDuration = p_tlStart->getKeyframes()[step]->value;
            growDuration = ofRandom(10,20);
            
            radiusVariance = radius - parent->radius;
        }
        
        for(int i=0; i<resolution-2; i++) { // add two extra points to draw all the way around
            float nangle = ofMap(i,0,resolution,0,TWO_PI) + startangle;
            ofVec3f pos = ofVec3f(sin(nangle)*radius, cos(nangle)*radius, 0);
            points.push_back(pos);
        }
        
        for(int i=0; i<points.size(); i++) {
            float nangle = ofMap(i,0,points.size(),0,TWO_PI);
            float noiseDisplace = ofNoise(points[i].x+seed, points[i].y+seed, /*points[i].z+seed*/0);

            points[i] += noiseDisplace*0.25;
        }
        
        points.push_back(points[0]);
        points.push_back(points[1]);
        
    }
    
    void make(int steps){
        if(steps > 1){
            delete(child);
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
    
    void drawAsMesh(float p) {
        p = ofClamp(p, 0, 1);
        
        ofMesh mesh;
        int left = resolution * p;
        
        if(left>0) {
        //cout<<p<<endl;
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        for (int i=1; i<left; i++) {

            //find this point and the next point
            ofVec3f thisPoint = points[i-1];
            ofVec3f nextPoint = points[i];
            
            //get the direction from one to the next.
            ofVec3f direction = (nextPoint - thisPoint);
            
            //get the distance from one point to the next
            float distance = direction.length();
            
            //get the normalized direction. normalized vectors always have a length of one
            //and are really useful for representing directions as opposed to something with length
            ofVec3f unitDirection = direction.normalized();
            
            //find both directions to the left and to the right
            ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
            ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));
            
            //use the map function to determine the distance.
            //the longer the distance, the narrower the line.
            //this makes it look a bit like brush strokes
            float thickness = ofMap(ofSignedNoise(points[i].x,points[i].y,points[i].z), 0, 1, 0.01, 0.011);
            
            //calculate the points to the left and to the right
            //by extending the current point in the direction of left/right by the length
            ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
            ofVec3f rightPoint = thisPoint+toTheRight*thickness;
            
            //add these points to the triangle strip
            mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f,1.0f));
            mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
            mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f,1.0f));
            mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
            

            
        }

        ofxMeshUtils::calcNormals(mesh);
        mesh.draw();
            
        /*vbo.setMesh(mesh, GL_STREAM_DRAW);
        vbo.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.getNumVertices());
        vbo.unbind();
        */
        //ofNoFill();
        //ofCircle(points[left],0.01);
        }
        
    }
    
    void drawActiveRings(float seconds) {
        
        
        if(step != -1) {
        start = p_tlStart->getKeyframes()[step]->time / 1000;
        growDuration = ofMap(p_tlStart->getKeyframes()[step]->value,
                             0,1,1,600);
                             
        
        if(seconds > start) {
                float elapsed = (seconds-start);
                float p = elapsed/growDuration;
                drawAsMesh(p);
        }
        
        }
            
        if(child) {
            child->drawActiveRings(seconds);
        }
        
        
        
    }
    
    
};


class TrunkRings : public ContentScene {
    
public:
    
    ofxOlaShaderLight::Material material;
    
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
    
    ofxTLCurves * tlStartRing;
    ofxTLCurves * tlRadius;
    ofxTLCurves * tlSpeed;
    ofxTLCurves * tlNoise;
    
    ofxTLCurves * tlKnockover;
    ofxTLCurves * tlRotateZ;
    ofxTLCurves * tlRotateY;
    
    //ofVec3f ringHistory [TAIL_LENGTH];
    
    vector<Ring *> rings;
    
    Ring * center;
    
    int numRings = 0;
    
    
};
