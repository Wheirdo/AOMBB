#pragma  once
#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>



class Material {
public:
	
	Material();
	Material(glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, float s);
	virtual ~Material();
	glm::vec3 getKa();
	glm::vec3 getKd();
	glm::vec3 getKs();
	float getS();


private:
	glm::vec3 ka;
	glm::vec3 kd;
	glm::vec3 ks;
	float s;
};

#endif
