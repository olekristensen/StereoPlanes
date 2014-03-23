//
//  deferredScene.cpp
//  Trae
//
//  Created by Johan Bichel Lindegaard on 22/03/14.
//
//

#include "DeferredScene.h"

void  DeferredScene::setup() {
    
}


void DeferredScene::geometryPass(int _surfaceId) {
    
    glActiveTexture(GL_TEXTURE0); // bind concrete texture
    m_texture.getTextureReference().bind();
    
    m_boxVbo.bind();
    
    // draw our randomly rotated boxes
    for (vector<Box>::iterator it=m_boxes.begin() ; it < m_boxes.end(); it++) {
        ofPushMatrix();
        ofRotate(it->angle, it->axis_x, it->axis_y, it->axis_z);
        ofScale(it->size, it->size, it->size);
        glDrawArrays(GL_TRIANGLES, 0, m_numBoxVerts);
        ofPopMatrix();
    }
    
    m_boxVbo.unbind();
    
    glBindTexture(GL_TEXTURE_2D, 0); // unbind the texture
    
}