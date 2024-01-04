#version 120

//Material
uniform vec3 lightPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 vertNor; //in camera space
varying vec3 fragPos; //in camera space



void main() {
	vec3 n = normalize(vertNor);
	vec3 eye = normalize(-1 * normalize(fragPos));

	float dot_product = dot(n,eye);
	if (dot_product < 0.3) {
		gl_FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0);
	}
	else {
		gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);
	}
}
