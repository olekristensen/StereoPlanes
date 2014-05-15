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

using namespace ofxCv;
using namespace cv;

class KinectUser {
public:
    ofVec3f pos;
    ofVec3f filteredPos;
    cv::KalmanFilter KF;
	cv::Mat_<float> measurement;
    
    float lastTimeMoved;
    float birthTime;
    bool dead;

    void setup(){
    	KF.init(6, 3, 0);
        /*
         KF.transitionMatrix = *(Mat_<float>(4, 4) <<
         1,0,1,0,
         0,1,0,1,
         0,0,1,0,
         0,0,0,1);
         */
    	KF.transitionMatrix = *(Mat_<float>(6, 6) <<
                                1,0,0,0.05,0,0,
                                0,1,0,0,0.05,0,
                                0,0,1,0,0,0.05,
                                0,0,0,1,0,0,
                                0,0,0,0,1,0,
                                0,0,0,0,0,1 );
        
        measurement = Mat_<float>::zeros(3,1);
        
        KF.statePre.at<float>(0) = 0;
        KF.statePre.at<float>(1) = 0;
        KF.statePre.at<float>(2) = 0;
        KF.statePre.at<float>(3) = 0;
        KF.statePre.at<float>(4) = 0;
        KF.statePre.at<float>(5) = 0;
        
        setIdentity(KF.measurementMatrix);
        setIdentity(KF.processNoiseCov, Scalar::all(.5e-4)); // smoother with lower value
        setIdentity(KF.measurementNoiseCov, Scalar::all(1e-2)); // smoother with higher value
        setIdentity(KF.errorCovPost, Scalar::all(.1));
        
        dead = false;
        birthTime =0;

    }
    
    void update(){
        // First predict, to update the internal statePre variable
        Mat prediction = KF.predict();
        cv::Point3d predictPt(prediction.at<float>(0),prediction.at<float>(1),prediction.at<float>(2));
        filteredPos.x = predictPt.x;
        filteredPos.y = predictPt.y;
        filteredPos.z = predictPt.z;
        
        // Get light point
        measurement(0) = pos.x;
        measurement(1) = pos.y;
        measurement(2) = pos.z;
        
        cv::Point3d measPt(measurement(0),measurement(1), measurement(2));
        
        // The "correct" phase that is going to use the predicted value and our measurement
        Mat estimated = KF.correct(measurement);
        cv::Point3d statePt(estimated.at<float>(0),estimated.at<float>(1),estimated.at<float>(2));
        
        float secondsToLive = 4;
        float secondsToFadeUp = 0.1;
        float secondsToFadeDown = 0.1;
        float now = ofGetElapsedTimef();
        float timeSincePoke = now - lastTimeMoved;
        
        float fadeFactor = 0.;
        
        if(timeSincePoke > secondsToLive){
            // fading down
            fadeFactor = ((secondsToLive+secondsToFadeDown)-timeSincePoke)/secondsToFadeDown;
            fadeFactor = fmax(fadeFactor, 0.);
            
            if (fadeFactor <= 0) {
                dead = true;
            }
            
            birthTime = -1;
        } else {
            if (birthTime > 0) {
                //fading up
                fadeFactor = (now - birthTime) / secondsToFadeUp;
                fadeFactor = fminf(1., fadeFactor);
            }
        }
    }
    
    void setPosition(float px, float py, float pz){
        pos.set(px, py, pz);
        lastTimeMoved = ofGetElapsedTimef();
        dead = false;
        if(birthTime < 0){
            birthTime = lastTimeMoved;
        }
    }
    
    void draw(){
        ofPushStyle();
        ofSetColor(ofColor::green);
        ofDrawSphere(pos, 0.05);
        ofSetColor(ofColor::red);
        ofDrawSphere(filteredPos, 0.05);
        ofPopStyle();
    }
    
};

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
    
    vector<userLight*> lights;
    
    vector<KinectUser*> users;
    
    bool allLightsDead;
    float globalLigtsDeadFactor;

    bool drawDebug;
    
};

