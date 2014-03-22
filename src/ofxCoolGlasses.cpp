//
//  ofxCoolGlasses.cpp
//  Trae
//
//  Created by Johan Bichel Lindegaard on 22/03/14.
//
//

#include "ofxCoolGlasses.h"

void ofxCoolGlasses::setup(int _w, int _h) {
    
    height = 2.;
    width = height *(_w/_h);
    
    leftFbo.allocate(_w,_h, GL_RGBA);
    rightFbo.allocate(_w, _h, GL_RGBA);
    
    leftFbo.begin();
    ofClear(0,0,0,0);
    leftFbo.end();
    
    rightFbo.begin();
    ofClear(0,0,0,0);
    rightFbo.end();
    
    left.setScale(1, 1, 1);
    left.setNearClip(0.1);
    
    right.setScale(1, 1, 1);
    right.setNearClip(0.1);
}

void ofxCoolGlasses::update(ofRectangle viewport) {
        
		eye = physical_eye_seperation_cm / physical_focus_distance_cm;
        
		/*aspect = viewport.width / viewport.height;
         fovy_tan = tanf(PI * left.getFov() / 360.);
         fovx_tan = fovy_tan * aspect;
         */
        
		//zNear = left.getNearClip();
		//zFar  = left.getFarClip();
        
        right.setPosition(left.getPosition().x + eye, left.getPosition().y, left.getPosition().z);
        
        right.setupOffAxisViewPortal(viewport.getTopLeft(), viewport.getBottomLeft(), viewport.getBottomRight());
        left.setupOffAxisViewPortal(viewport.getTopLeft(), viewport.getBottomLeft(), viewport.getBottomRight());
        
}