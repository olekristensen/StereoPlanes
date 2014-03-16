#include "testApp.h"
#include <OpenGL/OpenGL.h>
#include <ofGLUtils.h>

// Syphon together with 3D primitive and NoFill does not run

vector<ofVec3f> points;

//--------------------------------------------------------------
void testApp::setup()
{
    int resX = 2560;
    int resY = 720;
    
    ofSetLogLevel(OF_LOG_ERROR);
    ofDisableArbTex();

    ofSetFrameRate(30);
//    ofSetVerticalSync(true);
//    ofSetBackgroundAuto(true);
    ofBackground(0);
    
    //leftOutputServer.setName("Left");
    //rightOutputServer.setName("Right");
    sbsOutputServer.setName("Side By Side");
    
    fbo.allocate(resX, resY);
    
    fbo.begin();
    ofClear(0,0,0,0);
    fbo.end();
    
    settings.load("stereoplanes.xml");
    
    wall = new StereoPlane("wall");
    wall->setup(resX/2, resY, &settings);
    wall->pos = ofVec2f(0,0);
    planes.push_back(wall);
    
    activePlaneIndex = 0;
    activePlane = planes[activePlaneIndex];
    
    oscReceiver.setup(9001);
    
    camPosWall = ofVec3f(0, 0, -1);
    
    trae = new Trae();
    contentScenes.push_back(trae);
    
    for(int i=0; i<contentScenes.size(); i++) {
        contentScenes[i]->setupScene(i);
    }
    
    // add all parameterGroups from scenes to parameters
    // gui.setup(parameters);
    
    float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float width = 300-xInit;
    hideGUI = false;
    
    gui = new ofxUIScrollableCanvas(0, 0, width+xInit, ofGetHeight());
    
    gui->setScrollableDirections(false, true);
    
    gui->setFont("GUI/Arial.ttf");
    gui->setWidgetFontSize(OFX_UI_FONT_SMALL);
    gui->setColorBack(ofColor(10, 10, 10,220));
    
    gui->addLabel("trae", OFX_UI_FONT_LARGE);
    
    gui->addSlider("Eye seperation", 0, 7, &eyeSeperation);
    
    gui->addSlider("Wall X",  -2, 2, &camPosWall.x);
    gui->addSlider("Wall Y",  -1, 1, &camPosWall.y);
    gui->addSlider("Wall Z",  -4, -0.25, &camPosWall.z);

    gui->addSlider("Aspect",  0.0, 2.0, &aspect);
    
    gui->addSlider("Dancer X",  -1, 1, &dancerPos.x);
    gui->addSlider("Dancer Y",  -1, 1, &dancerPos.y);
    
    for(int i=0; i<contentScenes.size(); i++) {
        gui->addSpacer(width, 3)->setDrawOutline(true);
        contentScenes[i]->setGui(gui, width);
    }
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    
    gui->setDrawBack(false);
    gui->setScrollAreaToScreenHeight();
    
    gui->loadSettings("GUI/guiSettings.xml");
    
}


//--------------------------------------------------------------
void testApp::update()
{
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
    
    for(int i=0; i<planes.size(); i++) {
        planes[i]->cam.setPhysicalEyeSeparation(eyeSeperation);
        planes[i]->update();
    }

    for(int s=0; s<contentScenes.size();s++) {
        contentScenes[s]->update();
    }
    
    gui->setVisible(!hideGUI);
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
}


void testApp::drawScenes(int _surfaceId) {
    for(int s=0; s<contentScenes.size();s++) {
        contentScenes[s]->drawScene(_surfaceId);
    }
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetColor(255);
    ofBackgroundGradient(ofColor::darkGrey, ofColor::gray);
    //drawScenes(0);
    
    ofEnableLighting();
    ofEnableDepthTest();
    
    // draw scenes to surfaces, they are kept in the cameras fbo

    for(int i=0;i < planes.size(); i++) {
        planes[i]->beginLeft();
        ofClear(ofColor::black);
        glPushMatrix();
        glScalef(aspect*planes[i]->height*1.0/planes[i]->width, 1.0, 1.0);
        drawScenes(i);
        glPopMatrix();
        planes[i]->endLeft();
        
        planes[i]->beginRight();
        ofClear(ofColor::black);
        glPushMatrix();
        glScalef(aspect*planes[i]->height*1.0/planes[i]->width, 1.0, 1.0);
        drawScenes(i);
        glPopMatrix();
        planes[i]->endRight();
    }

    ofDisableDepthTest();
    ofDisableLighting();
    
    if(showGrid) {
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
    
    if(!hideMonitor) {
        ofSetColor(255,255);
        fbo.draw(300,0,(ofGetWidth()-300),(ofGetWidth()-300)*(fbo.getHeight()*1./fbo.getWidth()));
    }
    
    ofDisableArbTex();
    ofTexture t = fbo.getTextureReference();
    sbsOutputServer.publishTexture(&t);
    
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{
    
	if (key == 'f'){
		ofToggleFullscreen();
	} else if (key == 'g') {
		showGrid = !showGrid;
	} else if (key == 'd') {
        hideGUI = !hideGUI;
    } else if (key == 'm') {
        hideMonitor = !hideMonitor;
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
    
    gui->setScrollAreaToScreenHeight();
    
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