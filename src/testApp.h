#pragma once

#include "ofMain.h"
#include "ofxSyphon.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"
#include "ofxOscReceiver.h"
#include "StereoPlane.h"
#include "ContentScene.h"
#include "ofxUI.h"
#include "Trae.h"
#include "ofxUbo.h"


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

class testApp : public ofBaseApp
{
    struct Box {
        ofVec3f pos;
        float size;
        float angle;
        float axis_x;
        float axis_y;
        float axis_z;
        
        Box(ofVec3f pos=ofVec3f(0.0f, 0.0f, 0.0f), float angle=0.0f, float ax=0.0f, float ay=0.0f, float az=0.0f, float size=2.0f) :
        pos(pos),
        size(size),
        angle(angle),
        axis_x(ax),
        axis_y(ay),
        axis_z(az)
        {}
    };
    
    static const int skNumBoxes = 50;
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
	
	void setup();
	void update();
	void draw();
	
    void drawFly();
    
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
    
    ofMatrix3x3 calcNormalMatrix(ofMatrix4x4 matrix);
    
    void exit();

    ofxSyphonServer sbsOutputServer;
    
    ofxOscReceiver oscReceiver;
//    ofxOscSender oscSender;
    
    ofVec3f camPosWall;
    float eyeSeperation;
    
    float dancerEllipseSize;
    ofVec2f dancerPos;
    
    bool drawFBOs = false;
    bool drawChessboards = false;
    bool drawGrids = false;
    
    float aspect = 1.0;
    
    ofxUIScrollableCanvas *gui;
    void guiEvent(ofxUIEventArgs &e);
    
    ofFbo fbo;
    StereoPlane * wall;
    StereoPlane * activePlane;
    int activePlaneIndex;
    
    void drawFloor();
    vector<StereoPlane *> planes;
    ofxXmlSettings settings;
    
    float dancerHeight;
    
    void drawScenes(int _surfaceId=0);
    
    // Scenes
    Trae * trae;
    vector<ContentScene*> contentScenes;
    
    // Lights
    
    ofxUboShader lightShader;
    vector<shaderLight*> lights;
    Light light;
    
    Material white;

    float vertexNoise;
    
    // Time
    
    float time = 0;;
    float speed;
    
    /// Deferred rendering
    ofEasyCam m_cam;
    
    void setupModel();
    void setupLights();
    void setupScreenQuad();
    void setupPointLightPassFbo();
    //void resizeBuffersAndTextures();
    
    void addRandomLight();
    void createRandomBoxes();
    void randomizeLightColors();
    
    void unbindGBufferTextures();
    void bindGBufferTextures();
    
    void drawScreenQuad();
    
    void geometryPass();
    void pointLightStencilPass();
    void pointLightPass();
    void deferredRender();
    
    GBuffer m_gBuffer;
    
    SSAOPass m_ssaoPassLeft;
    SSAOPass m_ssaoPassRight;
    
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
    vector<Box> m_boxes;
    
    ///////////////////////////////////////

    
};