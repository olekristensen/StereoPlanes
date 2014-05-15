#include "testApp.h"
#include <OpenGL/OpenGL.h>
#include <ofGLUtils.h>


//--------------------------------------------------------------
void testApp::setup()
{
    int resX = 2560;
    int resY = 720;
    
    speed = 0;
    //time = 0;
    
    ofSetWindowTitle("Trae - openframeworks");

    //ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofSetFrameRate(30);
    ofSetVerticalSync(true);
    //    ofSetBackgroundAuto(true);
    ofBackground(0);
    
    //leftOutputServer.setName("Left");
    //rightOutputServer.setName("Right");
    sbsOutputServer.setName("Side By Side");
    
    timeline.setup();
    timeline.setupFont("GUI/Arial.ttf", 7);
    timeline.setDurationInSeconds(60*15);
    //timeline.setFrameRate(ofGetFrameRate());
    
    tlAudioMain = timeline.addAudioTrack("start opbyg");
    
    tlAudioMain->loadSoundfile("membrane-barkpop.wav");
    
    timeline.setLoopType(OF_LOOP_NORMAL);
    
    timeline.setBPM(120.f);
    //tlAudioMain = timeline.addAudioTrack("Audio", "tre-opbyg-beat.wav");
    
	ofAddListener(timeline.events().bangFired, this, &testApp::bangFired);
    
    timeline.addPage("Camera");
    tlCamX = timeline.addCurves("X");
    tlCamX->setValueRangeMin(-1);
    tlCamX->setValueRangeMax(1);
    tlCamY = timeline.addCurves("Y");
    tlCamY->setValueRangeMin(-1);
    tlCamY->setValueRangeMax(1);
    tlCamZ = timeline.addCurves("Z");
    tlCamZ->setValueRangeMin(-4);
    tlCamZ->setValueRangeMax(-0.25);
    
    ofFbo::Settings fboSettings;
    
    fboSettings.width = resX;
    fboSettings.height = resY;
    fboSettings.textureTarget = GL_TEXTURE_RECTANGLE_ARB;
    
    fbo.allocate(fboSettings);
    
    fbo.begin();
    ofClear(0,0,0,0);
    fbo.end();
    
    settings.load("stereoplanes.xml");
    
    wall = new StereoPlane("wall");
    wall->setup(resX/2, resY, &settings);
    wall->pos = ofVec2f(0,0);
    planes.push_back(wall);
    
    //activePlaneIndex = 0;
    //activePlane = planes[activePlaneIndex];
    
    oscReceiver.setup(9001);
    
    camPosWall = ofVec3f(0, 0, -1);
    
    lights = new Lights();
    contentScenes.push_back(lights);
    
    trae = new Trae();
    contentScenes.push_back(trae);

    kinectTracker = new KinectTracker();
    contentScenes.push_back(kinectTracker);

    for(int i=0; i<contentScenes.size(); i++) {
        //contentScenes[i]->mainTimeline = &timeline;
        contentScenes[i]->setupScene(i, &timeline);
        contentScenes[i]->enabled = true;
    }
    
    // add all parameterGroups from scenes to parameters
    // gui.setup(parameters);
    
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float width = 300-xInit;
    
    gui = new ofxUIScrollableCanvas(0, 0, width+xInit, ofGetHeight());
    
    gui->setScrollableDirections(false, true);
    gui->setTheme(OFX_UI_THEME_DEFAULT);
    
    gui->setFont("GUI/Arial.ttf");
    gui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    gui->setFontSize(OFX_UI_FONT_SMALL, 5);
    
    gui->addLabel("trae", OFX_UI_FONT_LARGE);
    
    gui->addFPS();
    gui->addSlider("Eye seperation", 0, 7, &eyeSeperation);
    
    /*gui->addSlider("Wall X",  -2, 2, &camPosWall.x);
    gui->addSlider("Wall Y",  -1, 1, &camPosWall.y);
    gui->addSlider("Wall Z",  -4, -0.25, &camPosWall.z);
*/
    
    gui->addSlider("Aspect",  0.0, 2.0, &aspect);
    //gui->addSlider("Speed", -1, 1, &speed);
    
    gui->addSpacer(width, 3)->setDrawOutline(true);
    
    //gui->addSlider("Vertex Noise", -1, 1, &vertexNoise);

    //gui->addSlider("Dancer X",  -1, 1, &dancerPos.x);
    //gui->addSlider("Dancer Y",  -1, 1, &dancerPos.y);

    gui->addToggle("Draw Checkers", &drawChessboards);
    gui->addToggle("Draw Planes", &drawGrids);
    gui->addToggle("Draw FBOs", &drawFBOs);
    gui->addToggle("Preview SBS", &previewSideBySide);
    
    for(int i=0; i<contentScenes.size(); i++) {
        gui->addSpacer(width, 3)->setDrawOutline(true);
        contentScenes[i]->setGui(gui, width);
    }
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    
    gui->setScrollAreaToScreenHeight();
    
    gui->loadSettings("GUI/guiSettings.xml");
    
}



