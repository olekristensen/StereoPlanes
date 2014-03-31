/***********************************************
 Geometry shader to convert lines into triangle strips
 Memo Akten
 www.memo.tv

************************************************/

#version 150

uniform float thickness;
uniform vec3 lightDir;

uniform mat4 modelViewProjectionMatrix;

out vec4 Color;
in vec4 vertexColor[];


void main() {
	vec3 p0 = gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_in[1].gl_Position.xyz;
	
	vec3 up = vec3(0, 0, 1);	// arbitrary up vector
	
	vec3 dir = normalize(p1 - p0);			// normalized direction vector from p0 to p1
	vec3 right = normalize(cross(dir, up));	// right vector
	vec3 norm = cross(right, dir);
	float fColMult = abs(dot(norm, lightDir));
	vec4 colMult = vec4(fColMult, fColMult, fColMult, 1.0);
	
	right *= thickness;
	
	gl_Position = modelViewProjectionMatrix * vec4(p0 - right, 1.0);
	Color = vertexColor[0] * colMult;
	EmitVertex();
	
	gl_Position = modelViewProjectionMatrix * vec4(p0 + right, 1.0);
	Color = vertexColor[0] * colMult;
	EmitVertex();
	
	gl_Position = modelViewProjectionMatrix * vec4(p1 - right, 1.0);
	Color = vertexColor[1] * colMult;
	EmitVertex();

	gl_Position = modelViewProjectionMatrix * vec4(p1 + right, 1.0);
	Color = vertexColor[1] * colMult;
	EmitVertex();
    
    /* todo use input color - find 150 syntax
    gl_Position = modelViewProjectionMatrix * vec4(p1 + right, 1.0);
	outColor = inColor * colMult;
	EmitVertex();*/

}