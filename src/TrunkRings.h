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
    int   resolution = 160;//400;
    float seed;
    int   step = -1; // first one is just the controller we dont draw it
    float variance = 4;
    float scale = 60;
    float startangle;
    
    float radiusVariance = 0.005;
    
    float growDuration = 5; // How many seconds to draw one ring
    float start = 2;
    
    bool meshReady = false;
    
    ofMesh mesh;
    
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
    
    void updateMesh(float p) {
        
        p = ofClamp(p, 0, 1);
        float left = float(resolution) * p;
        if(p<1) { meshReady = false; }
        
        if(left>0 || !meshReady) {
        mesh.clear();
        mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
            
        for (int i=1; i<ceil(left); i++) {
            
            //find this point and the next point
            ofVec3f thisPoint = points[i-1];
            ofVec3f nextPoint = points[i];
            
            float pOfLast = left - float(i+1);
            if(pOfLast < 1) {
                // We are drawinf the last one
                // Lets lerp to the next point
                
                nextPoint.interpolate(thisPoint, pOfLast);
                
            }
            
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
            ofVec3f normalRight = toTheRight.getRotated(-90, unitDirection);
            ofVec3f normalLeft = toTheLeft.getRotated(-90, unitDirection);
            
            //use the map function to determine the distance.
            //the longer the distance, the narrower the line.
            //this makes it look a bit like brush strokes
            float thickness = ofMap(ofSignedNoise(points[i].x,points[i].y,points[i].z), 0, 1, 0.005, 0.01);
            
            //calculate the points to the left and to the right
            //by extending the current point in the direction of left/right by the length
            ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
            ofVec3f rightPoint = thisPoint+toTheRight*thickness;
            
            //add these points to the triangle strip
  
            mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f,1.0f));
            mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
            mesh.addNormal(ofVec3f(normalRight));
            mesh.addColor(ofFloatColor(0.8f,0.8f,0.8f,1.0f));
            mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
            mesh.addNormal(ofVec3f(normalRight));

            // TODO animate last link in - interpolate to next point
        }
        meshReady = (p==1);
        
        //vbo.setMesh(mesh, GL_STREAM_DRAW);
        mesh.draw();
        //mesh.drawWireframe();
            
            /* draw normals
            ofxOlaShaderLight::end();
            
            vector<ofVec3f> n = mesh.getNormals();
            vector<ofVec3f> v = mesh.getVertices();
            float normalLength = .1;
            
                ofSetColor(255,255,255,127);
                for(unsigned int i=0; i < n.size() ;i++){
                    ofLine(v[i].x,v[i].y,v[i].z,
                           v[i].x+n[i].x*normalLength,v[i].y+n[i].y*normalLength,v[i].z+n[i].z*normalLength);
                    
                    ofLine(.98*v[i].x,.98*v[i].y,.98*v[i].z,
                           .98*v[i].x+n[i].x*normalLength*.2,.98*v[i].y+n[i].y*normalLength*.2,.98*v[i].z+n[i].z*normalLength*.2);
                    ofLine(.98*v[i].x+n[i].x*normalLength*.2,.98*v[i].y+n[i].y*normalLength*.2,.98*v[i].z+n[i].z*normalLength*.2,
                           v[i].x+n[i].x*normalLength*.2,v[i].y+n[i].y*normalLength*.2,v[i].z+n[i].z*normalLength*.2);
                }
        
            ofxOlaShaderLight::begin();
            
 */
        //
        //ofNoFill();
        //ofCircle(points[left],0.01);
        }
        
    }
    
    void drawVbo() {
        /*vbo.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, vbo.getNumVertices());
        vbo.unbind();
         */
    }
    
    void drawActiveRings(float seconds) {
        
        
        if(step != -1) {
        start = p_tlStart->getKeyframes()[step]->time / 1000;
        growDuration = ofMap(p_tlStart->getKeyframes()[step]->value,
                             0,1,1,120);
                             
        
        if(seconds > start) {
                float elapsed = (seconds-start);
                float p = elapsed/growDuration;
                updateMesh(p);
                //drawVbo();
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
    ofxTLCurves * tlX;
    ofxTLCurves * tlY;
    ofxTLCurves * tlZ;
    ofxTLCurves * tlNoise;
    
    ofxTLCurves * tlKnockover;
    ofxTLCurves * tlRotateZ;
    ofxTLCurves * tlRotateY;
    
    //ofVec3f ringHistory [TAIL_LENGTH];
    
    vector<Ring *> rings;
    
    Ring * center;
    
    int numRings = 0;
    
    ofShader fxaa;

    
    
};
