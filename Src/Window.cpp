#include <GL\glew.h>
#include <fstream>
#include "Window.h"
#include "Point.h"
#include <iostream>
#include <glm\glm.hpp>
#include "Primatives\Vertex.h"

const uint NUM_VERTICES_PER_TRI = 3;
const uint NUM_FLOATS_PER_VERTICE = 6;
GLuint vao;
GLuint programID;
const uint VERTEX_BYTE_SIZE = NUM_FLOATS_PER_VERTICE * sizeof(float);

void Window::sendDatatoOpenGL(){	
	
	Vertex myTri[] =
	{
		glm::vec3(0.0f, 1.0f, +0.0f),
		glm::vec3(+1.0f, +0.0f, +0.0f),

		glm::vec3(-1.0f, -1.0f, 0.0f),
		glm::vec3(+0.0f, +1.0f, +0.0f),

		glm::vec3(1.0f, -1.0f, +0.0f),
		glm::vec3(+0.0f, +0.0f, +1.0f),
	};

	//Vertex array object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//declare vertex buffer Id
	GLuint vertexBufferID;
	//Create vertex buffer
	glGenBuffers(1, &vertexBufferID);
	//Bind vertex buffer to vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	//Define which buffer to bind to vertex array
	glBufferData(GL_ARRAY_BUFFER, sizeof(myTri), myTri, GL_STATIC_DRAW);
	//enable vertex position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	//Describe type  of data to OpenGL (0 = position attribute, 3 = # of position floats, sizeof(float) * 3 = stride to next element)
	//(void*)(myTri+sizeof(glm::vec3))
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
	//enable vertex color
	glEnableVertexAttribArray(1);
	

	// Index array of control points on a curve (store each vertex as an index)
	GLushort indices[] = { 0, 1, 2};
	// declare index buffer Id
	GLuint indexBufferID;
	//Create index buffer
	glGenBuffers(1, &indexBufferID);
	//Bind index buffer to points elements
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	//Define which buffer to bind to index array to
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

std::string readShaderCode(const char*fileName)
{
	//read shader file
	std::ifstream meInput(fileName);

	//throws error if failed to load
	if (!meInput.good())
	{
		std::cout << "File failed to load " << fileName;
		exit(1);
	}
	return std::string(std::istreambuf_iterator<char>(meInput), std::istreambuf_iterator<char>());
}

bool checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetterFunc, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType){
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;
		delete[] buffer;
		return false;
	}
	return true;
}

bool Window::checkShaderStatus(GLuint shaderID){
	return checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool Window::checkProgramStatus(GLuint programID){
	return checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

void Window::installShaders(){
	//create vertex shader object
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	//create fragment shader object
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


	//assign source files to corresponding shaders
	const char* adapter[1];
	std::string temp = readShaderCode("VertexShaderCode.glsl");
	adapter[0] = temp.c_str();
	glShaderSource(vertexShaderID, 1, adapter, 0);
	std::string temp1 = readShaderCode("FragmentShaderCode.glsl");
	adapter[0] = temp1.c_str();
	glShaderSource(fragmentShaderID, 1, adapter, 0);

	//compile shaders
	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	//create program to link shaders
	programID = glCreateProgram();
	//attach shaders to program
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	//link program
	glLinkProgram(programID);
	//check for linking failture
	if (!checkProgramStatus(programID))
		return;

	//install shaders
	glUseProgram(programID);

	//checks if either shaders failed to copmpile
	if (!checkShaderStatus(vertexShaderID) || !checkShaderStatus(fragmentShaderID)){
		std:: cout <<"shader compile error";
		return;
	}


}


void  Window::initializeGL(){
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glewExperimental = true;
	glewInit();
	std::cout << "OpenGL version " << glGetString(GL_VERSION) << "\n";
	//glEnable(GL_DEPTH_TEST);
	sendDatatoOpenGL();
	installShaders();
}

void Window::paintGL() {
	std::cout << "drawing ";
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, width(), height());
	glUseProgram(programID);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	update();
}
