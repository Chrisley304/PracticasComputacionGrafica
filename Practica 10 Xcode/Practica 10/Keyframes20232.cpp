/*
Semestre 2023-2
Animaci�n:
1.- Simple o b�sica:Por banderas y condicionales
2.- Compleja: Por medio de funciones y algoritmos,Textura Animada.
3.- T�cnicas de Animaci�n: Por Keyframes
Adicional.- Texturizado con transparencia usando Blending: Requerimos dibujar lo que est� atras primero

*/
// para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// para probar el importer
// #include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

// para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

// variables para animaci�n
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetu = 0.0f;
float toffsetv = 0.0f;
float reproduciranimacion, habilitaranimacion,
	guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;

Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;

Model Rampa;
Model Skateboard;

Skybox skybox;

// materiales
Material Material_brillante;
Material Material_opaco;

// Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
// para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char *vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char *fShader = "shaders/shader_light.frag";

// PARA INPUT CON KEYFRAMES
void inputKeyframes(bool *keys);

// c�lculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int *indices, unsigned int indiceCount, GLfloat *vertices, unsigned int verticeCount,
						unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;
		vertices[in0] += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x;
		vertices[nOffset + 1] = vec.y;
		vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
		-1.0f, -1.0f, -0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f, 0.0f, 10.0f, 0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f, 10.0f, 10.0f, 0.0f, -1.0f, 0.0f};
	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4, 5, 6,
		4, 6, 7};

	GLfloat vegetacionVertices[] = {
		-0.5f,
		-0.5f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.5f,
		-0.5f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.5f,
		0.5f,
		0.0f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		-0.5f,
		0.5f,
		0.0f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,

		0.0f,
		-0.5f,
		-0.5f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		-0.5f,
		0.5f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.5f,
		0.5f,
		1.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,
		0.0f,
		0.5f,
		-0.5f,
		0.0f,
		1.0f,
		0.0f,
		0.0f,
		0.0f,

	};

	unsigned int flechaIndices[] = {
		0,
		1,
		2,
		0,
		2,
		3,
	};

	GLfloat flechaVertices[] = {
		-0.5f,
		0.0f,
		0.5f,
		0.0f,
		0.0f,
		0.0f,
		-1.0f,
		0.0f,
		0.5f,
		0.0f,
		0.5f,
		1.0f,
		0.0f,
		0.0f,
		-1.0f,
		0.0f,
		0.5f,
		0.0f,
		-0.5f,
		1.0f,
		1.0f,
		0.0f,
		-1.0f,
		0.0f,
		-0.5f,
		0.0f,
		-0.5f,
		0.0f,
		1.0f,
		0.0f,
		-1.0f,
		0.0f,

	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh *obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);
}

void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

///////////////////////////////KEYFRAMES/////////////////////

bool animacion = false;

// NEW// Keyframes
float posXavion = 0.0, posYavion = 0.0, posZavion = 20.0;
float movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;

#define MAX_FRAMES 30
int i_max_steps = 90; // Se deja en 90 porque no da problema, pero si se llega a laggear o ver fea la animaci�n, hay que bajar este valor.
int i_curr_steps = 16;
typedef struct _frame
{
	// Variables para GUARDAR Key Frames
	float movAvion_x;	 // Variable para PosicionX
	float movAvion_y;	 // Variable para PosicionY
	float movAvion_xInc; // Variable para IncrementoX
	float movAvion_yInc; // Variable para IncrementoY

	float giroAvion;
	float giroAvionInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 16; // introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion = giroAvion;

	FrameIndex++;
}

void resetElements(void)
{

	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;
}