//--------------------------------------------------------------
void testApp::bangFired(ofxTLBangEventArgs& args){
	//cout << "bang fired!" << args.flag << endl;
}


//--------------------------------------------------------------
void testApp::update()
{
    gui->setScrollArea(0, timeline.getHeight(), 300, ofGetHeight()-timeline.getHeight());
    
    camPosWall = ofVec3f(tlCamX->getValue(),tlCamY->getValue(),tlCamZ->getValue());

    while(oscReceiver.hasWaitingMessages()){
		// get the next message
		ofxOscMessage m;
		oscReceiver.getNextMessage(&m);
        
        for(int i=0; i<contentScenes.size(); i++) {
            contentScenes[i]->parseSceneOsc(&m);
        }
        
        //cout<<m.getAddress()<<endl;
        
		if(m.getAddress() == "/Wall/Camera/x"){
                camPosWall.x = m.getArgAsFloat(0);
            
        } else if(m.getAddress() == "/Wall/Camera/y"){
            camPosWall.y = m.getArgAsFloat(0);
            
        } else if(m.getAddress() == "/Wall/Cameraz/x"){
                camPosWall.z = m.getArgAsFloat(0);
            
		} else if(m.getAddress() == "/eyeSeperation/x"){
			eyeSeperation = m.getArgAsFloat(0);
            
		} else if(m.getAddress() == "/Dancer/x"){
            dancerPos.x = m.getArgAsFloat(0);
            
		} else if(m.getAddress() == "/Dancer/y"){
            dancerPos.y = m.getArgAsFloat(0);
            
		} else if(m.getAddress() == "/activescene/x"){
            for(int i=0; i<contentScenes.size(); i++) {
                contentScenes[i]->enabled = false;
            }
            contentScenes[m.getArgAsInt32(0)]->enabled = true;
        }
    }
    
    wall->cam.setPosition(camPosWall);
    wall->aspect = aspect;
    lights->updateCamPos(camPosWall);
    
    for(int i=0; i<planes.size(); i++) {
        planes[i]->cam.setPhysicalEyeSeparation(eyeSeperation);
        planes[i]->update();
    }

    for(int s=0; s<contentScenes.size();s++) {
        contentScenes[s]->update();
    }
    int t = 0;
    int nUsers = kinectTracker->users.size();

    for (std::vector<TreeNode*>::iterator it = trae->treeNodes.begin() ; it != trae->treeNodes.end(); ++it) {
        
        TreeNode * tNode = (*it);
        
        ofVec3f treeVec(tNode->getGlobalPosition());
        
        ofQuaternion rot = trae->cam.getOrientationQuat();
        treeVec.rotate(rot.w(), ofVec3f(rot.x(), rot.y(), rot.z()));
        treeVec.y -= 1.0;
        treeVec = treeVec.rotated(-180, 0, 0);
        treeVec.z += 1.0;
        treeVec += trae->cam.getPosition();
        
        int u = 0;
        for (std::vector<KinectUser*>::iterator iu = kinectTracker->users.begin() ; iu != kinectTracker->users.end(); ++iu) {
            KinectUser *kUser = (*iu);

            float distance = kUser->filteredPos.distance(treeVec);
            cout << distance << "\t";
            
            float alpha = 1.0 - ofClamp(distance, 0.0, 1.0);
            
            tNode->material.diffuseColor.w = alpha;
            u++;
        }
        t++;
    }
    
    cout << endl;
    
    //ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
}


