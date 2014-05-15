//
//  kinectLight.cpp
//  Multiplicity
//
//  Created by Ole Kristensen on 10/11/13.
//
//

#include "userLight.h"
#include "testApp.h"

using namespace ofxCv;
using namespace cv;

userLight::userLight(){
    ofxOlaShaderLight::ofxOlaShaderLight();
}

void userLight::setup(){

    ofxOlaShaderLight::setup();
    
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

void userLight::setPosition(float px, float py, float pz){
    pos.set(px, py, pz);
    lastTimeMoved = ofGetElapsedTimef();
    dead = false;
    if(birthTime < 0){
        birthTime = lastTimeMoved;
    }
}

void userLight::setColor(const ofFloatColor &c){
    ofxOlaShaderLight::setDiffuseColor(ofColor(c.r*255.0,c.g*255.0,c.b*255.0,255.0));
    ofxOlaShaderLight::setAttenuation(1./.5);

    color = c;
    //cout << getLightID() << ": set color" << endl;
}

void userLight::update(){
    // First predict, to update the internal statePre variable
	Mat prediction = KF.predict();
    cv::Point3d predictPt(prediction.at<float>(0),prediction.at<float>(1),prediction.at<float>(2));
    ofxOlaShaderLight::setGlobalPosition(predictPt.x,predictPt.y,predictPt.z);
	// Get light point
	measurement(0) = pos.x;
	measurement(1) = pos.y;
    measurement(2) = pos.z;
	
	cv::Point3d measPt(measurement(0),measurement(1), measurement(2));
    
	// The "correct" phase that is going to use the predicted value and our measurement
	Mat estimated = KF.correct(measurement);
	cv::Point3d statePt(estimated.at<float>(0),estimated.at<float>(1),estimated.at<float>(2));
    
    testApp * theApp = (testApp*)ofGetAppPtr();
    
    float secondsToLive = 4;
    float secondsToFadeUp = 2;
    float secondsToFadeDown = 5;
    float now = ofGetElapsedTimef();
    float timeSincePoke = now - lastTimeMoved;
    
    float globalFadeUpFactor = .25 + (theApp->kinectTracker->globalLigtsDeadFactor * .5);
    
    ofFloatColor c = color;

    float fadeFactor = 0.;
    
    if(timeSincePoke > secondsToLive){
        // fading down
        fadeFactor = ((secondsToLive+secondsToFadeDown)-timeSincePoke)/secondsToFadeDown;
        fadeFactor = fmax(fadeFactor, 0.);
        
        ofBoxPrimitive cropBox = theApp->kinectTracker->cropBox;
        float lightRandomSpeed = 0.1;
        int id = getLightID();
        
        ofVec3f homePos(cropBox.getPosition().x + (ofSignedNoise(ofGetElapsedTimef()*lightRandomSpeed, id, id)*cropBox.getWidth()/1.5),
                       cropBox.getPosition().y + (cropBox.getHeight()*-1.5),
                       (cropBox.getPosition().z + 6) + (theApp->kinectTracker->globalLigtsDeadFactor*ofSignedNoise(id, id, ofGetElapsedTimef()*lightRandomSpeed)*cropBox.getDepth()) + ((1.-theApp->kinectTracker->globalLigtsDeadFactor)*cropBox.getDepth()));
        homePos.x*=theApp->kinectTracker->globalLigtsDeadFactor;
        if (fadeFactor <= 0) {
            dead = true;
            pos.set(homePos*0.1 + pos*0.9);
        } else {
            pos.set(homePos*0.01 + pos*0.99);
        }
        
        birthTime = -1;
    } else {
        if (birthTime > 0) {
            //fading up
            fadeFactor = (now - birthTime) / secondsToFadeUp;
            fadeFactor = fminf(1., fadeFactor);
        }
    }
    float brightness = 1;//(fadeFactor*color.getBrightness()) + ((1.-fadeFactor)*globalFadeUpFactor*color.getBrightness());
    c.setBrightness(brightness);
    float saturation = (fadeFactor*color.getSaturation()*0.25) + (0.75*color.getSaturation());
    c.setSaturation(saturation);
    ofxOlaShaderLight::setDiffuseColor(c);
}

void userLight::customDraw() {
    ofDrawSphere( 0, 0, 0, 0.05);
    ofDrawAxis(0.1);
}