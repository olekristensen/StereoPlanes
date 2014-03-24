//
//  Trae.h
//  Trae
//
//  Created by Ole Kristensen on 15/03/14.
//
//

#pragma once
#include "ContentScene.h"
#include "ofMain.h"

class Branch : public ofCylinderPrimitive {

    void addBranch(int steps){
        Branch * b = new Branch();
        b->setParent(*this);
        b->set(this->getRadius()*.65,this->getHeight()*ofRandom(0.5,ofMap(steps, 10,0, 0.5,1.25)));
        b->setResolutionRadius(this->getResolutionRadius());
        b->setResolutionHeight(this->getResolutionHeight());
        b->boom(-this->getHeight()*.5);
        b->rotate(ofRandom(-75,75), ofVec3f(1,0,0));
        b->rotate(ofRandom(-180,180), ofVec3f(0,1,0));
        b->boom(-b->getHeight()*.5);
        
        //b->vbo.addVertices(b->getMesh().getVertices());
        
        branches.push_back(b);
    }
public:
    vector<Branch*> branches;
    ofMesh fullMesh;
   // ofVboMesh vbo;
    float bWidth;
    float bHeight;
    ofVec3f pos;
    
    void make(int steps){
        if(steps > 1){
            for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
                Branch *b = *(it);
                delete b;
            }
            branches.clear();
            int numberBranches = ofRandom(2,4);
            for(int i = 0; i < numberBranches; i++){
                addBranch(steps);
            }
            for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
                Branch *b = *(it);
                b->make(steps-1);
            }
            
            
            fullMesh.addVertices(getAllVertices());
            fullMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
        }
        
        //vbo = computeVbo();
        
    }
    
    void draw(){
        ofColor c = ofGetStyle().color;
        this->ofCylinderPrimitive::draw();
        //  ofSetColor(c.r, c.g, c.b, c.a*0.85);
        ofPushStyle();
        for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
            Branch *b = *(it);
            b->draw();
        }
        ofPopStyle();
    }
    
    void drawFull() {
        fullMesh.draw();
    }
    
    void drawVbo() {
        ofColor c = ofGetStyle().color;
        //this->ofCylinderPrimitive::draw();
        //  ofSetColor(c.r, c.g, c.b, c.a*0.85);
        //this->vbo.drawWireframe();
        //this->vbo.draw();
        //glDrawArrays(GL_TRIANGLES, 0, this->vbo.getNumVertices());
//        this->vbo.draw(GL_TRIANGLES, 0, this->vbo.getNumVertices());
//        this->vbo.unbind();
        
        ofPushStyle();
        for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
            Branch *b = *(it);
            
            
            b->drawVbo();
            
            
            
        }
        ofPopStyle();
    }
    
    ofNode getTopNode(){
        ofNode returnNode(*this);
        returnNode.boom(-this->getHeight()*.5);
        return returnNode;
    }

    vector<ofVec3f> getVecTree(){
        vector<ofVec3f> retVecs;
        retVecs.push_back(getTopNode().getGlobalPosition());
        for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
            Branch *b = *(it);
            vector<ofVec3f> branchVecs = b->getVecTree();
            retVecs.insert(retVecs.end(), branchVecs.begin(), branchVecs.end());
        }
        return retVecs;
    }
    
    
    vector<ofVec3f> getAllVertices(){
        vector<ofVec3f> retVecs;
        
        retVecs.insert(retVecs.end(), this->getMesh().getVertices().begin(), this->getMesh().getVertices().end());
        
        retVecs.push_back(getTopNode().getGlobalPosition());
        for (std::vector<Branch*>::iterator it = branches.begin() ; it != branches.end(); ++it) {
            Branch *b = *(it);
            vector<ofVec3f> branchVecs = b->getAllVertices();
            retVecs.insert(retVecs.end(), branchVecs.begin(), branchVecs.end());
        }
        return retVecs;
    }
    
    
    
    int drawVecTree(){
        vector<ofVec3f> vecs = getVecTree();
        for (std::vector<ofVec3f>::iterator it = vecs.begin() ; it != vecs.end(); ++it) {
            ofVec3f v = *(it);
            ofDrawBox(v, 0.03);
        }
        return vecs.size();
    }
    
};

class Trae : public ContentScene {
    
public:
    void draw(int _surfaceId);
    void update();
    void setup();
    void makeTrees();
    
    void setGui(ofxUICanvas * gui, float width);
    void guiEvent(ofxUIEventArgs &e);
    void receiveOsc(ofxOscMessage * m, string rest);
    
    float zPos;
    
    bool regrow;
    bool hasRegrown;
    vector<Branch*> trees;
    
    void drawVboBranches(Branch * branch);
    
    ofVbo branchVbo;
    ofVboMesh branchVboMesh;
    
};
