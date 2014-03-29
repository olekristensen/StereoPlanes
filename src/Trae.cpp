//
//  Trae.cpp
//  Trae
//
//  Created by Ole Kristensen on 15/03/14.
//
//

#include "Trae.h"

void Trae::setup() {
    
    name = "Trae Scene";
    oscAddress = "/trae";
    zPos = 0;
    
    mainTimeline->addPage(name);
    regrow = mainTimeline->addSwitches("Regrow");
    progress = mainTimeline->addCurves("Progress");

    treeColor = mainTimeline->addColors("Trees");
    groundColor = mainTimeline->addColors("Ground");

    groundMaterial.diffuseColor = ofVec4f(0.9, 0.75, 0.5, 1.0);
    groundMaterial.specularColor = ofVec4f(0.0, 0.0, 0.0, 1.0);
    groundMaterial.specularShininess = 0.5;

    treeMaterial.diffuseColor = ofVec4f(1.0, 1.0, 1.0, 1.0);
    treeMaterial.specularColor = ofVec4f(0.0, 0.0, 0.0, 1.0);
    treeMaterial.specularShininess = 0.5;
    
    cam.setTranslationKey('-');
    cam.setScale(2,2,2);
    cam.setPosition(0, 0, 0);
    cam.setOrientation(ofVec3f(0,0,0));

    cameraTrack = new ofxTLCameraTrack();
    cameraTrack->setDampening(1);
	cameraTrack->setCamera(cam);
    cameraTrack->setXMLFileName("Tree_Camera.xml");
	mainTimeline->addTrack("Camera", cameraTrack);
	
//	cameraTrack->lockCameraToTrack = true;

    for(int i = 0; i < 10000; i++){
		particles.addVertex(ofVec3f(ofRandom(-2000,2000),
									ofRandom(-2000,2000),
									ofRandom(-2000,2000)));
        
		particles.addColor(ofFloatColor(ofRandomuf()*.4));
	}

}

void Trae::draw(int _surfaceId) {
    cam.enableMouseInput();

    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        ofPushMatrix();
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);
    
        ofVec3f camPos = cam.getPosition();
        ofTranslate(cam.getPosition());
     
        ofxOlaShaderLight::setMaterial(treeMaterial);

        int i = 0;
        for (std::vector<ofxProcTree*>::iterator it = trees.begin() ; it != trees.end(); ++it) {
            ofPushMatrix();
            ofRotateY(360.0*i/trees.size());
            ofTranslate(0, 0, 1);
            ofRotateX(-180);
            ofTranslate(0,-1,0);
            ofQuaternion rot = cam.getOrientationQuat();
            ofRotate(rot.w(), rot.x(), rot.y(), rot.z());
            ofScale(1./3, 1./3, 1./3);
            ofxProcTree *t = *(it);
            t->mesh.draw();
            ofPopMatrix();
            i++;
        }
        //tree->drawSkeleton();
        //glDisable(GL_CULL_FACE);

        ofxOlaShaderLight::setMaterial(groundMaterial);
        //ofDrawBox(0, 1, 0, 2, 0.001, 2);

        ofPopMatrix();
    }
}

void Trae::update() {
    
    groundMaterial.diffuseColor = ofVec4f(
                                          groundColor->getColor().r/255.,
                                          groundColor->getColor().g/255.,
                                          groundColor->getColor().b/255.,
                                          1.0
    );

    treeMaterial.diffuseColor = ofVec4f(
                                          treeColor->getColor().r/255.,
                                          treeColor->getColor().g/255.,
                                          treeColor->getColor().b/255.,
                                          1.0
                                          );
    
    if (regrow->isOn() & !hasRegrown) {
        makeTrees();
        hasRegrown = true;
    }
    if (!regrow->isOn()){
        hasRegrown = false;
    }
    
    if(addCameraKeyFrame){
        cameraTrack->addKeyframe();
        addCameraKeyFrame = false;
    }
}

void Trae::makeTrees(){
    
    for (std::vector<ofxProcTree*>::iterator it = trees.begin() ; it != trees.end(); ++it) {
        ofxProcTree *t = *(it);
        delete t;
    }
    trees.clear();
    for (int i = 0; i < 1; i++) {
        
        ofxProcTreeBranch::Properties * p = new ofxProcTreeBranch::Properties();
        
        p->seed = 519+i;
        p->segments = 14+ofRandom(-3,1);
        p->levels = 5;
        p->vMultiplier = 1.01;
        p->twigScale = 0;
        p->initalBranchLength = 0.65+ofRandom(-.1,.5);
        p->lengthFalloffFactor = 0.73;
        p->lengthFalloffPower = 0.76;
        p->clumpMax = 0.53;
        p->clumpMin = 0.419;
        p->branchFactor = 3.4;// + ofRandom(-1,1);
        p->dropAmount = -0.16;
        p->growAmount = 0.419;//+ofRandom(-1,1);
        p->sweepAmount = 0.01;
        p->maxRadius = 0.168 + ofRandom(-0.1,0);
        p->climbRate = 0.472;
        p->trunkKink = 0.06;
        p->treeSteps = 5;
        p->taperRate = 0.835;
        p->radiusFalloffRate = 0.73;
        p->twistRate = 1.29;
        p->trunkLength = 2.2+ofRandom(-1,0);
        
        trees.push_back(new ofxProcTree(p));
    }
}

void Trae::setGui(ofxUICanvas * gui, float width){
    ContentScene::setGui(gui, width);
    gui->addLabelToggle("Lock Camera to Track",  &cameraTrack->lockCameraToTrack);
    gui->addLabelButton("Add Camera Keyframe", &addCameraKeyFrame);
}

void Trae::receiveOsc(ofxOscMessage * m, string rest) {
}

void Trae::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.getName();
	int kind = e.getKind();
    
	//cout << "got event from: " << name << endl;
    
}