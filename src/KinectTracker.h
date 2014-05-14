//
//  KinectTracker.h
//  Trae
//
//  Created by Ole Kristensen on 10/05/14.
//
//

#pragma once
#include "ofMain.h"
#include "ContentScene.h"
#include "ofxMeshUtils.h"
#include "ofxOlaShaderLight.h"
#include "ofxKinect.h"
#include "ofxCv.h"
#include "userLight.h"

class KinectTracker : public ContentScene {
    
public:
    
    ofxOlaShaderLight::Material material;
    
    void draw(int _surfaceId);
    void update();
    void setup();
    
    void setGui(ofxUICanvas * gui, float width);
    void guiEvent(ofxUIEventArgs &e);
    void receiveOsc(ofxOscMessage * m, string rest);
        
    ofxTLCurves * tlKinectScale;
    
    ofxTLCurves * tlKinectX;
    ofxTLCurves * tlKinectY;
    ofxTLCurves * tlKinectZ;
    
    ofxTLCurves * tlKinectRotX;
    ofxTLCurves * tlKinectRotY;
    ofxTLCurves * tlKinectRotZ;

    ofxTLCurves * tlOpenCVLearningTime;
    ofxTLCurves * tlOpenCVThreshold;
    ofxTLCurves * tlOpenCVBlur;
    
    ofxTLCurves * tlBlobThreshold;
    ofxTLCurves * tlBlobMinSize;
    ofxTLCurves * tlBlobMaxSize;
    
    ofxKinect kinect;
    
    ofImage depthImage;
    
    void updatePointCloud();
    void drawPointCloud();
    
	int nearThreshold;
	int farThreshold;
    
    ofMesh kinectMesh;
    ofVec3f kincetClosestPoint;
    ofVec3f kinectOrigin;
    ofVec3f kinectRotation;
    float kinectScale;
    
    bool setBackground;
    
    // open cv
    
	ofxCv::RunningBackground background;
    ofxCv::ContourFinder contourFinder;
    ofImage thresholded;
    
    // cropping
    
    ofBoxPrimitive cropBox;
    
    // lights
    
    vector<userLight> lights;
    
    bool allLightsDead;
    float globalLigtsDeadFactor;

    
};

