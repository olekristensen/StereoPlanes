#version 150

in vec3 pos;
in vec3 color;

out vec3 vColor; // Output to geometry (or fragment) shader

void main() {
    gl_Position = vec4(pos, 1.0);
    vColor = color;
}



/*#version 150
uniform mat4 viewMatrix, projMatrix;

in vec4 position;
in vec3 color;

out vec3 Color;

void main()
{
    Color = color;
    gl_Position = projMatrix * viewMatrix * position ;
}*/



/*#version 410

in vec3 position;
in vec3 normal;
in vec2 texCoord;

out VertexData {
    vec2 texCoord;
    vec3 normal;
} VertexOut;

void main()
{
    VertexOut.texCoord = texCoord;
    VertexOut.normal = normal;
    gl_Position = vec4(position, 1.0);
}*/


/*#version 150
 
 in vec4 position;
 in vec2 texcoord;
 in vec4 color;
 
 out vec2 TexCoord;
 out vec4 FrontColor;
 
 void main()
 {
 TexCoord = texcoord;
 FrontColor =  color;
 gl_Position = position;
 }*/