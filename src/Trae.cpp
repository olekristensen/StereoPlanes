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
    
    makeTrees();
    
    ofMesh branchCylinder;
    branchCylinder.cylinder(0.1, 4);
    branchVbo.setMesh(branchCylinder, GL_STATIC_DRAW);
    
    branchVboMesh = ofVboMesh(ofCylinderPrimitive(0.01, 1, 4, 4).getMesh());

    
}

void Trae::drawVboBranches(Branch * branch) {
    
    ofPushMatrix();
    ofTranslate(branch->getGlobalPosition());
    
    //ofMatrix4x4()
    //ofGetCurrentMatrix(OF_MATRIX_MODELVIEW).setRotate(branch->getGlobalOrientation());
    
    //branch->getGlobalOrientation().w()
    
    ofVec3f qaxis; float qangle;
    branch->getGlobalOrientation().getRotate(qangle, qaxis);
    ofRotate(qangle, qaxis.x, qaxis.y, qaxis.z);
    
    //ofRotateX(branch->getGlobalTransformMatrix().x());
    //ofRotateY(branch->getGlobalOrientation().y()*360);
    //ofRotateZ(branch->getGlobalOrientation().z()*360);
    
    //branch->getGlobalTransformMatrix().
    
    ofScale(branch->getScale().x, branch->getHeight(), branch->getScale().z);
    //branchVbo.draw(OF_PRIMITIVE_TRIANGLE_STRIP, 0, branchVbo.getNumVertices());
    
    ofPushMatrix();
    ofScale(branch->getRadius()*100,1,branch->getRadius()*100);
    branchVboMesh.draw();
    ofPopMatrix();
    //ofDrawBox(0, 0, 0, 0.1, 0.1, 0.1);
    
    ofPopMatrix();
    
    for (std::vector<Branch*>::iterator it = branch->branches.begin() ; it != branch->branches.end(); ++it) {
        Branch *b = *(it);
        drawVboBranches(b);
    }
    
}


void Trae::draw(int _surfaceId) {
    
    // A scene can draw to multiple surfaces
    if(_surfaceId == 0) {
        
        ofPushMatrix();
        
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

        ofRotateY(time*0.8);
        
        ofSetColor(255,255,255);
        
        for (std::vector<Branch*>::iterator it = trees.begin() ; it != trees.end(); ++it) {
            Branch *b = *(it);
            
            drawVboBranches(b);
            //b->drawFull();
            /* draw vectors
            ofPushStyle();
            ofDisableDepthTest();
            ofSetColor(0, 255, 255, 127);
            cout << b->drawVecTree() << endl;
            ofEnableDepthTest();
            ofPopStyle();
             */
        }

        ofPopMatrix();
        
    }
    
}

void Trae::update() {
    if (regrow & !hasRegrown) {
        makeTrees();
        hasRegrown = true;
    }
    if (!regrow){
        hasRegrown = false;
    }
}

void Trae::makeTrees(){
    
    for (std::vector<Branch*>::iterator it = trees.begin() ; it != trees.end(); ++it) {
        Branch *b = *(it);
        delete b;
    }
    trees.clear();
    for (int i = 0; i < 1; i++) {
        Branch * b = new Branch();
        float bWidth = ofRandom(0.03,0.075);
        float bHeight = ofRandom(0.5,0.75);
        b->set(bWidth,bHeight);
        //b->setPosition(ofRandom(-.5,.5),1-(bHeight*.5),ofRandom(-.5,.5));
        b->setPosition(0,1-(bHeight*.5),0);
        b->setResolutionRadius(5);
        b->setResolutionHeight(5);
        b->make(8);
        trees.push_back(b);
    }
}

void Trae::setGui(ofxUICanvas * gui, float width){
    ContentScene::setGui(gui, width);
    gui->addButton("Regrow", &regrow);
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