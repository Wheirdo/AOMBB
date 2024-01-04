#include "Shape.h"
#include <algorithm>
#include <iostream>


#include "GLSL.h"
#include "Program.h"



#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace std;

Shape::Shape() :
	posBufID(0),
	norBufID(0),
	texBufID(0)
{
}

Shape::~Shape()
{
}

void Shape::loadMesh(const string &meshName) {
	// Load geometry
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	string errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are going to duplicate all such
		// vertices.
		// Loop over shapes
		for(size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polygons)
			size_t index_offset = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for(size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+1]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+2]);
					if(!attrib.normals.empty()) {
						norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
					}
					if(!attrib.texcoords.empty()) {
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}

	this->indCount = 0;
}


void Shape::setMesh(vector<float> posBuf, vector<float> norBuf, vector<float> texBuf, vector<unsigned int> indBuf) {
	this->posBuf = posBuf;
	this->norBuf = norBuf;
	this->texBuf = texBuf;
	this->indBuf = indBuf;

	this->indCount = indBuf.size();
}

/*
vector<glm::vec3> Shape::getBoundingBox() {

	//Compute edgeBuffer
	vector<glm::vec3> edgeBuffer;
	for (int i = 0; i < posBuf.size()/9; i+=9) {
		glm::vec3 v1 = glm::vec3(posBuf.at(i + 0), posBuf.at(i + 1), posBuf.at(i + 2));
		glm::vec3 v2 = glm::vec3(posBuf.at(i + 3), posBuf.at(i + 4), posBuf.at(i + 5));
		glm::vec3 v3 = glm::vec3(posBuf.at(i + 6), posBuf.at(i + 7), posBuf.at(i + 8));


		edgeBuffer.push_back(normalize(v2 - v1));
		edgeBuffer.push_back(normalize(v3 - v2));
		edgeBuffer.push_back(normalize(v1 - v3));
	}

	//cout << this->getVMin().x << "," << this->getVMin().y << this->getVMin().z << endl;
	//cout << this->getVMax().x << "," << this->getVMax().y << this->getVMax().z << endl;



	vector<glm::vec3> potentialBoundingBoxes;

	//Loop thru all the edges
	for (glm::vec3 edge : edgeBuffer) {

		//project all points P onto the vector
		glm::vec3 vMin = glm::vec3(posBuf.at(0), posBuf.at(1), posBuf.at(2));
		glm::vec3 vMax = glm::vec3(posBuf.at(0), posBuf.at(1), posBuf.at(2));
		float currMinDistance = dot(vMin, edge);
		float currMaxDistance = dot(vMax, edge);
		
		//cout << "Start Min " << currMinDistance << endl;
		//cout << "Start Max " << currMaxDistance << endl;
	
		for (int i = 0; i < posBuf.size(); i+=3) {
			glm::vec3 vertex = glm::vec3(posBuf.at(i + 0), posBuf.at(i + 1), posBuf.at(i + 2));


			//project vertex onto edge
			float distance = dot(vertex, edge);

			//if vertex is bigger or smaller than max or min, store it
			if (distance > currMaxDistance) {
				vMax = vertex;
				currMaxDistance = distance;
				//cout << "New Max:" << currMaxDistance << endl;
			}
			else if (distance < currMinDistance) {
				vMin = vertex;
				currMinDistance = distance;
				//cout << "New Min:" << currMinDistance << endl;
			}

		}



		//cout << "Final Min:" << currMinDistance << endl;
		//cout << "Final Max: " << currMaxDistance << endl;
		

		//float curr_volume = abs(vMax.x - vMin.x) * abs(vMax.y - vMin.y) * abs(vMax.z - vMin.z);
		//cout << "Volume: " << curr_volume << endl;

		//cout << "vMin: " << vMin.x << "," << vMin.y << "," << vMin.z << endl;
		//cout << "vMax: " << vMax.x << "," << vMax.y << "," << vMax.z << endl << endl << endl;
		potentialBoundingBoxes.push_back(vMin);
		potentialBoundingBoxes.push_back(vMax);

		//break;
	}
		


	//glm::vec3 vMin = glm::vec3(5.0f, 5.0f, 5.0f);
	//glm::vec3 vMax = glm::vec3(-5.0f, -5.0f, -5.0f);
	glm::vec3 vMin = glm::vec3(0,0,0);
	glm::vec3 vMax = glm::vec3(0,0,0);
	float curr_volume = abs(vMax.x - vMin.x) * abs(vMax.y - vMin.y) * abs(vMax.z - vMin.z);

	//cout << "Starting: " << curr_volume << endl;

	for (int i = 0; i < potentialBoundingBoxes.size(); i+=2) {

		glm::vec3 v1 = potentialBoundingBoxes.at(i);
		glm::vec3 v2 = potentialBoundingBoxes.at(i + 1);
		float volume = abs(v2.x - v1.x) * abs(v2.y - v1.y) * abs(v2.z - v1.z);


		if (volume > curr_volume) {
			vMin = v1;
			vMax = v2;

			curr_volume = volume;

			//cout << "V1: " << v1.x << "," << v1.y << "," << v1.z << endl;
			//cout << "V2: " << v1.x << "," << v1.y << "," << v1.z << endl;

			//cout << "i:" << i << " " << curr_volume << endl;
		}
	}


	vector<glm::vec3> bounding_box;
	bounding_box.push_back(vMin);
	bounding_box.push_back(vMax);

	return bounding_box;

}
*/