void testApp::drawScenes(int _surfaceId) {
    
    for(int s=0; s<contentScenes.size();s++) {
        contentScenes[s]->drawScene(_surfaceId);
    }
    
/*    ofSetColor(255);

    for (std::vector<TreeNode*>::iterator it = trae->treeNodes.begin() ; it != trae->treeNodes.end(); ++it) {
        
        TreeNode * tNode = (*it);
        
        ofVec3f treeVec(tNode->getGlobalPosition());
        
        ofQuaternion rot = trae->cam.getOrientationQuat();
        treeVec.rotate(rot.w(), ofVec3f(rot.x(), rot.y(), rot.z()));
        treeVec.y -= 1.0;
        treeVec = treeVec.rotated(-180, 0, 0);
        treeVec.z += 1.0;
        treeVec += trae->cam.getPosition();

        ofDrawSphere(treeVec, 0.5);
        
    }
*/
    
}

//--------------------------------------------------------------
void testApp::draw()
{
    
    for(int s=0; s<contentScenes.size();s++) {
        contentScenes[s]->time = timeline.getCurrentTime();
    }
    
    ofSetColor(255);
    ofBackgroundGradient(ofColor::darkGrey, ofColor::gray);
    //drawScenes(0);
    
    ofEnableDepthTest();
    
    // draw scenes to surfaces, they are kept in the cameras fbo
    
    for(int i=0;i < planes.size(); i++) {
        planes[i]->beginLeft();
        ofClear(ofColor::black);
        glPushMatrix();
        lights->begin();
        drawScenes(i);
        lights->end();
        glPopMatrix();
        planes[i]->endLeft();
        
        planes[i]->beginRight();
        ofClear(ofColor::black);
        glPushMatrix();
        lights->begin();
        drawScenes(i);
        lights->end();
        glPopMatrix();
        planes[i]->endRight();
    }

    ofDisableDepthTest();
    
    if(drawChessboards) {
        for(int i=0; i<planes.size(); i++) {
            planes[i]->drawChessboards();
        }
    }

    if(drawGrids) {
        for(int i=0; i<planes.size(); i++) {
            planes[i]->drawGrids();
        }
    }
    
    // Draw the scenes to the output fbo
    fbo.begin(); {
        ofClear(0, 0, 0, 0);
        ofSetColor(255,255);
        ofFill();
        for(int i=0; i<planes.size(); i++) {
            planes[i]->draw();
        }
        
    }fbo.end();
    
    if(drawFBOs) {
        ofSetColor(255,255);
        float fboHeight = (ofGetWidth()-300)*(fbo.getHeight()*1./fbo.getWidth());
        if(previewSideBySide) {
            fbo.draw(300,timeline.getHeight(),(ofGetWidth()-300),fboHeight);
        } else {
            fbo.draw(300,timeline.getHeight(),(ofGetWidth()-300)*2,fboHeight*2);
        }
    }
    ofSetColor(64,255);
    ofRect(timeline.getDrawRect());
    ofSetColor(255,255);
    timeline.draw();
    kinectTracker->depthImage.draw(ofGetWidth()-320, 0, 320, 240);
    kinectTracker->thresholded.draw(ofGetWidth()-320, 240, 320, 240);
    ofPushMatrix();
    ofTranslate(ofGetWidth()-320, 0);
    ofScale(0.5, 0.5);
    kinectTracker->contourFinder.draw();
    ofPushStyle();
    ofSetLineWidth(1.0);
    ofNoFill();
    int n = kinectTracker->contourFinder.size();
    for(int i = 0; i < n; i++) {
        // smallest rectangle that fits the contour
        ofSetColor(ofxCv::cyanPrint);
        ofPolyline minAreRect = ofxCv::toOf(kinectTracker->contourFinder.getMinAreaRect(i));
        minAreRect.draw();
        
        // ellipse that best fits the contour
        ofSetColor(ofxCv::magentaPrint);
        cv::RotatedRect ellipse = kinectTracker->contourFinder.getFitEllipse(i);
        ofPushMatrix();
        ofVec2f ellipseCenter = ofxCv::toOf(ellipse.center);
        ofVec2f ellipseSize = ofxCv::toOf(ellipse.size);
        ofTranslate(ellipseCenter.x, ellipseCenter.y);
        ofRotate(ellipse.angle);
        ofEllipse(0, 0, ellipseSize.x, ellipseSize.y);
        ofPopMatrix();
        
        // minimum area circle that encloses the contour
        ofSetColor(ofxCv::cyanPrint);
        float circleRadius;
        ofVec2f circleCenter = ofxCv::toOf(kinectTracker->contourFinder.getMinEnclosingCircle(i, circleRadius));
        ofCircle(circleCenter, circleRadius);
        
        // convex hull of the contour
        ofSetColor(ofxCv::yellowPrint);
        ofPolyline convexHull = ofxCv::toOf(kinectTracker->contourFinder.getConvexHull(i));
        convexHull.draw();
        
        // defects of the convex hull
        vector<cv::Vec4i> defects = kinectTracker->contourFinder.getConvexityDefects(i);
        for(int j = 0; j < defects.size(); j++) {
            ofLine(defects[j][0], defects[j][1], defects[j][2], defects[j][3]);
        }
        
        // some different styles of contour centers
        ofVec2f centroid = ofxCv::toOf(kinectTracker->contourFinder.getCentroid(i));
        ofVec2f average = ofxCv::toOf(kinectTracker->contourFinder.getAverage(i));
        ofVec2f center = ofxCv::toOf(kinectTracker->contourFinder.getCenter(i));
        ofSetColor(ofxCv::cyanPrint);
        ofCircle(centroid, 1);
        ofSetColor(ofxCv::magentaPrint);
        ofCircle(average, 1);
        ofSetColor(ofxCv::yellowPrint);
        ofCircle(center, 1);
        
        // you can also get the area and perimeter using ofPolyline:
        // ofPolyline::getArea() and ofPolyline::getPerimeter()
        double area = kinectTracker->contourFinder.getContourArea(i);
        double length = kinectTracker->contourFinder.getArcLength(i);
        
        // balance is useful for detecting when a shape has an "arm" sticking out
        // if balance.length() is small, the shape is more symmetric: like I, O, X...
        // if balance.length() is large, the shape is less symmetric: like L, P, F...
        ofVec2f balance = ofxCv::toOf(kinectTracker->contourFinder.getBalance(i));
        ofPushMatrix();
        ofTranslate(centroid.x, centroid.y);
        ofScale(5, 5);
        ofLine(0, 0, balance.x, balance.y);
        ofPopMatrix();
        ofSetColor(ofColor::gray);
        ofDrawBitmapString(ofToString(kinectTracker->contourFinder.getLabel(i)), center);
        
        
    }
    ofPopStyle();
    ofPopMatrix();
    sbsOutputServer.publishFBO(&fbo);
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    
	if (key == 'f'){
		ofToggleFullscreen();
	} else if (key == 'g') {
		drawChessboards = !drawChessboards;
	} else if (key == 'm') {
        drawFBOs = !drawFBOs;
	} else if (key == 'w'){
        gui->loadSettings("GUI/wallsetting.xml");
    } else if(key == 'q'){
        gui->loadSettings("GUI/floorsetting.xml");
    } else if (key == 'W'){
        gui->saveSettings("GUI/wallsetting.xml");
    }
    else if(key == 'Q'){
        gui->saveSettings("GUI/floorsetting.xml");
    }
    
}


