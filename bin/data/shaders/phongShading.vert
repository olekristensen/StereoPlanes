#version 150

in vec4 position;
in vec3 normal;

flat out vec3 vertexNormalFlat;
flat out vec3 cameraSpacePositionFlat;

out vec3 vertexNormal;
out vec3 cameraSpacePosition;

uniform mat4 modelViewProjectionMatrix;
uniform mat4 modelViewMatrix;

uniform float vertexNoise;

/*uniform NormalMatrix{
	mat3 matrix;
}normalMatrix;
*/

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
	//vertexNormal = normalize(normalMatrix.matrix * normal);
    // this is a dirty hack!!
    vertexNormal = vec3(modelViewMatrix * vec4(normal,0.0));
    vertexNormalFlat = vec3(modelViewMatrix * vec4(normal,0.0));
    vec3 vertexOffset = vec3(1.0,1.0,1.0);
    vertexOffset.x -= rand(position.yz)*2.0;
    vertexOffset.y -= rand(position.xz)*2.0;
    vertexOffset.z -= rand(position.xy)*2.0;
    vertexOffset*=vertexNoise;
    cameraSpacePosition = (modelViewMatrix * position).xyz;
    cameraSpacePosition += vertexOffset;
    cameraSpacePositionFlat = (modelViewMatrix * position).xyz;
    cameraSpacePositionFlat += vertexOffset;
	gl_Position = modelViewProjectionMatrix * (position+vec4(vertexOffset,0.0));
}