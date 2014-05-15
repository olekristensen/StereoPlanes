//
//  userLight.h
//  Multiplicity
//
//  Created by Ole Kristensen on 10/11/13.
//
//

#ifndef __Multiplicity__userLight__
#define __Multiplicity__userLight__

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOlaShaderLight.h"

class userLight : public ofxOlaShaderLight {
public:
    
    userLight();
    
    void setup();
    
    void setPosition(float px, float py, float pz);
    
    void update();
    
    void setColor(const ofFloatColor &c);
    
    void customDraw();
    
    float lastTimeMoved;
    float birthTime;
    bool dead;
    
    ofFloatColor color;
	
    ofPolyline predicted, line;
    
    ofVec3f pos;
    
	cv::KalmanFilter KF;
	cv::Mat_<float> measurement;

};
#endif /* defined(__Multiplicity__userLight__) */
