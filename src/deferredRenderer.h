//
//  deferredRenderer.h
//  Trae
//
//  Created by Johan Bichel Lindegaard on 22/03/14.
//
//

#pragma once
#include "gBuffer.h"
#include "ssaoPass.h"
#include "pointLight.h"
#include "primitives.h"


struct Material {
    ofVec4f diffuseColor;
    ofVec4f specularColor;
    float specularShininess;
};

struct PerLight {
    ofVec3f cameraSpaceLightPos;
    ofVec4f lightIntensity;
    float lightAttenuation;
};

struct Light {
    ofVec4f ambientIntensity;
    int numberLights;
    PerLight lights[20];
};

class shaderLight : public ofNode {
    
};

class DeferredRenderer {
    
    static const int skNumLights = 100;
    static const int skRadius = 20;
    static const int skMaxPointLightRadius = 8;
    
    enum TEXTURE_UNITS {
        TEX_UNIT_ALBEDO,
        TEX_UNIT_NORMALS_DEPTH,
        TEX_UNIT_SSAO,
        TEX_UNIT_POINTLIGHT_PASS,
        TEX_UNIT_NUM_UNITS
    };
    
public:
    ofCamera * cam; // pointer to the camera for the scene
    // mabe put a pointer to the scene here
    
    void genericSetup() {
        ofDisableArbTex();
    };
    
    void setup(ofCamera *_cam) {
        cam = _cam;
        
        m_shader.load("shaders/mainScene.vert", "shaders/mainScene.frag");
        
    };
    
    void beginGeometryPass();
    
    void endGeometryPass();
    
    void lightPass();
    
    void debugDraw() {
        
    };
    
    /// Deferred rendering
    
    void setupModel();
    void setupLights();
    void setupScreenQuad();
    void setupPointLightPassFbo();
    //void resizeBuffersAndTextures();
    
    void addRandomLight();
    //void createRandomBoxes();
    void randomizeLightColors();
    
    void unbindGBufferTextures();
    void bindGBufferTextures();
    
    void drawScreenQuad();
    
    void geometryPass();
    void pointLightStencilPass();
    void pointLightPass();
    void deferredRender();
    
    GBuffer m_gBuffer;
    
    SSAOPass m_ssaoPass;
    ofVbo m_quadVbo;
    
    ofShader m_shader;
    ofShader m_pointLightPassShader;
    ofShader m_pointLightStencilShader;
    
    ofImage m_texture;
    
    GLuint m_textureUnits[TEX_UNIT_NUM_UNITS];
    
    ofVbo  m_sphereVbo;
    int    m_numSphereVerts;
    
    ofVbo  m_boxVbo;
    int    m_numBoxVerts;
    
    float   m_angle;
    
    bool    m_bDrawDebug;
    bool    m_bPulseLights;
    
    int     m_windowWidth;
    int     m_windowHeight;
    
    vector<PointLight> m_lights;
    
    
    ///////////////////////////////////////
    
    

    
};