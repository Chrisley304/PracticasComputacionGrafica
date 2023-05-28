#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
	bool getShouldClose()
	{
		return glfwWindowShouldClose(mainWindow);
	}
	bool *getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	GLfloat getMovimientoXAuto() { return movimientoXAuto; }
	GLfloat getDireccionAuto() { return direccionAuto; }
	GLfloat getMovimientoZAuto() { return movimientoZAuto; }
	GLfloat getMovimientoRuedas() { return mov_ruedas; }
	GLfloat isHelicopteroEncendido() { return helicopteroEncendido; }
	void alternHelicopteroEncendido() { helicopteroEncendido = !helicopteroEncendido; }
	void toggleAutoManual()
	{
		autoManual = !autoManual;
	}
	bool isAutoManual() { return autoManual; }
	bool resetCarro() { return resetCar; }
	void setResetFlag(bool val) { resetCar = val; }
	bool getIniciaAnimacion() { return playAnimacion; }
	void toggleplayAnimacion() { playAnimacion = !playAnimacion; }
	bool getMovimientoXHelicoptero() { return movX_helicoptero; }
	bool getMovimientoYHelicoptero() { return movY_helicoptero; }
	bool getGiroHelicoptero() { return movGiro_helicoptero; }
	void toogleMovimientoXHelicoptero() { movX_helicoptero = !movX_helicoptero; }
	void toogleMovimientoYHelicoptero() { movY_helicoptero = !movY_helicoptero; }
	void toogleGiroHelicoptero() { movGiro_helicoptero = !movGiro_helicoptero; }
	bool getMovimientoXNegHelicoptero() { return movXNeg_helicoptero; }
	bool getMovimientoYNegHelicoptero() { return movYNeg_helicoptero; }
	bool getGiroNegHelicoptero() { return movGiroNeg_helicoptero; }
	void toogleMovimientoXNegHelicoptero() { movXNeg_helicoptero = !movXNeg_helicoptero; }
	void toogleMovimientoYNegHelicoptero() { movYNeg_helicoptero = !movYNeg_helicoptero; }
	void toogleGiroZNegHelicoptero() { movGiroNeg_helicoptero = !movGiroNeg_helicoptero; }

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
	bool mov_ruedas;
	bool helicopteroEncendido;
	bool autoManual;
	bool resetCar;
	bool playAnimacion;
	bool movX_helicoptero;
	bool movY_helicoptero;
	bool movGiro_helicoptero;
	bool movXNeg_helicoptero;
	bool movYNeg_helicoptero;
	bool movGiroNeg_helicoptero;

	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow *window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow *window, double xPos, double yPos);
};
