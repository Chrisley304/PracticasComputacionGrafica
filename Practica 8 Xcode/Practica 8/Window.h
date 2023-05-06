#pragma once
#include<stdio.h>
#include<GL/glew.h>
#include<GLFW/glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
    GLfloat getMovimientoXAuto() { return movimientoXAuto; }
    GLfloat getDireccionAuto() { return direccionAuto; }
    GLfloat getMovimientoZAuto() { return movimientoZAuto; }
    GLfloat getMovimientoXHelicoptero() { return movX_helicoptero; }
    GLfloat getMovimientoYHelicoptero() { return movY_helicoptero; }
    GLfloat getMovimientoZHelicoptero() { return movZ_helicoptero; }
    GLfloat getMovimientoRuedas() { return mov_ruedas; }
    
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
    GLfloat movimientoXAuto;
    GLfloat direccionAuto;
    GLfloat movimientoZAuto;
    GLfloat movX_helicoptero;
    GLfloat movY_helicoptero;
    GLfloat movZ_helicoptero;
    GLfloat mov_ruedas;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

