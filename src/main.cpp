#include <cassert>
#include <cstring>
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "Camera.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Shape.h"
#include "Material.h"
#include "Light.h"

using namespace std;
using glm::vec3, glm::mat4;


GLFWwindow *window;           // Main application window
string RESOURCE_DIR = "./";   // Where the resources are loaded from

shared_ptr<Camera> camera;
vector<shared_ptr<Program>> the_programs;
vector<shared_ptr<Shape>> the_shapes;

shared_ptr<Shape> box;
vector<mat4> basic_bb;
vector<mat4> arbitrarilyOriented_bb;

bool keyToggles[256] = {false};  // only for English keyboards!


int shader_count;                //keeps track of current shader
int shapes_count;                //keeps track of the current shape


// This function is called when a GLFW error occurs
static void error_callback(int error, const char *description) {
	cerr << description << endl;
}

// This function is called when a key is pressed
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

// This function is called when the mouse is clicked
static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
	// Get the current mouse position.
	double xmouse, ymouse;
	glfwGetCursorPos(window, &xmouse, &ymouse);
	// Get current window size.
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	if(action == GLFW_PRESS) {
		bool shift = (mods & GLFW_MOD_SHIFT) != 0;
		bool ctrl  = (mods & GLFW_MOD_CONTROL) != 0;
		bool alt   = (mods & GLFW_MOD_ALT) != 0;
		camera->mouseClicked((float)xmouse, (float)ymouse, shift, ctrl, alt);
	}
}

// This function is called when the mouse moves
static void cursor_position_callback(GLFWwindow* window, double xmouse, double ymouse) {
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	if(state == GLFW_PRESS) {
		camera->mouseMoved((float)xmouse, (float)ymouse);
	}
}

static void char_callback(GLFWwindow *window, unsigned int key) {
	keyToggles[key] = !keyToggles[key];


	switch (key) {
		case 's': {
			shader_count += 1;
			break;
		}
		case 'S': {
			shader_count += 1;
			break;
		}
		case 'd': {
			shapes_count += 1;
			break;
		}
		case 'D': {
			shapes_count += 1;
			break;
		}
	}

}


// If the window is resized, capture the new size and reset the viewport
static void resize_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}


static mat4 cubeToBoundingBox(vec3 p1, vec3 p2) {
	vec3 center = (p1 + p2) / 2.0f;

	double dx = std::abs(p2.x - p1.x);
	double dy = std::abs(p2.y - p1.y);
	double dz = std::abs(p2.z - p1.z);

	cout << dx * dy * dz << endl;

	mat4 scale = glm::mat4(1.0f);
	scale[0][0] = dx;
	scale[1][1] = dy;
	scale[2][2] = dz;

	mat4 translation = glm::mat4(1.0f);
	translation[0][3] = -center.x;
	translation[1][3] = -center.y;
	translation[2][3] = -center.z;

	mat4 trans = translation * scale;

	return trans;
}


static void init() {
	// Initialize time.
	glfwSetTime(0.0);
	
	// Set background color.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	camera = make_shared<Camera>();
	camera->setInitDistance(2.0f); // Camera's initial Z translation


	//Load OBJ Files
	vector<string> obj_files = { "bunny", "teapot", "ashtray", "triangle", "molecule" , "sphere", "great_dodecahedron"};
	for (int x = 0; x < obj_files.size(); x++) {
		//Regular file
		the_shapes.push_back(make_shared<Shape>());
		the_shapes[the_shapes.size()-1]->loadMesh(RESOURCE_DIR + obj_files[x] + ".obj");
		the_shapes[the_shapes.size()-1]->fitToUnitBox();
		the_shapes[the_shapes.size()-1]->init();

		//Convex Hull of the input
		the_shapes.push_back(make_shared<Shape>());
		the_shapes[the_shapes.size()-1]->loadMesh(RESOURCE_DIR + obj_files[x] + "_convex.obj");
		the_shapes[the_shapes.size()-1]->fitToUnitBox();
		the_shapes[the_shapes.size()-1]->init();

		//Guassian Sphere repersentation of the Convex Hull
		the_shapes.push_back(make_shared<Shape>());
		the_shapes[the_shapes.size() - 1]->loadMesh(RESOURCE_DIR + obj_files[x] + "_convex.obj");
		the_shapes[the_shapes.size() - 1]->spherify();
		the_shapes[the_shapes.size() - 1]->fitToUnitBox();
		the_shapes[the_shapes.size() - 1]->init();
	}



	//Load In 1x1x1 Box
	box = make_shared<Shape>();
	box->loadMesh(RESOURCE_DIR + "cube.obj");
	box->fitToUnitBox();
	box->init();


	//Compute and store bounding boxes
	for (int x = 0; x < the_shapes.size(); x+=3) {
		cout << "Bounding Box Volume for '" << obj_files.at(x / 3) << "'" << endl;
		cout << "Default Bounding Box: ";
		basic_bb.push_back(cubeToBoundingBox(the_shapes.at(x+1)->getVMin(), the_shapes.at(x + 1)->getVMax()));

		cout << "Arbitrarily Oriented Bounding Box: ";
		vector<vec3> bounding_box = the_shapes.at(x + 1)->getBoundingBox();
		arbitrarilyOriented_bb.push_back(cubeToBoundingBox(bounding_box.at(0), bounding_box.at(1)));
		cout << "" << endl;
	}





	GLSL::checkError(GET_FILE_LINE);

	//Intialize the programs (shaders)
	shader_count = 0;
	vector<string> shaders_names = { "default", "normal", "silhouette"};

	for (int x = 0; x < shaders_names.size(); x++) {
		the_programs.push_back(make_shared<Program>());
		the_programs[x]->setShaderNames(RESOURCE_DIR + shaders_names[x] + "_vert.glsl", RESOURCE_DIR + shaders_names[x] + "_frag.glsl");

	
		the_programs[x]->setVerbose(true);
		the_programs[x]->init();
		the_programs[x]->addAttribute("aPos");
		the_programs[x]->addAttribute("aNor");
		the_programs[x]->addAttribute("aTex");
		the_programs[x]->addUniform("MV");
		the_programs[x]->addUniform("P");
		the_programs[x]->addUniform("IT_MV");
		the_programs[x]->addUniform("lightPos");
		the_programs[x]->addUniform("lightCol");
		the_programs[x]->addUniform("ka");
		the_programs[x]->addUniform("kd");
		the_programs[x]->addUniform("ks");
		the_programs[x]->addUniform("s");
	}



	shader_count = 0;
	shapes_count = 0;
}