ostream& operator<<(ostream& os, const glm::vec3& vector) {
	os << vector.x << ',' << vector.y << ',' << vector.z;
	return os;
}



vector<glm::vec3> Shape::getBoundingBox() {

	glm::vec3 minBB = { FLT_MAX, FLT_MAX, FLT_MAX };
	glm::vec3 maxBB = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	float volumeBB = abs(maxBB.x - minBB.x) * abs(maxBB.y - minBB.y) * abs(maxBB.z - minBB.z);
	glm::mat4 finalRotation;
	

	const float step = glm::pi<float>() / 180.0f;
	for (float theta = 0; theta < glm::pi<float>() / 2; theta += step) {
		for (float phi = 0; phi < glm::pi<float>(); phi += step) {


			//Rotation Matrix (around X and Z)
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), theta, glm::vec3(1, 0, 0));
			rotation *= glm::rotate(rotation, phi, glm::vec3(0, 0, 1));

			
			//Rotate all values 
			glm::vec3 rotatedMin(FLT_MAX, FLT_MAX, FLT_MAX);
			glm::vec3 rotatedMax(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			for (int i = 0; i < posBuf.size() / 3; i++) {
				glm::vec3 vertex(posBuf.at(i * 3), posBuf.at(i * 3 + 1), posBuf.at(i * 3 + 2));

				vertex = glm::vec3(rotation * glm::vec4(vertex, 1.0f));

				//Find the smallest and largest values when rotated
				rotatedMin = glm::min(rotatedMin, vertex);
				rotatedMax = glm::max(rotatedMax, vertex);
			}


			// Update final rotation matrix and bounding box
			if (glm::dot(rotatedMax - rotatedMin, rotatedMax - rotatedMin) < glm::dot(maxBB - minBB, maxBB - minBB)) {
				minBB = rotatedMin;
				maxBB = rotatedMax;
				finalRotation = rotation;
			}
		}
	}



	//Rotate entire object to fit constructed bounding box
	for (int i = 0; i < posBuf.size() / 3; i++) {
		glm::vec3 vertex(posBuf.at(i * 3), posBuf.at(i * 3 + 1), posBuf.at(i * 3 + 2));

		vertex = glm::vec3(finalRotation * glm::vec4(vertex, 1.0f));

		posBuf.at(i * 3 + 0) = vertex.x;
		posBuf.at(i * 3 + 1) = vertex.y;
		posBuf.at(i * 3 + 2) = vertex.z;
	}


	//Move Object back to the center
	this->moveToCenter();
	this->init();



	//Move bounding box to center
	glm::vec3 centerBB = 0.5f * (minBB + maxBB);
	minBB -= centerBB;
	maxBB -= centerBB;

	//Return answer
	vector<glm::vec3> bounding_box;
	bounding_box.push_back(minBB);
	bounding_box.push_back(maxBB);

	return bounding_box;
}




