#version 120

uniform mat4 P;
uniform mat4 MV;
uniform mat4 IT_MV;

attribute vec4 aPos; // in object space
attribute vec3 aNor; // in object space

varying vec3 vertNor; //in camera space
varying vec3 fragPos; //in camera space

void main() {
	gl_Position = P * (MV * aPos);

	vertNor = normalize((mat3(IT_MV)) * aNor);
	fragPos = vec3(MV * aPos);
}
