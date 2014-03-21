#include "testApp.h"
#include <OpenGL/OpenGL.h>
#include <ofGLUtils.h>

//--------------------------------------------------------------
void testApp::setup()
{
    int resX = 2560;
    int resY = 720;
    
    speed = 0;
    time = 0;

    ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofSetFrameRate(30);
//    ofSetVerticalSync(true);
//    ofSetBackgroundAuto(true);
    ofBackground(0);
    
    //leftOutputServer.setName("Left");
    //rightOutputServer.setName("Right");
    sbsOutputServer.setName("Side By Side");
    
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
    gui->addSlider("Speed", -1, 1, &speed);
    gui->addSlider("Vertex Noise", -1, 1, &vertexNoise);

    gui->addSlider("Dancer X",  -1, 1, &dancerPos.x);
    gui->addSlider("Dancer Y",  -1, 1, &dancerPos.y);

    gui->addToggle("Draw Checkers", &drawChessboards);
    gui->addToggle("Draw Planes", &drawGrids);
    gui->addToggle("Draw FBOs", &drawFBOs);

    for(int i=0; i<contentScenes.size(); i++) {
        gui->addSpacer(width, 3)->setDrawOutline(true);
        contentScenes[i]->setGui(gui, width);
    }
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&testApp::guiEvent);
    
    gui->setDrawBack(false);
    gui->setScrollAreaToScreenHeight();
    
    gui->loadSettings("GUI/guiSettings.xml");
    
    // Set up Lighting

    lightShader.load("shaders/light");
    lightShader.printLayout("Material");
    lightShader.printLayout("Light");
//    lightShader.printLayout("NormalMatrix");
    
    light.ambientIntensity = ofVec4f(.0, .0, .0, 1.0);
    light.numberLights = 2;
    light.lights[0].lightIntensity = ofVec4f(1., .7, .5, 1.0);
    light.lights[0].lightAttenuation = 1/.5;
    light.lights[1].lightIntensity = ofVec4f(.0, 0.3, 0.5, 0.1);
    light.lights[1].lightAttenuation = 1/.95;
    
    lights.push_back(new shaderLight());
    lights.push_back(new shaderLight());
    
    // create Materials
    white.diffuseColor = ofVec4f(0.9, 0.9, 0.9, 1.0);
    white.specularColor = ofVec4f(0.0, 0.0, 0.0, 0.0);
    white.specularShininess = 0.5;

    
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
    
    wall->aspect = aspect;
    
    for(int i=0; i<planes.size(); i++) {
        planes[i]->cam.setPhysicalEyeSeparation(eyeSeperation);
        planes[i]->update();
    }

    for(int s=0; s<contentScenes.size();s++) {
        contentScenes[s]->time = time;
        contentScenes[s]->update();
    }
    
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    time += speed;

}


void testApp::drawScenes(int _surfaceId) {
    for(int s=0; s<contentScenes.size();s++) {
        contentScenes[s]->drawScene(_surfaceId);
        drawFly();
    }
}

void testApp::drawFly(){
    
    float flyTime = time * 0.01;
    
    float zPos =ofSignedNoise(0,0,flyTime);
    float reduction = fmaxf(0,ofMap(zPos, 1, -1, 0.0, 1));
    reduction = pow(reduction, 3);
                              
    ofVec3f pos(
                ofMap(reduction, 0,1,ofSignedNoise(flyTime), camPosWall.x),
                ofMap(reduction, 0,1,ofSignedNoise(0,flyTime), camPosWall.y),
                2.2*zPos
                );
    
    lightShader.end();
    ofDrawSphere(pos, 0.01);
    lightShader.begin();
    lights.front()->setGlobalPosition(pos);
    lights.back()->setGlobalPosition(pos.y,-1,pos.x);
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofSetColor(255);
    ofBackgroundGradient(ofColor::darkGrey, ofColor::gray);
    //drawScenes(0);
    
    ofEnableDepthTest();
    
    // draw scenes to surfaces, they are kept in the cameras fbo

    for(int i=0;i < planes.size(); i++) {
        planes[i]->beginLeft();
        ofClear(ofColor::black);
        glPushMatrix();
        
        // update Lighting
        ofVec3f light1CamSpacePosLeft = lights[0]->getPosition() * ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
        light.lights[0].cameraSpaceLightPos = light1CamSpacePosLeft;
        ofVec3f light2CamSpacePosLeft = lights[1]->getPosition() * ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
        light.lights[1].cameraSpaceLightPos = light2CamSpacePosLeft;
        

        lightShader.begin();
        lightShader.setUniform1f("vertexNoise", vertexNoise);
        
        lightShader.setUniformBuffer("Light", light);
        //draw plane
        lightShader.setUniformBuffer("Material", white);
        //lightShader.setUniformBuffer("NormalMatrix", calcNormalMatrix(ofGetCurrentMatrix(OF_MATRIX_MODELVIEW) * trae->trees.back()->getGlobalTransformMatrix()));
        
        drawScenes(i);
        lightShader.end();
        glPopMatrix();
        planes[i]->endLeft();
        
        planes[i]->beginRight();
        ofClear(ofColor::black);
        glPushMatrix();
        
        // update Lighting
        ofVec3f light1CamSpacePosRight = lights[0]->getPosition() * ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
        light.lights[0].cameraSpaceLightPos = light1CamSpacePosRight;
        ofVec3f light2CamSpacePosRight = lights[1]->getPosition() * ofGetCurrentMatrix(OF_MATRIX_MODELVIEW);
        light.lights[1].cameraSpaceLightPos = light2CamSpacePosRight;
        
        lightShader.begin();
        
        lightShader.setUniformBuffer("Light", light);
        //draw plane
        lightShader.setUniformBuffer("Material", white);
        //lightShader.setUniformBuffer("NormalMatrix",calcNormalMatrix(ofGetCurrentMatrix(OF_MATRIX_MODELVIEW) * trae->trees.back()->getGlobalTransformMatrix()));
        
        drawScenes(i);
        
        lightShader.end();
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
        fbo.draw(300,0,(ofGetWidth()-300),(ofGetWidth()-300)*(fbo.getHeight()*1./fbo.getWidth()));
    }
    
//    ofEnableArbTex();
    
    sbsOutputServer.publishFBO(&fbo);
//    ofTexture t = fbo.getTextureReference();
//    sbsOutputServer.publishTexture(&t);
    
    
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

ofMatrix3x3 testApp::calcNormalMatrix(ofMatrix4x4 matrix){
    ofMatrix3x3 normalMat(matrix._mat[0].x,matrix._mat[0].y,matrix._mat[0].z,
                          matrix._mat[1].x,matrix._mat[1].y,matrix._mat[1].z,
                          matrix._mat[2].x,matrix._mat[2].y,matrix._mat[2].z);
    normalMat.invert();
    normalMat.transpose();
    return normalMat;
}