//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{
    //ofVec3f mousePoint(x - ofGetWidth()/2,y-ofGetHeight()/2, 400);
    //points.push_back(mousePoint);
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{
    
    gui->setScrollArea(0, timeline.getHeight(), 300, ofGetHeight()-timeline.getHeight());
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{
    
}

void testApp::exit() {
    
    gui->saveSettings("GUI/guiSettings.xml");
    delete gui;
    
    for(int i=0; i < planes.size(); i++) {
        planes[i]->exit();
    }
    
    for(int s=0; s<contentScenes.size();s++) {
        contentScenes[s]->exit();
    }
    
    settings.save("stereoplanes.xml");
    cout<<"exit"<<endl;
}


void testApp::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.getName();
	int kind = e.getKind();
	//cout << "got event from: " << name << endl;
    
    /*if(name=="Wall cam") {
        ofxUI2DPad *pad = (ofxUI2DPad *) e.widget;
        camPosWall.x = pad->getScaledValue().x;
        camPosWall.y = pad->getScaledValue().y;
    }*/
    
    for(int i=0; i<contentScenes.size(); i++) {
        contentScenes[i]->guiEvent(e);
    }
}

ofMatrix3x3 testApp::calcNormalMatrix(ofMatrix4x4 matrix){
    ofMatrix3x3 normalMat(matrix._mat[0].x,matrix._mat[0].y,matrix._mat[0].z,
                          matrix._mat[1].x,matrix._mat[1].y,matrix._mat[1].z,
                          matrix._mat[2].x,matrix._mat[2].y,matrix._mat[2].z);
    normalMat.invert();
    normalMat.transpose();
    return normalMat;
}
