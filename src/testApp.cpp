#include "testApp.h"
#include <OpenGL/OpenGL.h>
#include <ofGLUtils.h>


//--------------------------------------------------------------
void testApp::setup()
{
    
    ofSetVerticalSync(false); // can cause problems on some Linux implementations
    
    ofDisableArbTex();
    setupScreenQuad();
    
    m_shader.load("shaders/mainScene.vert", "shaders/mainScene.frag");
    m_pointLightPassShader.load("shaders/pointLightPass.vert", "shaders/pointLightPass.frag");
    m_pointLightStencilShader.load("shaders/pointLightStencil.vert", "shaders/pointLightStencil.frag");
    
    m_texture.loadImage("textures/concrete.jpg");
    
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
    
    

    m_windowWidth  = resX/2;
    m_windowHeight = resY;
    
    m_gBuffer.setup( m_windowWidth, m_windowHeight);
    
    //m_gBufferRight.setup( m_windowWidth, m_windowHeight);
    
    m_ssaoPassLeft.setup(m_windowWidth, m_windowHeight);
    m_ssaoPassRight.setup(m_windowWidth, m_windowHeight);
    
    // set our camera parameters for ssao pass - inverse proj matrix + far clip are used in shader to recreate position from linear depth
    //ofGetCurrentMatrix();
    m_ssaoPassLeft.setCameraProperties(wall->cam.left.getProjectionMatrix().getInverse(),  wall->cam.left.getFarClip());
    m_ssaoPassRight.setCameraProperties(wall->cam.right.getProjectionMatrix().getInverse(), wall->cam.right.getFarClip());
    
    bindGBufferTextures(); // bind them once to upper texture units - faster than binding/unbinding every frame
    
    setupLights();
    createRandomBoxes();
    
    ofMesh boxMesh = Primitives::getBoxMesh(1.0f, 1.0f, 1.0f);
    m_numBoxVerts = boxMesh.getNumVertices();
    m_boxVbo.setMesh(boxMesh, GL_STATIC_DRAW);
    
    ofMesh sphereMesh = Primitives::getSphereMesh(6);
    m_numSphereVerts = sphereMesh.getVertices().size();
    m_sphereVbo.setMesh(sphereMesh, GL_STATIC_DRAW);
    
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
    light.lights[0].lightAttenuation = 1/.25;
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
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        contentScenes[s]->drawScene(_surfaceId);
        drawFly();
        glDisable(GL_CULL_FACE);
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
        
        geometryPass();
        pointLightPass();
        
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
    //resizeBuffersAndTextures();
    
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

void testApp::createRandomBoxes() {
    // create randomly rotated boxes
    for (unsigned int i=0; i<skNumBoxes; i++) {
        float x = 0;
        float y = 0;
        float z = 0;
        float size = skRadius;
        float angle = ofRandom(0.0f, 90.0f);
        ofVec3f axis = ofVec3f(ofRandom(0.0f, 1.0f), ofRandom(0.0f, 1.0f), ofRandom(0.0f, 1.0f));
        axis.normalize();
        
        m_boxes.push_back(Box(ofVec3f(x, y, z), angle, axis.x, axis.y, axis.z, size));
    }
}

void testApp::setupLights() {
    for (unsigned int i=0; i<skNumLights; i++) {
        addRandomLight();
    }
}

void testApp::addRandomLight() {
    // create a random light that is positioned on bounding sphere of scene (skRadius)
    PointLight l;
    ofVec3f posOnSphere = ofVec3f(ofRandom(-1.0f, 1.0f), ofRandom(-1.0f, 1.0f), ofRandom(-1.0f, 1.0f));
    posOnSphere.normalize();
    posOnSphere.scale(ofRandom(0.95f, 1.05f));
    
    ofVec3f orbitAxis = ofVec3f(ofRandom(0.0f, 1.0f), ofRandom(0.0f, 1.0f), ofRandom(0.0f, 1.0f));
    orbitAxis.normalize();
    l.orbitAxis = orbitAxis;
    
    posOnSphere.scale(skRadius-1);
    
    l.setPosition(posOnSphere);
    l.setAmbient(0.0f, 0.0f, 0.0f);
    
    ofVec3f col = ofVec3f(ofRandom(0.3f, 0.5f), ofRandom(0.2f, 0.4f), ofRandom(0.7f, 1.0f));
    l.setDiffuse(col.x, col.y, col.z);
    l.setSpecular(col.x, col.y, col.z);
    l.setAttenuation(0.0f, 0.0f, 0.2f); // set constant, linear, and exponential attenuation
    l.intensity = 0.8f;
    
    m_lights.push_back(l);
}

void testApp::randomizeLightColors() {
    for (vector<PointLight>::iterator it = m_lights.begin(); it != m_lights.end(); it++) {
        ofVec3f col = ofVec3f(ofRandom(0.4f, 1.0f), ofRandom(0.1f, 1.0f), ofRandom(0.3f, 1.0f));
        it->setDiffuse(col.x, col.y, col.z);
    }
}

void testApp::bindGBufferTextures() {
    // set up the texture units we want to use - we're using them every frame, so we'll leave them bound to these units to save speed vs. binding/unbinding
    m_textureUnits[TEX_UNIT_ALBEDO] = 15;
    m_textureUnits[TEX_UNIT_NORMALS_DEPTH] = 14;
    m_textureUnits[TEX_UNIT_SSAO] = 13;
    m_textureUnits[TEX_UNIT_POINTLIGHT_PASS] = 12;
    
    // bind all GBuffer textures
    glActiveTexture(GL_TEXTURE0 + m_textureUnits[TEX_UNIT_ALBEDO]);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(GBuffer::GBUFFER_TEXTURE_TYPE_ALBEDO));
    
    glActiveTexture(GL_TEXTURE0 + m_textureUnits[TEX_UNIT_NORMALS_DEPTH]);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(GBuffer::GBUFFER_TEXTURE_TYPE_NORMALS_DEPTH));
    
    // bind SSAO texture
    glActiveTexture(GL_TEXTURE0 + m_textureUnits[TEX_UNIT_SSAO]);
    glBindTexture(GL_TEXTURE_2D, m_ssaoPassLeft.getTextureReference());
    
    // point light pass texture
    glActiveTexture(GL_TEXTURE0 + m_textureUnits[TEX_UNIT_POINTLIGHT_PASS]);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getTexture(GBuffer::GBUFFER_TEXTURE_TYPE_LIGHT_PASS));
    
    m_shader.begin();  // our final deferred scene shader
    m_shader.setUniform1i("u_albedoTex", m_textureUnits[TEX_UNIT_ALBEDO]);
    m_shader.setUniform1i("u_ssaoTex", m_textureUnits[TEX_UNIT_SSAO]);
    m_shader.setUniform1i("u_pointLightPassTex", m_textureUnits[TEX_UNIT_POINTLIGHT_PASS]);
    m_shader.end();
    
    m_pointLightPassShader.begin();  // our point light pass shader
    m_pointLightPassShader.setUniform1i("u_albedoTex", m_textureUnits[TEX_UNIT_ALBEDO]);
    m_pointLightPassShader.setUniform1i("u_normalAndDepthTex", m_textureUnits[TEX_UNIT_NORMALS_DEPTH]);
    m_pointLightPassShader.setUniform1i("u_ssaoTex", m_textureUnits[TEX_UNIT_SSAO]);
    m_pointLightPassShader.setUniform2f("u_inverseScreenSize", 1.0f/m_windowWidth, 1.0f/m_windowHeight);
    m_pointLightPassShader.end();
    
    glActiveTexture(GL_TEXTURE0);
}