//Transforms the given shape into a Gaussian Sphere
// it is a unit sphere, with one polygon (or region, denoted as R(v)), where v is a vertex of the input polyhedra P
// we define a "supporting plane" as a plane that is a unit normal vector, n, that starts at the origin, is unit 
void Shape::spherify() {
	std::vector<float> spherePositions;
	std::vector<float> sphereNormals;
	std::vector<unsigned int> sphereIndices;


	//Create a unit sphere
	int polygons = std::sqrt(posBuf.size()/3);
	for (int i = 0; i < polygons; i++) {//rows
		for (int j = 0; j < polygons; j++) {//columns

			float theta = i * (glm::pi<float>() / (polygons - 1));
			float phi = j * (2 * glm::pi<float>() / (polygons - 1));

			float x = 0.5 * sin(theta) * sin(phi);
			float y = 0.5 * cos(theta);
			float z = 0.5 * sin(theta) * cos(phi);

			//Vertex
			spherePositions.push_back(x);
			spherePositions.push_back(y);
			spherePositions.push_back(z);

			//Normals
			sphereNormals.push_back(x);
			sphereNormals.push_back(y);
			sphereNormals.push_back(z);
		}
	}



	// Compute supporting planes for each vertex
	std::vector<glm::vec4> supportingPlanes(posBuf.size() / 3);
	for (unsigned int i = 0; i < posBuf.size(); i += 3) {
		glm::vec3 v = glm::vec3(posBuf[i], posBuf[i + 1], posBuf[i + 2]);
		glm::vec3 n = glm::vec3(norBuf[i], norBuf[i + 1], norBuf[i + 2]);
		glm::vec4 plane(n.x, n.y, n.z, -glm::dot(n, v));
		supportingPlanes[i / 3] = plane;
	}
	

	// Map sphere vertices to supporting planes
	for (unsigned int i = 0; i < spherePositions.size(); i+=3) {
		glm::vec3 p = glm::vec3(spherePositions[i+0], spherePositions[i+1], spherePositions[i+2]);
		for (unsigned int j = 0; j < posBuf.size(); j+=3) {
			glm::vec4 plane = supportingPlanes[j/3];
			if (glm::dot(glm::vec3(plane), p) + plane.w > 0) {

				glm::vec3 temp = p * glm::length(posBuf[j]);
				spherePositions[i + 0] = temp.x;
				spherePositions[i + 1] = temp.y;
				spherePositions[i + 2] = temp.z;


				glm::vec3 temp2 = glm::vec3(plane);
				sphereNormals[i + 0] = temp2.x;
				sphereNormals[i + 1] = temp2.y;
				sphereNormals[i + 2] = temp2.z;

				break;
			}
		}
	}


	//Generate sphere indicies
	for (int i = 0; i < polygons - 1; i++) {
		for (int j = 0; j < polygons - 1; j++) {
			sphereIndices.push_back(i + 0 + j * (polygons));
			sphereIndices.push_back(i + 1 + j * (polygons));
			sphereIndices.push_back(i + 1 + (j + 1) * (polygons));

			sphereIndices.push_back(i + 0 + j * (polygons));
			sphereIndices.push_back(i + 1 + (j + 1) * (polygons));
			sphereIndices.push_back(i + 0 + (j + 1) * (polygons));
		}
	}

	this->posBuf = spherePositions;
	this->norBuf = sphereNormals;
	this->indBuf = sphereIndices;
	this->indCount = sphereIndices.size();

	//this->init();
}



void Shape::fitToUnitBox(){

	updateBounds();

	glm::vec3 diff = vmax - vmin;
	float diffmax = diff.x;
	diffmax = max(diffmax, diff.y);
	diffmax = max(diffmax, diff.z);
	float scale = 1.0f / diffmax;
	for(int i = 0; i < (int)posBuf.size(); i += 3) {
		posBuf[i  ] = (posBuf[i  ] - center.x) * scale;
		posBuf[i+1] = (posBuf[i+1] - center.y) * scale;
		posBuf[i+2] = (posBuf[i+2] - center.z) * scale;
	}

	updateBounds();
}

void Shape::moveToCenter() {

	updateBounds();

	for (int i = 0; i < (int)posBuf.size(); i += 3) {
		posBuf[i + 0] = (posBuf[i + 0] - center.x);
		posBuf[i + 1] = (posBuf[i + 1] - center.y);
		posBuf[i + 2] = (posBuf[i + 2] - center.z);
	}

	updateBounds();
}


