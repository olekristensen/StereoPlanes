//
//  deferredRenderer.cpp
//  Trae
//
//  Created by Johan Bichel Lindegaard on 22/03/14.
//
//

#include "deferredRenderer.h"

void DeferredRenderer::beginGeometryPass() {
    glDisable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // CREATE GBUFFER
    // --------------
    // start our GBuffer for writing (pass in near and far so that we can create linear depth values in that range)
    
    m_gBuffer.bindForGeomPass(cam->getNearClip(), cam->getFarClip());
    cam->begin();
}

void DeferredRenderer::endGeometryPass() {
    cam->end();
    m_gBuffer.unbindForGeomPass(); // done rendering out to our GBuffer
}

void DeferredRenderer::deferredRender() {
    
    // final deferred shading pass
    glDisable(GL_DEPTH_TEST);
    ofDisableLighting();
    
    m_shader.begin();
    drawScreenQuad();
    m_shader.end();
    
    glActiveTexture(GL_TEXTURE0);
}

void DeferredRenderer::lightPass() {
    
    m_gBuffer.resetLightPass();
    m_pointLightPassShader.begin();
    
    // pass in lighting info
    int numLights = m_lights.size();
    m_pointLightPassShader.setUniform1i("u_numLights", numLights);
    m_pointLightPassShader.setUniform1f("u_farDistance", cam->getFarClip());
    m_pointLightPassShader.end();
    
    cam->begin();
    
    ofMatrix4x4 camModelViewMatrix = cam->getModelViewMatrix(); // need to multiply light positions by camera's modelview matrix to transform them from world space to view space (reason for this is our normals and positions in the GBuffer are in view space so we must do our lighting calculations in the same space). It's faster to do it here on CPU vs. in shader
    
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
    cam->end();
}

void DeferredRenderer::drawScreenQuad() {
    // set identity matrices and save current matrix state
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // draw the full viewport quad
    m_quadVbo.draw(GL_QUADS, 0, 4);
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}


void DeferredRenderer::draw() {
    geometryPass();
    pointLightPass();
    
    // GENERATE SSAO TEXTURE
    // ---------------------
    // pass in texture units. ssaoPass.applySSAO() expects the required textures to already be bound at these units
    m_ssaoPass.applySSAO(m_textureUnits[TEX_UNIT_NORMALS_DEPTH]);
    
    deferredRender();
    
    if (m_bDrawDebug) {
        m_gBuffer.drawDebug(0, 0);
        m_ssaoPass.drawDebug(ofGetWidth()/4*3, 0);
        
        // draw our debug/message string
        ofEnableAlphaBlending();
        glDisable(GL_CULL_FACE); // need to do this to draw planes in OF because of vertex ordering
        
        ofSetColor(255, 255, 255, 255);
        char debug_str[255];
        sprintf(debug_str, "Framerate: %f\nNumber of lights: %li\nPress SPACE to toggle drawing of debug buffers\nPress +/- to add and remove lights\n'p' to toggle pulsing of light intensity\n'r' to randomize light colours", ofGetFrameRate(), m_lights.size());
        ofDrawBitmapString(debug_str, ofPoint(15, 20));
    }  
}


