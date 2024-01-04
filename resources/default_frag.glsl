#version 120

//Material
uniform vec3 lightPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 vertNor; //in camera space
varying vec3 fragPos; //in camera space

varying vec3 color; // passed from the vertex shader

void main()
{
	gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}
