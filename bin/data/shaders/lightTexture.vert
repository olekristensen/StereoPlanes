#version 150

in vec3 position;
in vec3 normal;
in vec2 texcoord;

out vec3 vertexNormal;
out vec3 cameraSpacePosition;
out vec2 TexCoord;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

uniform NormalMatrix{
	mat3 normalMatrix;
};

void main(){
	vertexNormal = normalize(normalMatrix * normal);
    cameraSpacePosition = (modelViewMatrix * vec4(position,0.0)).xyz;
	TexCoord = texcoord;
	gl_Position = modelViewProjectionMatrix * vec4(position,0.0);
}