void testApp::unbindGBufferTextures() {
    // unbind textures and reset active texture back to zero (OF expects it at 0 - things like ofDrawBitmapString() will break otherwise)
    glActiveTexture(GL_TEXTURE0 + m_textureUnits[TEX_UNIT_ALBEDO]); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + m_textureUnits[TEX_UNIT_NORMALS_DEPTH]); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0 + m_textureUnits[TEX_UNIT_SSAO]); glBindTexture(GL_TEXTURE_2D, 0);
    
    glActiveTexture(GL_TEXTURE0);
}

void testApp::geometryPass() {
    
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // CREATE GBUFFER
    // --------------
    // start our GBuffer for writing (pass in near and far so that we can create linear depth values in that range)
    m_gBuffer.bindForGeomPass(wall->cam.left.getNearClip(), wall->cam.left.getFarClip());
    
    wall->cam.left.begin();
    
    glActiveTexture(GL_TEXTURE0); // bind concrete texture
    m_texture.getTextureReference().bind();
    m_boxVbo.bind();
    
    // draw our randomly rotate boxes
    for (vector<Box>::iterator it=m_boxes.begin() ; it < m_boxes.end(); it++) {
        ofPushMatrix();
        ofRotate(it->angle, it->axis_x, it->axis_y, it->axis_z);
        ofScale(it->size, it->size, it->size);
        glDrawArrays(GL_TRIANGLES, 0, m_numBoxVerts);
        ofPopMatrix();
    }
    
    m_boxVbo.unbind();
    
    // draw all of our light spheres so we can see where our lights are at
    m_sphereVbo.bind();
    
    for (vector<PointLight>::iterator it = m_lights.begin(); it != m_lights.end(); it++) {
        ofPushMatrix();
        ofTranslate(it->getGlobalPosition());
        ofScale(0.25f, 0.25f, 0.25f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numSphereVerts);
        ofPopMatrix();
    }
    
    m_sphereVbo.unbind();
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    
    m_cam.end();
    
    m_gBuffer.unbindForGeomPass(); // done rendering out to our GBuffer
}