// This function is called every frame to draw the scene.
static void render() {
	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(keyToggles[(unsigned)'w']) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	camera->setAspect((float)width/(float)height);
	
	double t = glfwGetTime();
	if(!keyToggles[(unsigned)' ']) {
		// Spacebar turns animation on/off
		t = 0.0f;
		glfwSetTime(0.0);
	}
	
	// Matrix stacks
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();

	// Apply camera transforms
	P->pushMatrix();
	camera->applyProjectionMatrix(P);
	MV->pushMatrix();	
	camera->applyViewMatrix(MV);

	glm::mat4 inverse_MV;
	glm::mat4 inverse_transpose_MV;

	Light the_light = Light(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.8f, 0.8f, 0.8f));
	Material the_material = Material(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.8f, 0.7f, 0.7f), glm::vec3(1.0f, 0.9f, 0.8f), 100);
	shared_ptr<Program> prog = the_programs[shader_count%3];



	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));


	int currShapeCount = shapes_count % the_shapes.size();
	int currBBCount = currShapeCount / 3;



	//Draw the current object
	std::shared_ptr<Shape> object = the_shapes[currShapeCount];

	MV->pushMatrix();
	MV->rotate(t, 0.0f, 1.0f, 0.0f);
	inverse_MV = glm::inverse(MV->topMatrix());
	inverse_transpose_MV = glm::transpose(inverse_MV);
	glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
	glUniformMatrix4fv(prog->getUniform("IT_MV"), 1, GL_FALSE, glm::value_ptr(inverse_transpose_MV));
	object->draw(prog);
	MV->popMatrix();


	//Draw bounding box
	if (currShapeCount % 3 != 2) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		MV->pushMatrix();
		
		MV->rotate(t, 0.0f, 1.0f, 0.0f);
		//Basic Bounding Box
		if (currShapeCount % 3 == 0) {
			MV->multMatrix(basic_bb.at(currBBCount));
		}
		//Facny Bounding Box
		else if (currShapeCount % 3 == 1) {
			MV->multMatrix(arbitrarilyOriented_bb.at(currBBCount));
		}
		

		
		inverse_MV = glm::inverse(MV->topMatrix());
		inverse_transpose_MV = glm::transpose(inverse_MV);
		glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, glm::value_ptr(MV->topMatrix()));
		glUniformMatrix4fv(prog->getUniform("IT_MV"), 1, GL_FALSE, glm::value_ptr(inverse_transpose_MV));
		box->draw(prog);
		MV->popMatrix();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glUniform3f(prog->getUniform("lightPos"), the_light.getPos()[0], the_light.getPos()[1], the_light.getPos()[2]);
	glUniform3f(prog->getUniform("lightCol"), the_light.getColor()[0], the_light.getColor()[1], the_light.getColor()[2]);
	glUniform3f(prog->getUniform("ka"), the_material.getKa()[0], the_material.getKa()[1], the_material.getKa()[2]);
	glUniform3f(prog->getUniform("kd"), the_material.getKd()[0], the_material.getKd()[1], the_material.getKd()[2]);
	glUniform3f(prog->getUniform("ks"), the_material.getKs()[0], the_material.getKs()[1], the_material.getKs()[2]);
	glUniform1f(prog->getUniform("s"), the_material.getS());
	prog->unbind();
	
	MV->popMatrix();
	P->popMatrix();
	


	GLSL::checkError(GET_FILE_LINE);
}

int main(int argc, char **argv) {
	if(argc < 2) {
		cout << "Usage: MinBox RESOURCE_DIR" << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Minimal Enclosing Boxes", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	GLSL::checkVersion();
	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Set char callback.
	glfwSetCharCallback(window, char_callback);
	// Set cursor position callback.
	glfwSetCursorPosCallback(window, cursor_position_callback);
	// Set mouse button callback.
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	// Set the window resize call back.
	glfwSetFramebufferSizeCallback(window, resize_callback);
	// Initialize scene.
	init();

	cout << "Usage" << endl;
	cout << "W: toggle Vertex Mode" << endl;
	cout << "A: toggle Bounding Box" << endl;
	cout << "S: switch shader" << endl;
	cout << "D: switch object" << endl;
	cout << "Space: toggle spinning" << endl << endl << endl;

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
