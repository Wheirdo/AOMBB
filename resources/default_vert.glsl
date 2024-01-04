#version 120

uniform mat4 P;
uniform mat4 MV;
uniform mat4 IT_MV;

attribute vec4 aPos; // in object space
attribute vec3 aNor; // in object space
attribute vec3 aTex; // in object space

varying vec3 vertNor; //in camera space
varying vec3 fragPos; //in camera space

varying vec3 color; // Pass to fragment shader
void main()
{
	gl_Position = P * (MV * aPos);
	color = 0.5 * aNor + vec3(0.5, 0.5, 0.5);
}