void animate(void)
{
	// Movimiento del objeto
	if (play)
	{
		if (i_curr_steps >= i_max_steps) // end of animation between frames?
		{
			playIndex++;
			printf("playindex : %d\n", playIndex);
			if (playIndex > FrameIndex - 2) // end of total animation?
			{
				printf("Frame index= %d\n", FrameIndex);
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else // Next frame interpolations
			{
				// printf("entro aqu�\n");
				i_curr_steps = 0; // Reset counter
				// Interpolation
				interpolation();
			}
		}
		else
		{
			// printf("se qued� aqui\n");
			// printf("max steps: %f", i_max_steps);
			// Draw animation
			movAvion_x += KeyFrame[playIndex].movAvion_xInc;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			i_curr_steps++;
		}
	}
}

/* FIN KEYFRAMES*/

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();
    pisoTexture = Texture("Textures/piso.tga");
    pisoTexture.LoadTextureA();
    Rampa = Model();
    Rampa.LoadModel("Models/rampa.obj");
    Skateboard = Model();
    Skateboard.LoadModel("Models/skateboard_mrbeast.obj");
    

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	// luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
								 0.3f, 0.3f,
								 0.0f, 0.0f, -1.0f);
	// contador de luces puntuales
	unsigned int pointLightCount = 0;
	// Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
								0.0f, 1.0f,
								0.0f, 2.5f, 1.5f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	// linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
							  0.0f, 2.0f,
							  0.0f, 0.0f, 0.0f,
							  0.0f, -1.0f, 0.0f,
							  1.0f, 0.0f, 0.0f,
							  5.0f);
	spotLightCount++;

	// luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
							  1.0f, 2.0f,
							  5.0f, 10.0f, 0.0f,
							  0.0f, -5.0f, 0.0f,
							  1.0f, 0.0f, 0.0f,
							  15.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		   uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 5.0f;
	avanza = true;

	glm::vec3 posSkate = glm::vec3(2.0f, 0.0f, 0.0f);

	// KEYFRAMES DECLARADOS INICIALES
	// Movimiento senoidal del avion hasta llegar a X = 300, despues da vuelta en 180° y regresa a X = 0

    KeyFrame[0].movAvion_x = 0.0f;
    KeyFrame[0].movAvion_y = 0.0f;
    KeyFrame[0].giroAvion = 0;
    
    KeyFrame[1].movAvion_x = -24.0f;
    KeyFrame[1].movAvion_y = 0.0f;
    KeyFrame[1].giroAvion = 0;
    
    KeyFrame[2].movAvion_x = -28.0f;
    KeyFrame[2].movAvion_y = 0.2f;
    KeyFrame[2].giroAvion = 0.2;
    
    KeyFrame[3].movAvion_x = -33.0f;
    KeyFrame[3].movAvion_y = 1.2f;
    KeyFrame[3].giroAvion = 18.7;
    
    KeyFrame[4].movAvion_x = -36.0f;
    KeyFrame[4].movAvion_y = 2.3f;
    KeyFrame[4].giroAvion = 28.7;
    
    KeyFrame[5].movAvion_x = -39.0f;
    KeyFrame[5].movAvion_y = 3.4f;
    KeyFrame[5].giroAvion = 38.7;
    
    KeyFrame[6].movAvion_x = -41.0f;
    KeyFrame[6].movAvion_y = 5.5f;
    KeyFrame[6].giroAvion = 40.7;
    
    KeyFrame[7].movAvion_x = -43.0f;
    KeyFrame[7].movAvion_y = 7.5f;
    KeyFrame[7].giroAvion = 40.7;
    
    KeyFrame[8].movAvion_x = -45.0f;
    KeyFrame[8].movAvion_y = 9.5f;
    KeyFrame[8].giroAvion = 52.7;

    KeyFrame[9].movAvion_x = -46.0f;
    KeyFrame[9].movAvion_y = 11.5f;
    KeyFrame[9].giroAvion = 54.7;

    KeyFrame[10].movAvion_x = -47.1f;
    KeyFrame[10].movAvion_y = 13.5f;
    KeyFrame[10].giroAvion = 58.7;

    KeyFrame[11].movAvion_x = -48.f;
    KeyFrame[11].movAvion_y = 15.5f;
    KeyFrame[11].giroAvion = 62.7;

    KeyFrame[12].movAvion_x = -49.f;
    KeyFrame[12].movAvion_y = 17.5f;
    KeyFrame[12].giroAvion = 74.7;

    KeyFrame[13].movAvion_x = -49.f;
    KeyFrame[13].movAvion_y = 30.5f;
    KeyFrame[13].giroAvion = 80.7;

    KeyFrame[14].movAvion_x = -51.f;
    KeyFrame[14].movAvion_y = 31.5f;
    KeyFrame[14].giroAvion = 48.7;

	KeyFrame[15].movAvion_x = -55.f;
	KeyFrame[15].movAvion_y = 21.f;
	KeyFrame[15].giroAvion = 0.0;
	
	// Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		if (avanza)
		{
			if (movCoche < 301.0f)
			{
				movCoche -= movOffset * deltaTime;
				// printf("avanza%f \n ",movCoche);
			}
			if (movCoche < -300.0f)
			{
				avanza = false;
			}
		}
		else
		{
			if (movCoche < 300.0f)
			{
				movCoche += movOffset * deltaTime;
			}
			else
			{
				// printf("entro");
				avanza = true;
			}
		}

		rotllanta += rotllantaOffset * deltaTime;

		// Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// para keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		// informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la c�mara de tipo flash
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		// informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

        model = glm::mat4(1.0);
        posSkate = glm::vec3(posXavion, posYavion + movAvion_y, posZavion + movAvion_x); // Nota, se va a usar el movx en z, para simplificar el codigo y porque los tengo acomodados de esta manera.
        model = glm::translate(model, posSkate);
        model = glm::translate(model, glm::vec3(0.f, -1.9f, 0.f));
        model = glm::scale(model, glm::vec3(6.f, 6.f, 6.f));
        model = glm::rotate(model, giroAvion * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Skateboard.RenderModel();

        
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.f, -1.8f, -30.f));
		model = glm::scale(model, glm::vec3(6.f, 6.f, 6.f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Rampa.RenderModel();

//		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}

void inputKeyframes(bool *keys)
{
	if (mainWindow.getIniciaAnimacion())
	{
		if (play == false && (FrameIndex > 1))
		{
			resetElements();
			// First Interpolation
			interpolation();
			play = true;
			playIndex = 0;
			i_curr_steps = 0;
			printf("\n Presiona ESPACIO para reproducir de nuevo la animacion\n");
			habilitaranimacion = 0;
		}
		else
		{
			play = false;
		}
		mainWindow.toggleplayAnimacion();
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			// printf("movAvion_y es: %f\n", movAvion_y);
			printf(" \npresiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}

	if (keys[GLFW_KEY_P])
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
		}
	}

	// Traslación en +X
	if (mainWindow.getMovimientoXHelicoptero())
	{
		movAvion_x += 1.0f;
		printf("\n movAvion_x es: %f\n", movAvion_x);
		mainWindow.toogleMovimientoXHelicoptero();
	}

	// Traslacion en +Y
	if (mainWindow.getMovimientoYHelicoptero())
	{
		movAvion_y += 1.0f;
		printf("\n movAvion_y es: %f\n", movAvion_y);
		mainWindow.toogleMovimientoYHelicoptero();
	}

	// Traslacion en +Z
	if (mainWindow.getGiroHelicoptero())
	{
		giroAvion += 1.0f;
		printf("\n giroAvion es: %f\n", giroAvion);
		mainWindow.toogleGiroHelicoptero();
	}

	// Traslacion en -X
	if (mainWindow.getMovimientoXNegHelicoptero())
	{
		movAvion_x -= 1.0f;
		printf("\n movAvion_x es: %f\n", movAvion_x);
		mainWindow.toogleMovimientoXNegHelicoptero();
	}

	// Traslacion en -Y
	if (mainWindow.getMovimientoYNegHelicoptero())
	{
		movAvion_y -= 1.0f;
		printf("\n movAvion_y es: %f\n", movAvion_y);
		mainWindow.toogleMovimientoYNegHelicoptero();
	}

	// Traslacion en -Z
	if (mainWindow.getGiroNegHelicoptero())
	{
		giroAvion -= 1.0f;
		printf("\n giroAvion es: %f\n", giroAvion);
		mainWindow.toogleGiroZNegHelicoptero();
	}

	//	if (keys[GLFW_KEY_2])
	//	{
	//		if (ciclo2 < 1)
	//		{
	//			ciclo = 0;
	//		}
	//	}
}
