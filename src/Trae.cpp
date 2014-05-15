//
//  Trae.cpp
//  Trae
//
//  Created by Ole Kristensen on 15/03/14.
//
//

#include "Trae.h"
#include "testApp.h"
#include <dispatch/dispatch.h>

void Trae::setup() {
    
    name = "Trae Scene";
    oscAddress = "/trae";
    zPos = 0;
    
    noiseTimeElapsed = 0.0;
    
    mainTimeline->addPage(name);
    noiseFront = mainTimeline->addCurves("Noise Front", ofRange(0.0, 4.0));
    noiseBack = mainTimeline->addCurves("Noise Back", ofRange(0.0, 4.0));
    noiseSpeed = mainTimeline->addCurves("Noise Speed", ofRange(0.0, 0.1));
    
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
    
    makeTrees();
    
    noisePoints.numberOfPoints = 10;
    noisePoints.points[0] = ofVec4f(0,0,-1,.25);
    noisePoints.points[1] = ofVec4f(0,0,-1,.25);
    noisePoints.points[2] = ofVec4f(0,0,-1,.25);
    noisePoints.points[3] = ofVec4f(0,0,-1,.25);
    noisePoints.points[4] = ofVec4f(0,0,-1,.25);
    noisePoints.points[5] = ofVec4f(0,0,-1,.25);
    noisePoints.points[6] = ofVec4f(0,0,-9,.25);
    noisePoints.points[7] = ofVec4f(0,0,-9,.25);
    noisePoints.points[8] = ofVec4f(0,0,-9,.25);
    noisePoints.points[9] = ofVec4f(0,0,-9,.25);
    
}

void Trae::draw(int _surfaceId) {
    cam.enableMouseInput();

    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        ofPushMatrix();
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);
    
        // +z points forward
        
        ofVec3f camPos = cam.getPosition();
        ofTranslate(cam.getPosition());
        
        for (int i = 0; i < noisePoints.numberOfPoints; i++) {
            
            ofVec4f position;
            
            if (i%2 == 0) {
                position = ofVec4f(-0.66,1.0,-3.0, noiseFront->getValue());
            } else {
                position = ofVec4f(0,2.5,-8.0, noiseBack->getValue());
            }
            position += ofVec4f(ofSignedNoise(noiseTimeElapsed, i)/2.0, ofSignedNoise(0, noiseTimeElapsed, i)/2.0, ofSignedNoise(0,0, noiseTimeElapsed, i)/2.0, 0.0);

            noisePoints.points[i] = position;
        }
        
/*        ofxOlaShaderLight::shader->setUniform1f("time", ofGetElapsedTimef());*/

        ofxOlaShaderLight::setNoisePoints(noisePoints);

        int i = 0;
        for (std::vector<ofxProcTree*>::iterator it = trees.begin() ; it != trees.end(); ++it) {
            ofPushMatrix();
            ofTranslate(0, 0, 1);
            ofRotateX(-180);
            ofTranslate(0,-1,0);
            ofQuaternion rot = cam.getOrientationQuat();
            ofRotate(rot.w(), rot.x(), rot.y(), rot.z());
            treeNodes[i]->transformGL();
            ofxProcTree *t = *(it);
            ofxOlaShaderLight::setMaterial(treeNodes[i]->material);
            t->mesh.draw();
            //ofxOlaShaderLight::setMaterial(treeMaterial);
            //t->drawFoilage();
            ofPopMatrix();
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
    
    if (regrow) {
        makeTrees();
        regrow = false;
    }
    
    if(addCameraKeyFrame){
        cameraTrack->addKeyframe();
        addCameraKeyFrame = false;
    }
    
    noiseTimeElapsed += noiseSpeed->getValue();

}

