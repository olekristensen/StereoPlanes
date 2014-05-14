//
//  KinectTracker.cpp
//  Trae
//
//  Created by Ole Kristensen on 10/05/14.
//
//

#include "KinectTracker.h"

void KinectTracker::setup(){
    
    name = "Kinect";
    oscAddress = "/kinect";
    
    mainTimeline->addPage(name);
    
    tlKinectX = mainTimeline->addCurves("Kinect X", ofRange(-10, 10));
    tlKinectY = mainTimeline->addCurves("Kinect Y", ofRange(-10, 10));
    tlKinectZ = mainTimeline->addCurves("Kinect Z", ofRange(-10, 10));

    tlKinectRotX = mainTimeline->addCurves("Kinect Rot X", ofRange(-180, 180));
    tlKinectRotY = mainTimeline->addCurves("Kinect Rot Y", ofRange(-180, 180));
    tlKinectRotZ = mainTimeline->addCurves("Kinect Rot Z", ofRange(-180, 180));
    
    tlKinectScale = mainTimeline->addCurves("Kinect Scale", ofRange(0, 0.05));
    
	// enable depth->video image calibration
	kinect.setRegistration(false);
    
    kinect.setDepthClipping(2000,6000);
    
	kinect.init();
	kinect.open();
    
    depthImage.allocate(kinect.width, kinect.height, OF_IMAGE_COLOR_ALPHA);
    
    // print the intrinsic IR sensor values
	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: "    << kinect.getSensorEmitterDistance()    << "cm";
		ofLogNotice() << "sensor-camera dist:  "    << kinect.getSensorCameraDistance()     << "cm";
		ofLogNotice() << "zero plane pixel size: "  << kinect.getZeroPlanePixelSize()       << "mm";
		ofLogNotice() << "zero plane dist: "        << kinect.getZeroPlaneDistance()        << "mm";
	}
    
}

void KinectTracker::update(){
    kinect.update();
    
    if(kinect.isFrameNewDepth()){
        depthImage.setFromPixels(kinect.getDepthPixelsRef());
        depthImage.setImageType(OF_IMAGE_COLOR);
        depthImage.update();
        updatePointCloud();
        ofxCv::blur(depthImage, 2);
        depthImage.update();
        thresholded.update();
        background.setLearningTime(900);
        background.setThresholdValue(50);
        background.update(depthImage, thresholded);
        if(ofGetElapsedTimef() < 5.0){
            background.reset();
        }
        thresholded.update();
        contourFinder.findContours(thresholded);
        int n = contourFinder.size();
        
        int step = 4;
        
        for(int i = 0; i < n; i++) {
            cv::Rect boundingRect = contourFinder.getBoundingRect(i);
            vector <cv::Point> contour = contourFinder.getContour(i);
            
            kincetClosestPoint = ofVec3f(0,1000,0);
        
            bool foundPoint = false;
            
            for (int x=boundingRect.x; x < boundingRect.width+boundingRect.x; x+=step) {
                for (int y=boundingRect.y; y < boundingRect.height+boundingRect.y; y+=step) {
                    if(kinect.getDistanceAt(x, y) > 0 && pointPolygonTest(contour, cv::Point(x,y), false)) {
                        ofVec3f v = kinect.getWorldCoordinateAt(x, y) * ofVec3f(1,1,1);
                        v.rotate(kinectRotation.x, kinectRotation.y, kinectRotation.z);
                        v *= ofVec3f(kinectScale, kinectScale, kinectScale);
                        v += kinectOrigin;
                        if(v.y < kincetClosestPoint.y) {
                            kincetClosestPoint = v;
                            foundPoint = true;
                        }
                    }
                }
            }
            if(foundPoint){
                /*
                float closestLightDistance = 10000;
                userLight* closestLight = NULL;
                
                bool foundCloseLight = false;
                for(std::vector<userLight>::iterator ul = lights.begin() ; ul != lights.end(); ++ul){
                    float distanceToLight = kincetClosestPoint.distance(ul->pos);
                    if(distanceToLight < closestLightDistance && !ul->dead ){
                        closestLightDistance = distanceToLight;
                        closestLight = &(*ul);
                    }
                }
                if (closestLight != NULL) {
                    //                    cout << "blob " << i << " distance to light " << closestLight->getLightID() << ": " << closestLightDistance << endl;
                }
                
                
                if( closestLightDistance < 30){
                    closestLight->setPosition(kincetClosestPoint.x, kincetClosestPoint.y, kincetClosestPoint.z);
                    //                    cout << "light " << closestLight->getLightID() << " was close" << endl;
                    //                    cout << "we set position to " << kincetClosestPoint.x << ", " << kincetClosestPoint.y << ", " << kincetClosestPoint.z << endl;
                } else {
                    //                cout << "blob " << i << " did not find light" << endl;
                    for(std::vector<userLight>::iterator ul = lights.begin() ; ul != lights.end(); ++ul){
                        if (ul->dead) {
                            //                        cout << "light " << ul->getLightID() << " was dead" << endl;
                            //                        cout << "we set position to " << kincetClosestPoint.x << ", " << kincetClosestPoint.y << ", " << kincetClosestPoint.z << endl;
                            ul->setPosition(kincetClosestPoint.x, kincetClosestPoint.y, kincetClosestPoint.z);
                            break;
                        }
                    }
                }
                 */
            }
        }
    }
    
    cropBox.set(3.0,2.0,2.0);
    cropBox.setPosition(0,0,1.05);

}

