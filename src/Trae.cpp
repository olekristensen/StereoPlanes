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
    
}

void Trae::draw(int _surfaceId) {
    
    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        
        ofPushMatrix();
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        
/*      FLOOR AND AXIS
        ofPushMatrix();
        ofRotateX(90);
        ofTranslate(0,0,-1);
        ofSetColor(255, 255, 127);
        ofDrawPlane(2, 2);
        ofDrawAxis(1);
        ofPopMatrix();
*/
        ofTranslate(ofPoint(0.,0,zPos));

        //ofRotateY(time);
        
        ofSetColor(255,255,255);
        
        int i = 0;
        for (std::vector<ofxProcTree*>::iterator it = trees.begin() ; it != trees.end(); ++it) {
            ofPushMatrix();
            ofRotateY(360.0*i/trees.size());
            ofTranslate(0, 0, 1);
            ofRotateX(-180);
            ofTranslate(0,-1,0);
            ofScale(1./3, 1./3, 1./3);
            ofxProcTree *t = *(it);
            t->mesh.draw();
            ofPopMatrix();
            i++;
        }
        //tree->drawSkeleton();
        ofPopMatrix();
        glDisable(GL_CULL_FACE);
    }
    
}

void Trae::update() {
    if (regrow->isOn() & !hasRegrown) {
        makeTrees();
        hasRegrown = true;
    }
    if (!regrow->isOn()){
        hasRegrown = false;
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
    gui->addSlider("Z pos", -2, 3, &zPos);
}

void Trae::receiveOsc(ofxOscMessage * m, string rest) {
}

void Trae::guiEvent(ofxUIEventArgs &e)
{
    
    string name = e.getName();
	int kind = e.getKind();
	//cout << "got event from: " << name << endl;
    
}