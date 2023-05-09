#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
    movX_helicoptero = 0.0f;
    movY_helicoptero = 0.0f;
    movZ_helicoptero = 0.0f;
    movimientoXAuto = 0.0f;
    movimientoZAuto = 0.0f;
    direccionAuto = -1.0f;
    
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	// Inicializaci�n de GLFW
	if (!glfwInit())
	{
		printf("Fallo inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	// Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	// para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Practica 9 - Christian Leyva", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	// Obtener tama�o de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	// MANEJAR TECLADO y MOUSE
	createCallbacks();

	// Se oculta el mouse
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fallo inicializacion de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); // HABILITAR BUFFER DE PROFUNDIDAD
							 //  Asignar valores de la ventana y coordenadas

	// Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	// Callback para detectar que se est� usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::ManejaTeclado(GLFWwindow *window, int key, int code, int action, int mode)
{
	Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
    
    // AUTO -------------------
	// Movimiento X del auto
	if (key == GLFW_KEY_V)
	{
		theWindow->movimientoXAuto += 10.0;
		theWindow->mov_ruedas += 5.0;
		theWindow->direccionAuto = 1.0f; // Coloca la luz mirando hacia adelante
	}
	if (key == GLFW_KEY_B)
	{
		theWindow->movimientoXAuto -= 10.0;
		theWindow->mov_ruedas -= 5.0;
		theWindow->direccionAuto = -1.0f; // Coloca la luz mirando hacia atras
	}
	// Movimiento Z del auto
	if (key == GLFW_KEY_N)
	{
		theWindow->movimientoZAuto += 10.0;
		theWindow->mov_ruedas += 5.0;
	}
	if (key == GLFW_KEY_M)
	{
		theWindow->movimientoZAuto -= 10.0;
		theWindow->mov_ruedas -= 5.0;
	}
    
    // HELICOPTERO ---------------
	// Movimiento X del helicoptero
	if (key == GLFW_KEY_R)
	{
		theWindow->movX_helicoptero += 5.0;
	}
	if (key == GLFW_KEY_T)
	{
		theWindow->movX_helicoptero -= 5.0;
	}
	// Movimiento Y del helicoptero
	if (key == GLFW_KEY_Y)
	{
		theWindow->movY_helicoptero += 5.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow->movY_helicoptero -= 5.0;
	}
	// Movimiento Z del helicoptero
	if (key == GLFW_KEY_I)
	{
		theWindow->movZ_helicoptero += 5.0;
	}
	if (key == GLFW_KEY_O)
	{
		theWindow->movZ_helicoptero -= 5.0;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			// printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			// printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow *window, double xPos, double yPos)
{
	Window *theWindow = static_cast<Window *>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}
