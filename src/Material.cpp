#include "Material.h"

#include <algorithm>
#include <iostream>

#include "GLSL.h"
#include "Program.h"

using namespace std;

Material::Material() {
	
}

Material::Material(glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, float s) {
	this->ka = ka;
	this->kd = kd;
	this->ks = ks;

	this->s = s;
}


glm::vec3 Material::getKa() {
	return this->ka;
}

glm::vec3 Material::getKd() {
	return this->kd;
}

glm::vec3 Material::getKs() {
	return this->ks;
}


float Material::getS() {
	return this->s;
}




Material::~Material(){}