void Shape::init() {

	//Normal OBJ file
	if (indCount == 0) {
		// Send the position array to the GPU
		glGenBuffers(1, &posBufID);
		glBindBuffer(GL_ARRAY_BUFFER, posBufID);
		glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);

		// Send the normal array to the GPU
		if (!norBuf.empty()) {
			glGenBuffers(1, &norBufID);
			glBindBuffer(GL_ARRAY_BUFFER, norBufID);
			glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);
		}

		// Send the texture array to the GPU
		if (!texBuf.empty()) {
			glGenBuffers(1, &texBufID);
			glBindBuffer(GL_ARRAY_BUFFER, texBufID);
			glBufferData(GL_ARRAY_BUFFER, texBuf.size() * sizeof(float), &texBuf[0], GL_STATIC_DRAW);
		}

		// Unbind the arrays
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	//Constructed Mesh
	else {
		GLuint tmp[3];
		glGenBuffers(3, tmp);
		bufIDs["bPos"] = tmp[0];
		bufIDs["bNor"] = tmp[1];
		bufIDs["bInd"] = tmp[2];
		glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bPos"]);
		glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bNor"]);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIDs["bInd"]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indBuf.size() * sizeof(unsigned int), &indBuf[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	//Calculate vmin, vmax, and center
	updateBounds();
	
	GLSL::checkError(GET_FILE_LINE);
}

void Shape::draw(const shared_ptr<Program> prog) {


	//Load OBJ file
	if (this->indCount == 0) {
		// Bind position buffer
		size_t h_pos = prog->getAttribute("aPos");
		glEnableVertexAttribArray(h_pos);
		glBindBuffer(GL_ARRAY_BUFFER, posBufID);
		glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	
		// Bind normal buffer
		int h_nor = prog->getAttribute("aNor");
		if(h_nor != -1 && norBufID != 0) {
			glEnableVertexAttribArray(h_nor);
			glBindBuffer(GL_ARRAY_BUFFER, norBufID);
			glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	
		// Bind texcoords buffer
		int h_tex = prog->getAttribute("aTex");
		if(h_tex != -1 && texBufID != 0) {
			glEnableVertexAttribArray(h_tex);
			glBindBuffer(GL_ARRAY_BUFFER, texBufID);
			glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}
	
		// Draw
		int count = posBuf.size()/3; // number of indices to be rendered
		glDrawArrays(GL_TRIANGLES, 0, count);
	
		// Disable and unbind
		if(h_tex != -1) {
			glDisableVertexAttribArray(h_tex);
		}
		if(h_nor != -1) {
			glDisableVertexAttribArray(h_nor);
		}
		glDisableVertexAttribArray(h_pos);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	
		GLSL::checkError(GET_FILE_LINE);
	}
	//Constructed Mesh
	else {

		glEnableVertexAttribArray(prog->getAttribute("aPos"));
		glEnableVertexAttribArray(prog->getAttribute("aNor"));
		glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bPos"]);
		glVertexAttribPointer(prog->getAttribute("aPos"), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ARRAY_BUFFER, bufIDs["bNor"]);
		glVertexAttribPointer(prog->getAttribute("aNor"), 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIDs["bInd"]);
		glDrawElements(GL_TRIANGLES, indCount, GL_UNSIGNED_INT, (void*)0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisableVertexAttribArray(prog->getAttribute("aNor"));
		glDisableVertexAttribArray(prog->getAttribute("aPos"));

	}
}


void Shape::updateBounds() {
	vmin = glm::vec3(posBuf[0], posBuf[1], posBuf[2]);
	vmax = glm::vec3(posBuf[0], posBuf[1], posBuf[2]);

	for (int i = 0; i < (int)posBuf.size(); i += 3) {
		glm::vec3 v(posBuf[i], posBuf[i + 1], posBuf[i + 2]);
		vmin.x = min(vmin.x, v.x);
		vmin.y = min(vmin.y, v.y);
		vmin.z = min(vmin.z, v.z);
		vmax.x = max(vmax.x, v.x);
		vmax.y = max(vmax.y, v.y);
		vmax.z = max(vmax.z, v.z);
	}

	center = 0.5f * (vmin + vmax);
}

//Getters
glm::vec3 Shape::getVMin() const{
	return this->vmin;
}

glm::vec3 Shape::getVMax() const {
	return this->vmax;
}

glm::vec3 Shape::getCenter() const {
	return this->center;
}