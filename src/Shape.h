#pragma once
#ifndef SHAPE_H
#define SHAPE_H

#include <string>
#include <vector>
#include <memory>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



using std::string, std::map;



class Program;

/**
 * A shape defined by a list of triangles
 * - posBuf should be of length 3*ntris
 * - norBuf should be of length 3*ntris (if normals are available)
 * - texBuf should be of length 2*ntris (if texture coords are available)
 * posBufID, norBufID, and texBufID are OpenGL buffer identifiers.
 */
class Shape
{
public:

	Shape();
	virtual ~Shape();
	void loadMesh(const std::string &meshName);
	void setMesh(std::vector<float> posBuf, std::vector<float> norBuf, std::vector<float> texBuf, std::vector<unsigned int> indBuf);
	void fitToUnitBox();
	void moveToCenter();
	void init();
	void draw(const std::shared_ptr<Program> prog);

	void spherify();
	std::vector<glm::vec3> Shape::getBoundingBox();

	void updateBounds();
	glm::vec3 getVMin() const;
	glm::vec3 getVMax() const;
	glm::vec3 getCenter() const;
	
private:
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;
	std::vector<unsigned int> indBuf;
	unsigned posBufID;
	unsigned norBufID;
	unsigned texBufID;

	glm::vec3 vmin;
	glm::vec3 vmax;
	glm::vec3 center;

	map<string, GLuint> bufIDs;
	int indCount;
};

#endif
