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
float posXavion = 2.0, posYavion = 5.0, posZavion = -3.0;
float movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;

#define MAX_FRAMES 30
int i_max_steps = 90; // Se deja en 90 porque no da problema, pero si se llega a laggear o ver fea la animaci�n, hay que bajar este valor.
int i_curr_steps = 123;
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
int FrameIndex = 123; // introducir datos
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
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");

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

	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);

	// KEYFRAMES DECLARADOS INICIALES
	// Movimiento senoidal del avion hasta llegar a X = 300, despues da vuelta en 180° y regresa a X = 0

	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].giroAvion = 0;

	KeyFrame[1].movAvion_x = 5.0f;
	KeyFrame[1].movAvion_y = 10.0f;
	KeyFrame[1].giroAvion = 0;

	KeyFrame[2].movAvion_x = 10.0f;
	KeyFrame[2].movAvion_y = 0.0f;
	KeyFrame[2].giroAvion = 0;

	KeyFrame[3].movAvion_x = 15.0f;
	KeyFrame[3].movAvion_y = -10.0f;
	KeyFrame[3].giroAvion = 0;

	KeyFrame[4].movAvion_x = 20.0f;
	KeyFrame[4].movAvion_y = 0.0f;
	KeyFrame[4].giroAvion = 0;

	KeyFrame[5].movAvion_x = 25.0f;
	KeyFrame[5].movAvion_y = 10.0f;
	KeyFrame[5].giroAvion = 0;

	KeyFrame[6].movAvion_x = 30.0f;
	KeyFrame[6].movAvion_y = 0.0f;
	KeyFrame[6].giroAvion = 0;

	KeyFrame[7].movAvion_x = 35.0f;
	KeyFrame[7].movAvion_y = -10.0f;
	KeyFrame[7].giroAvion = 0;

	KeyFrame[8].movAvion_x = 40.0f;
	KeyFrame[8].movAvion_y = 0.0f;
	KeyFrame[8].giroAvion = 0;

	KeyFrame[9].movAvion_x = 45.0f;
	KeyFrame[9].movAvion_y = 10.0f;
	KeyFrame[9].giroAvion = 0;

	KeyFrame[10].movAvion_x = 50.0f;
	KeyFrame[10].movAvion_y = 0.0f;
	KeyFrame[10].giroAvion = 0;

	KeyFrame[11].movAvion_x = 55.0f;
	KeyFrame[11].movAvion_y = -10.0f;
	KeyFrame[11].giroAvion = 0;

	KeyFrame[12].movAvion_x = 60.0f;
	KeyFrame[12].movAvion_y = 0.0f;
	KeyFrame[12].giroAvion = 0;

	KeyFrame[13].movAvion_x = 65.0f;
	KeyFrame[13].movAvion_y = 10.0f;
	KeyFrame[13].giroAvion = 0;

	KeyFrame[14].movAvion_x = 70.0f;
	KeyFrame[14].movAvion_y = 0.0f;
	KeyFrame[14].giroAvion = 0;

	KeyFrame[15].movAvion_x = 75.0f;
	KeyFrame[15].movAvion_y = -10.0f;
	KeyFrame[15].giroAvion = 0;

	KeyFrame[16].movAvion_x = 80.0f;
	KeyFrame[16].movAvion_y = 0.0f;
	KeyFrame[16].giroAvion = 0;

	KeyFrame[17].movAvion_x = 85.0f;
	KeyFrame[17].movAvion_y = 10.0f;
	KeyFrame[17].giroAvion = 0;

	KeyFrame[18].movAvion_x = 90.0f;
	KeyFrame[18].movAvion_y = 0.0f;
	KeyFrame[18].giroAvion = 0;

	KeyFrame[19].movAvion_x = 95.0f;
	KeyFrame[19].movAvion_y = -10.0f;
	KeyFrame[19].giroAvion = 0;

	KeyFrame[20].movAvion_x = 100.0f;
	KeyFrame[20].movAvion_y = 0.0f;
	KeyFrame[20].giroAvion = 0;

	KeyFrame[21].movAvion_x = 105.0f;
	KeyFrame[21].movAvion_y = 10.0f;
	KeyFrame[21].giroAvion = 0;

	KeyFrame[22].movAvion_x = 110.0f;
	KeyFrame[22].movAvion_y = 0.0f;
	KeyFrame[22].giroAvion = 0;

	KeyFrame[23].movAvion_x = 115.0f;
	KeyFrame[23].movAvion_y = -10.0f;
	KeyFrame[23].giroAvion = 0;

	KeyFrame[24].movAvion_x = 120.0f;
	KeyFrame[24].movAvion_y = 0.0f;
	KeyFrame[24].giroAvion = 0;

	KeyFrame[25].movAvion_x = 125.0f;
	KeyFrame[25].movAvion_y = 10.0f;
	KeyFrame[25].giroAvion = 0;

	KeyFrame[26].movAvion_x = 130.0f;
	KeyFrame[26].movAvion_y = 0.0f;
	KeyFrame[26].giroAvion = 0;

	KeyFrame[27].movAvion_x = 135.0f;
	KeyFrame[27].movAvion_y = -10.0f;
	KeyFrame[27].giroAvion = 0;

	KeyFrame[28].movAvion_x = 140.0f;
	KeyFrame[28].movAvion_y = 0.0f;
	KeyFrame[28].giroAvion = 0;

	KeyFrame[29].movAvion_x = 145.0f;
	KeyFrame[29].movAvion_y = 10.0f;
	KeyFrame[29].giroAvion = 0;

	KeyFrame[30].movAvion_x = 150.0f;
	KeyFrame[30].movAvion_y = 0.0f;
	KeyFrame[30].giroAvion = 0;

	KeyFrame[31].movAvion_x = 155.0f;
	KeyFrame[31].movAvion_y = -10.0f;
	KeyFrame[31].giroAvion = 0;

	KeyFrame[32].movAvion_x = 160.0f;
	KeyFrame[32].movAvion_y = 0.0f;
	KeyFrame[32].giroAvion = 0;

	KeyFrame[33].movAvion_x = 165.0f;
	KeyFrame[33].movAvion_y = 10.0f;
	KeyFrame[33].giroAvion = 0;

	KeyFrame[34].movAvion_x = 170.0f;
	KeyFrame[34].movAvion_y = 0.0f;
	KeyFrame[34].giroAvion = 0;

	KeyFrame[35].movAvion_x = 175.0f;
	KeyFrame[35].movAvion_y = -10.0f;
	KeyFrame[35].giroAvion = 0;

	KeyFrame[36].movAvion_x = 180.0f;
	KeyFrame[36].movAvion_y = 0.0f;
	KeyFrame[36].giroAvion = 0;

	KeyFrame[37].movAvion_x = 185.0f;
	KeyFrame[37].movAvion_y = 10.0f;
	KeyFrame[37].giroAvion = 0;

	KeyFrame[38].movAvion_x = 190.0f;
	KeyFrame[38].movAvion_y = 0.0f;
	KeyFrame[38].giroAvion = 0;

	KeyFrame[39].movAvion_x = 195.0f;
	KeyFrame[39].movAvion_y = -10.0f;
	KeyFrame[39].giroAvion = 0;

	KeyFrame[40].movAvion_x = 200.0f;
	KeyFrame[40].movAvion_y = 0.0f;
	KeyFrame[40].giroAvion = 0;

	KeyFrame[41].movAvion_x = 205.0f;
	KeyFrame[41].movAvion_y = 10.0f;
	KeyFrame[41].giroAvion = 0;

	KeyFrame[42].movAvion_x = 210.0f;
	KeyFrame[42].movAvion_y = 0.0f;
	KeyFrame[42].giroAvion = 0;

	KeyFrame[43].movAvion_x = 215.0f;
	KeyFrame[43].movAvion_y = -10.0f;
	KeyFrame[43].giroAvion = 0;

	KeyFrame[44].movAvion_x = 220.0f;
	KeyFrame[44].movAvion_y = 0.0f;
	KeyFrame[44].giroAvion = 0;

	KeyFrame[45].movAvion_x = 225.0f;
	KeyFrame[45].movAvion_y = 10.0f;
	KeyFrame[45].giroAvion = 0;

	KeyFrame[46].movAvion_x = 230.0f;
	KeyFrame[46].movAvion_y = 0.0f;
	KeyFrame[46].giroAvion = 0;

	KeyFrame[47].movAvion_x = 235.0f;
	KeyFrame[47].movAvion_y = -10.0f;
	KeyFrame[47].giroAvion = 0;

	KeyFrame[48].movAvion_x = 240.0f;
	KeyFrame[48].movAvion_y = 0.0f;
	KeyFrame[48].giroAvion = 0;

	KeyFrame[49].movAvion_x = 245.0f;
	KeyFrame[49].movAvion_y = 10.0f;
	KeyFrame[49].giroAvion = 0;

	KeyFrame[50].movAvion_x = 250.0f;
	KeyFrame[50].movAvion_y = 0.0f;
	KeyFrame[50].giroAvion = 0;

	KeyFrame[51].movAvion_x = 255.0f;
	KeyFrame[51].movAvion_y = -10.0f;
	KeyFrame[51].giroAvion = 0;

	KeyFrame[52].movAvion_x = 260.0f;
	KeyFrame[52].movAvion_y = 0.0f;
	KeyFrame[52].giroAvion = 0;

	KeyFrame[53].movAvion_x = 265.0f;
	KeyFrame[53].movAvion_y = 10.0f;
	KeyFrame[53].giroAvion = 0;

	KeyFrame[54].movAvion_x = 270.0f;
	KeyFrame[54].movAvion_y = 0.0f;
	KeyFrame[54].giroAvion = 0;

	KeyFrame[55].movAvion_x = 275.0f;
	KeyFrame[55].movAvion_y = -10.0f;
	KeyFrame[55].giroAvion = 0;

	KeyFrame[56].movAvion_x = 280.0f;
	KeyFrame[56].movAvion_y = 0.0f;
	KeyFrame[56].giroAvion = 0;

	KeyFrame[57].movAvion_x = 285.0f;
	KeyFrame[57].movAvion_y = 10.0f;
	KeyFrame[57].giroAvion = 0;

	KeyFrame[58].movAvion_x = 290.0f;
	KeyFrame[58].movAvion_y = 0.0f;
	KeyFrame[58].giroAvion = 0;

	KeyFrame[59].movAvion_x = 295.0f;
	KeyFrame[59].movAvion_y = -10.0f;
	KeyFrame[59].giroAvion = 0;

	KeyFrame[60].movAvion_x = 300.0f;
	KeyFrame[60].movAvion_y = 0.0f;
	KeyFrame[60].giroAvion = 0;

	KeyFrame[61].movAvion_x = 300.0f;
	KeyFrame[61].movAvion_y = 0.0f;
	KeyFrame[61].giroAvion = 180.0f;

	KeyFrame[62].movAvion_x = 300.0f;
	KeyFrame[62].movAvion_y = 0.0f;
	KeyFrame[62].giroAvion = 180;

	KeyFrame[63].movAvion_x = 295.0f;
	KeyFrame[63].movAvion_y = -10.0f;
	KeyFrame[63].giroAvion = 180;

	KeyFrame[64].movAvion_x = 290.0f;
	KeyFrame[64].movAvion_y = 0.0f;
	KeyFrame[64].giroAvion = 180;

	KeyFrame[65].movAvion_x = 285.0f;
	KeyFrame[65].movAvion_y = 10.0f;
	KeyFrame[65].giroAvion = 180;

	KeyFrame[66].movAvion_x = 280.0f;
	KeyFrame[66].movAvion_y = 0.0f;
	KeyFrame[66].giroAvion = 180;

	KeyFrame[67].movAvion_x = 275.0f;
	KeyFrame[67].movAvion_y = -10.0f;
	KeyFrame[67].giroAvion = 180;

	KeyFrame[68].movAvion_x = 270.0f;
	KeyFrame[68].movAvion_y = 0.0f;
	KeyFrame[68].giroAvion = 180;

	KeyFrame[69].movAvion_x = 265.0f;
	KeyFrame[69].movAvion_y = 10.0f;
	KeyFrame[69].giroAvion = 180;

	KeyFrame[70].movAvion_x = 260.0f;
	KeyFrame[70].movAvion_y = 0.0f;
	KeyFrame[70].giroAvion = 180;

	KeyFrame[71].movAvion_x = 255.0f;
	KeyFrame[71].movAvion_y = -10.0f;
	KeyFrame[71].giroAvion = 180;

	KeyFrame[72].movAvion_x = 250.0f;
	KeyFrame[72].movAvion_y = 0.0f;
	KeyFrame[72].giroAvion = 180;

	KeyFrame[73].movAvion_x = 245.0f;
	KeyFrame[73].movAvion_y = 10.0f;
	KeyFrame[73].giroAvion = 180;

	KeyFrame[74].movAvion_x = 240.0f;
	KeyFrame[74].movAvion_y = 0.0f;
	KeyFrame[74].giroAvion = 180;

	KeyFrame[75].movAvion_x = 235.0f;
	KeyFrame[75].movAvion_y = -10.0f;
	KeyFrame[75].giroAvion = 180;

	KeyFrame[76].movAvion_x = 230.0f;
	KeyFrame[76].movAvion_y = 0.0f;
	KeyFrame[76].giroAvion = 180;

	KeyFrame[77].movAvion_x = 225.0f;
	KeyFrame[77].movAvion_y = 10.0f;
	KeyFrame[77].giroAvion = 180;

	KeyFrame[78].movAvion_x = 220.0f;
	KeyFrame[78].movAvion_y = 0.0f;
	KeyFrame[78].giroAvion = 180;

	KeyFrame[79].movAvion_x = 215.0f;
	KeyFrame[79].movAvion_y = -10.0f;
	KeyFrame[79].giroAvion = 180;

	KeyFrame[80].movAvion_x = 210.0f;
	KeyFrame[80].movAvion_y = 0.0f;
	KeyFrame[80].giroAvion = 180;

	KeyFrame[81].movAvion_x = 205.0f;
	KeyFrame[81].movAvion_y = 10.0f;
	KeyFrame[81].giroAvion = 180;

	KeyFrame[82].movAvion_x = 200.0f;
	KeyFrame[82].movAvion_y = 0.0f;
	KeyFrame[82].giroAvion = 180;

	KeyFrame[83].movAvion_x = 195.0f;
	KeyFrame[83].movAvion_y = -10.0f;
	KeyFrame[83].giroAvion = 180;

	KeyFrame[84].movAvion_x = 190.0f;
	KeyFrame[84].movAvion_y = 0.0f;
	KeyFrame[84].giroAvion = 180;

	KeyFrame[85].movAvion_x = 185.0f;
	KeyFrame[85].movAvion_y = 10.0f;
	KeyFrame[85].giroAvion = 180;

	KeyFrame[86].movAvion_x = 180.0f;
	KeyFrame[86].movAvion_y = 0.0f;
	KeyFrame[86].giroAvion = 180;

	KeyFrame[87].movAvion_x = 175.0f;
	KeyFrame[87].movAvion_y = -10.0f;
	KeyFrame[87].giroAvion = 180;

	KeyFrame[88].movAvion_x = 170.0f;
	KeyFrame[88].movAvion_y = 0.0f;
	KeyFrame[88].giroAvion = 180;

	KeyFrame[89].movAvion_x = 165.0f;
	KeyFrame[89].movAvion_y = 10.0f;
	KeyFrame[89].giroAvion = 180;

	KeyFrame[90].movAvion_x = 160.0f;
	KeyFrame[90].movAvion_y = 0.0f;
	KeyFrame[90].giroAvion = 180;

	KeyFrame[91].movAvion_x = 155.0f;
	KeyFrame[91].movAvion_y = -10.0f;
	KeyFrame[91].giroAvion = 180;

	KeyFrame[92].movAvion_x = 150.0f;
	KeyFrame[92].movAvion_y = 0.0f;
	KeyFrame[92].giroAvion = 180;

	KeyFrame[93].movAvion_x = 145.0f;
	KeyFrame[93].movAvion_y = 10.0f;
	KeyFrame[93].giroAvion = 180;

	KeyFrame[94].movAvion_x = 140.0f;
	KeyFrame[94].movAvion_y = 0.0f;
	KeyFrame[94].giroAvion = 180;

	KeyFrame[95].movAvion_x = 135.0f;
	KeyFrame[95].movAvion_y = -10.0f;
	KeyFrame[95].giroAvion = 180;

	KeyFrame[96].movAvion_x = 130.0f;
	KeyFrame[96].movAvion_y = 0.0f;
	KeyFrame[96].giroAvion = 180;

	KeyFrame[97].movAvion_x = 125.0f;
	KeyFrame[97].movAvion_y = 10.0f;
	KeyFrame[97].giroAvion = 180;

	KeyFrame[98].movAvion_x = 120.0f;
	KeyFrame[98].movAvion_y = 0.0f;
	KeyFrame[98].giroAvion = 180;

	KeyFrame[99].movAvion_x = 115.0f;
	KeyFrame[99].movAvion_y = -10.0f;
	KeyFrame[99].giroAvion = 180;

	KeyFrame[100].movAvion_x = 110.0f;
	KeyFrame[100].movAvion_y = 0.0f;
	KeyFrame[100].giroAvion = 180;

	KeyFrame[101].movAvion_x = 105.0f;
	KeyFrame[101].movAvion_y = 10.0f;
	KeyFrame[101].giroAvion = 180;

	KeyFrame[102].movAvion_x = 100.0f;
	KeyFrame[102].movAvion_y = 0.0f;
	KeyFrame[102].giroAvion = 180;

	KeyFrame[103].movAvion_x = 95.0f;
	KeyFrame[103].movAvion_y = -10.0f;
	KeyFrame[103].giroAvion = 180;

	KeyFrame[104].movAvion_x = 90.0f;
	KeyFrame[104].movAvion_y = 0.0f;
	KeyFrame[104].giroAvion = 180;

	KeyFrame[105].movAvion_x = 85.0f;
	KeyFrame[105].movAvion_y = 10.0f;
	KeyFrame[105].giroAvion = 180;

	KeyFrame[106].movAvion_x = 80.0f;
	KeyFrame[106].movAvion_y = 0.0f;
	KeyFrame[106].giroAvion = 180;

	KeyFrame[107].movAvion_x = 75.0f;
	KeyFrame[107].movAvion_y = -10.0f;
	KeyFrame[107].giroAvion = 180;

	KeyFrame[108].movAvion_x = 70.0f;
	KeyFrame[108].movAvion_y = 0.0f;
	KeyFrame[108].giroAvion = 180;

	KeyFrame[109].movAvion_x = 65.0f;
	KeyFrame[109].movAvion_y = 10.0f;
	KeyFrame[109].giroAvion = 180;

	KeyFrame[110].movAvion_x = 60.0f;
	KeyFrame[110].movAvion_y = 0.0f;
	KeyFrame[110].giroAvion = 180;

	KeyFrame[111].movAvion_x = 55.0f;
	KeyFrame[111].movAvion_y = -10.0f;
	KeyFrame[111].giroAvion = 180;

	KeyFrame[112].movAvion_x = 50.0f;
	KeyFrame[112].movAvion_y = 0.0f;
	KeyFrame[112].giroAvion = 180;

	KeyFrame[113].movAvion_x = 45.0f;
	KeyFrame[113].movAvion_y = 10.0f;
	KeyFrame[113].giroAvion = 180;

	KeyFrame[114].movAvion_x = 40.0f;
	KeyFrame[114].movAvion_y = 0.0f;
	KeyFrame[114].giroAvion = 180;

	KeyFrame[115].movAvion_x = 35.0f;
	KeyFrame[115].movAvion_y = -10.0f;
	KeyFrame[115].giroAvion = 180;

	KeyFrame[116].movAvion_x = 30.0f;
	KeyFrame[116].movAvion_y = 0.0f;
	KeyFrame[116].giroAvion = 180;

	KeyFrame[117].movAvion_x = 25.0f;
	KeyFrame[117].movAvion_y = 10.0f;
	KeyFrame[117].giroAvion = 180;

	KeyFrame[118].movAvion_x = 20.0f;
	KeyFrame[118].movAvion_y = 0.0f;
	KeyFrame[118].giroAvion = 180;

	KeyFrame[119].movAvion_x = 15.0f;
	KeyFrame[119].movAvion_y = -10.0f;
	KeyFrame[119].giroAvion = 180;

	KeyFrame[120].movAvion_x = 10.0f;
	KeyFrame[120].movAvion_y = 0.0f;
	KeyFrame[120].giroAvion = 180;

	KeyFrame[121].movAvion_x = 5.0f;
	KeyFrame[121].movAvion_y = 10.0f;
	KeyFrame[121].giroAvion = 180;

	KeyFrame[122].movAvion_x = 0.0f;
	KeyFrame[122].movAvion_y = 0.0f;
	KeyFrame[122].giroAvion = 180;

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
		model = glm::translate(model, glm::vec3(movCoche, 0.5f, -1.5f));
		modelaux = model;
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Kitt_M.RenderModel();
		model = modelaux;
		model = glm::translate(model, glm::vec3(-1.0, -0.1f, 0.2f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.07f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotllanta * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		// Llanta_M.RenderModel();

		// model = glm::mat4(1.0);
		// model = glm::translate(model, glm::vec3(0.0f+3*movCoche, 3.0f + 0.33*sin(glm::radians(rotllanta)), -1.0 ));
		// model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		// model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		// model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		// Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		////color = glm::vec3(0.0f, 1.0f, 0.0f);
		////glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		// glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		// Blackhawk_M.RenderModel();

		model = glm::mat4(1.0);
		posblackhawk = glm::vec3(posXavion + movAvion_x, posYavion + movAvion_y, posZavion);
		model = glm::translate(model, posblackhawk);
		model = glm::translate(model, glm::vec3(0.f, 10.f, -10.f));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		Blackhawk_M.RenderModel();

		// color = glm::vec3(1.0f, 1.0f, 1.0f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.53f, 0.0f));
		model = glm::scale(model, glm::vec3(25.0f, 1.0f, 2.0f));

		// glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Camino_M.RenderModel();

		// Agave �qu� sucede si lo renderizan antes del coche y de la pista?
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, -2.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		// blending: transparencia o traslucidez
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		AgaveTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		// textura con movimiento
		// Importantes porque la variable uniform no podemos modificarla directamente
		toffsetu += 0.001;
		toffsetv += 0.001;
		// para que no se desborde la variable
		if (toffsetu > 1.0)
			toffsetu = 0.0;
		// if (toffsetv > 1.0)
		// toffsetv = 0;
		// printf("\ntfosset %f \n", toffsetu);
		// pasar a la variable uniform el valor actualizado
		toffset = glm::vec2(toffsetu, toffsetv);

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.2f, -6.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		FlechaTexture.UseTexture();
		// Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();
		glDisable(GL_BLEND);

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