void Trae::makeTrees(){
    
    int numberTrees = 40;
    
    for (std::vector<ofxProcTree*>::iterator it = trees.begin() ; it != trees.end(); ++it) {
        ofxProcTree *t = *(it);
        delete t;
    }

    trees.clear();

    for (std::vector<TreeNode*>::iterator it = treeNodes.begin() ; it != treeNodes.end(); ++it) {
        TreeNode *t = *(it);
        delete t;
    }
    
    treeNodes.clear();
    
    for (int i = 0; i < numberTrees; i++) {
        TreeNode* treeNode = new TreeNode();
        
        ofVec3f pos;
        
        pos.y = 0.0;
        pos.z = ofRandom(-8.0,0.0);
        pos.x = ofRandom(-1.0,1.0);
        pos.x *= fabs(pos.z-1.0);
        
        treeNode->material.diffuseColor = ofVec4f(1.0, 1.0, 1.0, 1.0);
        treeNode->material.specularShininess = 0.5;
        treeNode->setScale(1.0);
        treeNode->size = powf(ofMap(pos.z, 0.0, -8.0, 0.5, 1.0),1.1) * ofRandom(1.0);
        treeNode->setGlobalPosition(pos.x, pos.y, pos.z);

        // pre overrides

        if(i==0){
            // left black
            treeNode->setGlobalPosition(-(16.0/9.0)/((testApp*)ofGetAppPtr())->aspect*.85, 0.0, 1.45);
            treeNode->size = 0.9;
            treeNode->material.diffuseColor = ofVec4f(0.0, 0.0, 0.0, 1.0);
            treeNode->material.specularColor = ofVec4f(0.0, 0.0, 0.0, 1.0);
        }
        if(i==1){
            // right black
            treeNode->setGlobalPosition((16.0/9.0)/((testApp*)ofGetAppPtr())->aspect*.85, 0.0, 1.45);
            treeNode->size = 0.8;
            treeNode->material.diffuseColor = ofVec4f(0.0, 0.0, 0.0, 1.0);
            treeNode->material.specularColor = ofVec4f(0.0, 0.0, 0.0, 1.0);
        }
        if(i==2){
            // frontmost
            treeNode->setGlobalPosition((16.0/9.0)/((testApp*)ofGetAppPtr())->aspect*.4, 0.0, 0.5);
            treeNode->size = 0.2;
        }
        
        float sizeFactor = treeNode->size;
        
        treeNode->properties.seed = random();
        treeNode->properties.segments = 6; // radial resolution of branches
        treeNode->properties.levels = round(ofMap(sizeFactor, 0.0, 1.0, 2,7)); // branching level depth
        treeNode->properties.vMultiplier = 1.01; // texture v multiplier
        treeNode->properties.twigScale = 0;
        treeNode->properties.initalBranchLength = ofMap(sizeFactor, 0.0, 1.0, 0.03,0.61);
        treeNode->properties.lengthFalloffFactor = ofMap(sizeFactor, 0.0, 1.0, 0.9, 0.85);
        treeNode->properties.lengthFalloffPower = ofMap(sizeFactor, 0.0, 1.0, 0.8, 0.94);
        treeNode->properties.clumpMax = 0.423;
        treeNode->properties.clumpMin = 0.125;
        treeNode->properties.branchFactor = 2.0; // Symmetry
        treeNode->properties.dropAmount = ofMap(sizeFactor, 0.0, 1.0, -0.01,-0.14);
        treeNode->properties.growAmount = ofMap(sizeFactor, 0.0, 1.0, 0.8, 1.0);
        treeNode->properties.sweepAmount = ofRandom(-0.001,0.001);
        treeNode->properties.maxRadius = ofMap(sizeFactor, 0, 1, 0.0075,0.168);
        treeNode->properties.trunkLength = ofMap(sizeFactor, 0, 1, 0.15,1.15);
        treeNode->properties.climbRate = ofMap(sizeFactor, 0,1, 0.1, 0.508);
        treeNode->properties.trunkKink = ofMap(sizeFactor, 0,1, 0.01, 0.093);
        treeNode->properties.treeSteps = ofMap(sizeFactor, 0, 1, 2, 6); // trunk forks
        treeNode->properties.taperRate = ofMap(sizeFactor, 0, 1, 0.9,0.929);
        treeNode->properties.radiusFalloffRate = 0.71;
        treeNode->properties.twistRate = ofMap(sizeFactor, 0, 1, 1.0,ofRandom(10));
        
        // post overrides
        if(i==0){
            // left black
            treeNode->properties.initalBranchLength *= 0.75;
            treeNode->properties.levels = 2;
            treeNode->properties.dropAmount = 0.2;

        }
        if(i==1){
            // right black
            treeNode->properties.initalBranchLength *= 0.75;
            treeNode->properties.levels = 2;
            treeNode->properties.dropAmount = 0.2;
        }
        
        treeNodes.push_back(treeNode);
        trees.push_back(NULL);
        
    }
    
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0);
    
    dispatch_apply(numberTrees, queue, ^(size_t i) {

        ofxProcTreeBranch::Properties * p = &(treeNodes[i]->properties);
        
        trees[i] = new ofxProcTree(p);
    });
}

void Trae::setGui(ofxUICanvas * gui, float width){
    ContentScene::setGui(gui, width);
    gui->addLabelToggle("Lock Camera to Track",  &cameraTrack->lockCameraToTrack);
    gui->addLabelButton("Add Camera Keyframe", &addCameraKeyFrame);
    gui->addLabelButton("Regrow", &regrow);
}

void Trae::receiveOsc(ofxOscMessage * m, string rest) {
}

void Trae::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.getName();
	int kind = e.getKind();
    
	//cout << "got event from: " << name << endl;
    
}