void KinectTracker::draw(int _surfaceId){
    
	glPointSize(3);
	ofPushMatrix();
    ofDisableDepthTest();
    bool lightsWereEnabled = ofxOlaShaderLight::isEnabled();
    ofxOlaShaderLight::end();
	kinectMesh.drawVertices();
    ofSetColor(ofxCv::cyanPrint);
    cropBox.drawWireframe();
    ofSetColor(ofColor::white);
	ofPopMatrix();
    ofPushMatrix();
    ofTranslate(kinectOrigin);
    ofRotateX(kinectRotation.x);
    ofRotateY(kinectRotation.y);
    ofRotateZ(kinectRotation.z);
    ofScale(kinectScale, kinectScale, kinectScale);
    ofDrawAxis(10);
    ofDrawSphere(10);
    ofPopMatrix();
    ofPushMatrix();
    ofTranslate(kincetClosestPoint);
    ofDrawSphere(0.2);
    ofPopMatrix();
    if(lightsWereEnabled) ofxOlaShaderLight::begin();
    ofEnableDepthTest();
    
}

void KinectTracker::updatePointCloud() {
	int w = 640;
	int h = 480;
    
	int step = 2;
    
    bool maskBox = true;
    
    bool kinectLighting = true;
    kincetClosestPoint = ofVec3f(0,0,10000);
    kinectOrigin = ofVec3f(tlKinectX->getValue(), tlKinectY->getValue(), tlKinectZ->getValue());
    kinectRotation = ofVec3f(tlKinectRotX->getValue(), tlKinectRotY->getValue(), tlKinectRotZ->getValue());
    kinectScale = tlKinectScale->getValue();
    
    kincetClosestPoint.rotate(kinectRotation.x, kinectRotation.y, kinectRotation.z);
    kincetClosestPoint *= ofVec3f(kinectScale,kinectScale, kinectScale);
    kincetClosestPoint += kinectOrigin;
    
    ofColor outsideColor = ofColor::blueSteel;
    outsideColor.a = 20;
    ofColor insideColor = ofColor::white;
    insideColor.a = 127;
    
    ofMesh * lineMeshes = new ofMesh[h];
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
    
    dispatch_apply(h / step, queue, ^(size_t yIdx) {
        int y = yIdx * step;
        ofMesh lineMesh;
        //    for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinect.getDistanceAt(x, y) > 0) {
                ofVec3f v = kinect.getWorldCoordinateAt(x, y) * ofVec3f(1,1,1);
                v.rotate(kinectRotation.x, kinectRotation.y, kinectRotation.z);
                v *= ofVec3f(kinectScale,kinectScale, kinectScale);
                v += kinectOrigin;
				if(!maskBox ||
                   (v.x > cropBox.getPosition().x - cropBox.getWidth()/2. &&
                    v.x < cropBox.getPosition().x + cropBox.getWidth()/2. &&
                    v.y > cropBox.getPosition().y - cropBox.getHeight()/2. &&
                    v.y < cropBox.getPosition().y + cropBox.getHeight()/2. &&
                    v.z > cropBox.getPosition().z - cropBox.getDepth()/2. &&
                    v.z < cropBox.getPosition().z + cropBox.getDepth()/2.
                    )
                   ){
                    
                    lineMeshes[y].addColor(insideColor);
                    if(kinectLighting && v.distance(kinectOrigin) < kincetClosestPoint.distance(kinectOrigin)) {
                        kincetClosestPoint = v;
                    }
                    
                } else {
                    lineMeshes[y].addColor(outsideColor);
                    // openCV treshold cropping
                    for (int pixX = x-step; pixX < x+step; pixX++) {
                        for (int pixY = y-step; pixY < y+step; pixY++) {
                            
                            int clippedX = min(depthImage.width-1, max(0, pixX));
                            int clippedY = min(depthImage.height-1, max(0, pixY));
                            
                            depthImage.setColor(clippedX, clippedY, ofColor(0));
                        }
                    }
                }
                lineMeshes[y].addVertex(v);
			}
            
		}
        //}
    });
    
    
    thresholded.update();
    
    kinectMesh.clear();
	kinectMesh.setMode(OF_PRIMITIVE_POINTS);
    
    for (int y = 0; y < h; y+=step) {
        kinectMesh.append(lineMeshes[y]);
    }
    
    delete [] lineMeshes;
    
}


void KinectTracker::setGui(ofxUICanvas * gui, float width){
    ContentScene::setGui(gui, width);
}

void KinectTracker::receiveOsc(ofxOscMessage * m, string rest) {
}

void KinectTracker::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.getName();
	int kind = e.getKind();
	//cout << "got event from: " << name << endl;
    
}

