#include "Light.h"

#include <algorithm>
#include <iostream>

#include "GLSL.h"
#include "Program.h"

using namespace std;

Light::Light() {
	
}

Light::Light(glm::vec3 pos, glm::vec3 color) {
	this->pos = pos;
	this->color = color;
}

glm::vec3 Light::getPos() {
	return this->pos;
}

glm::vec3 Light::getColor() {
	return this->color;
}


Light::~Light(){}

