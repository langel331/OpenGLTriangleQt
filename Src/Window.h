#pragma once
#include <QtOpenGL\qglwidget>
#include <QtGui\qkeyevent>

class Window : public QGLWidget {
public:

	bool checkShaderStatus(GLuint shaderID);
	bool checkProgramStatus(GLuint programID);
	void installShaders();
	void sendDatatoOpenGL();
protected:
	virtual void initializeGL();
	virtual void paintGL();
};