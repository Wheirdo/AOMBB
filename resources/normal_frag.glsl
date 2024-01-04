#version 120

//Material
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

//Light
uniform vec3 lightPos;
uniform vec3 lightCol;

varying vec3 vertNor; //in camera space
varying vec3 fragPos; //in camera space

void main() {
	vec3 n = normalize(vertNor);
	vec3 l = normalize((lightPos - fragPos));
	float diffuse = max(dot(l,n),0);

	vec3 eye = normalize(-1 * fragPos);
	vec3 h = normalize(l + eye);
	float specular = pow(max(dot(h,n),0),s);

	vec3 color = lightCol*(ka + diffuse*kd + ks*specular);
	gl_FragColor = vec4(color.r, color.g, color.b, 1.0);
}
