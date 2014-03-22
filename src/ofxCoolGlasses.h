#pragma once

#include "ofMain.h"

class ofxCoolGlasses {
    
public:
    
    ofCamera left;
    ofCamera right;
    
    ofxCoolGlasses(): physical_eye_seperation_cm(6.5), physical_focus_distance_cm(200), focus_distance(200) {};
    ~ofxCoolGlasses(){};
    
	void setup(int _w, int _h);
    
    void setPosition(float x, float y, float z) {
        setPosition(ofVec3f(x,y,z));
    }
    
    void setPosition(ofVec3f pos) {
        left.setPosition(pos);
        right.setPosition(pos);
    }
    
    void setupPerspective(){
        left.setupPerspective();
        right.setupPerspective();
    }
    
    void lookAt(ofVec3f v){
        left.lookAt(v);
        right.lookAt(v);
    }
    
	void update(ofRectangle viewport = ofGetCurrentViewport());

	void setPhysicalEyeSeparation(float cm) {
        physical_eye_seperation_cm = cm; }
	void setPhysicalFocusDistance(float cm) {
        physical_focus_distance_cm = cm; }
	void setFocusDistance(float v) {
        focus_distance = v; }

	void beginLeft()
	{
        currentCam = &left;
        leftFbo.begin();
        ofClear(0,0,0,0);
        left.begin();
	}

	void endLeft()
	{
        left.end();
        leftFbo.end();
	}

	void beginRight()
	{
        currentCam = &right;
        rightFbo.begin();
        ofClear(0,0,0,0);
        right.begin();
    }

	void endRight()
	{
	    right.end();
        rightFbo.end();
	}
    
    ofCamera * getCurrentCam() {
        return currentCam;
    };
    
    ofFbo leftFbo, rightFbo;
    
protected:
    ofCamera * currentCam;
    
	int width, height;

	float eye, focus_distance;

	float physical_eye_seperation_cm;
	float physical_focus_distance_cm;

	// parallel
	float zNear, zFar;
	float fovx_tan, fovy_tan;
	float aspect;
	
private:

};