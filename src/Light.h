#pragma  once
#ifndef LIGHT_H
#define LIGHT_H

#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>



class Light {
public:
	
	Light();
	Light(glm::vec3 pos, glm::vec3 color);
	virtual ~Light();

	void Light::moveLight(char direction, float value);
	glm::vec3 Light::getPos();
	glm::vec3 Light::getColor();

private:
	glm::vec3 pos;
	glm::vec3 color;
};

#endif