void testApp::pointLightStencilPass() {
}

void testApp::pointLightPass() {
    
    m_gBuffer.resetLightPass();
    m_pointLightPassShader.begin();
    // pass in lighting info
    int numLights = m_lights.size();
    m_pointLightPassShader.setUniform1i("u_numLights", numLights);
    m_pointLightPassShader.setUniform1f("u_farDistance", wall->cam.left.getFarClip());
    m_pointLightPassShader.end();
    
    wall->cam.left.begin();
    
    ofMatrix4x4 camModelViewMatrix = wall->cam.left.getModelViewMatrix(); // need to multiply light positions by camera's modelview matrix to transform them from world space to view space (reason for this is our normals and positions in the GBuffer are in view space so we must do our lighting calculations in the same space). It's faster to do it here on CPU vs. in shader
    
    m_sphereVbo.bind();
    
    for (vector<PointLight>::iterator it = m_lights.begin(); it != m_lights.end(); it++) {
        ofVec3f lightPos = it->getPosition();
        float radius = it->intensity * skMaxPointLightRadius;
        
        // STENCIL
        // this pass creates a stencil so that only the affected pixels are shaded - prevents us shading things that are outside our light volume
        m_gBuffer.bindForStencilPass();
        m_pointLightStencilShader.begin();
        ofPushMatrix();
        ofTranslate(lightPos);
        ofScale(radius, radius, radius);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numSphereVerts);
        ofPopMatrix();
        m_pointLightStencilShader.end();
        
        // SHADING/LIGHTING CALCULATION
        // this pass draws the spheres representing the area of influence each light has
        // in a fragment shader, only the pixels that are affected by the drawn geometry are processed
        // drawing light volumes (spheres for point lights) ensures that we're only running light calculations on
        // the areas that the spheres affect
        m_gBuffer.bindForLightPass();
        m_pointLightPassShader.begin();
        ofVec3f lightPosInViewSpace = it->getPosition() * camModelViewMatrix;
        
        m_pointLightPassShader.setUniform3fv("u_lightPosition", &lightPosInViewSpace.getPtr()[0]);
        m_pointLightPassShader.setUniform4fv("u_lightAmbient", it->ambient);
        m_pointLightPassShader.setUniform4fv("u_lightDiffuse", it->diffuse);
        m_pointLightPassShader.setUniform4fv("u_lightSpecular", it->specular);
        m_pointLightPassShader.setUniform1f("u_lightIntensity", it->intensity);
        m_pointLightPassShader.setUniform3fv("u_lightAttenuation", it->attenuation);
        
        m_pointLightPassShader.setUniform1f("u_lightRadius", radius);
        
        ofPushMatrix();
        ofTranslate(lightPos);
        ofScale(radius, radius, radius);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, m_numSphereVerts);
        ofPopMatrix();
        
        m_pointLightPassShader.end();
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_BLEND);
        glCullFace(GL_BACK);
    }
    
    m_sphereVbo.unbind();
    
    m_gBuffer.unbind();
    wall->cam.left.end();
}

void testApp::deferredRender() {
    
    // final deferred shading pass
    glDisable(GL_DEPTH_TEST);
    ofDisableLighting();
    
    m_shader.begin();
    drawScreenQuad();
    m_shader.end();
    
    glActiveTexture(GL_TEXTURE0);
}


void testApp::setupScreenQuad() {
    ofVec2f quadVerts[] = {
        ofVec2f(-1.0f, -1.0f),
        ofVec2f(1.0f, -1.0f),
        ofVec2f(1.0f, 1.0f),
        ofVec2f(-1.0f, 1.0f)
    };
    
    ofVec2f quadTexCoords[] = {
        ofVec2f(0.0f, 0.0f),
        ofVec2f(1.0f, 0.0f),
        ofVec2f(1.0f, 1.0f),
        ofVec2f(0.0f, 1.0f)
    };
    
    // full viewport quad vbo
    m_quadVbo.setVertexData(&quadVerts[0], 4, GL_STATIC_DRAW);
    m_quadVbo.setTexCoordData(&quadTexCoords[0], 4, GL_STATIC_DRAW);
}

