/*
   Semestre 2023-2
   Animaci�n:
   1.- Simple o basica:Por banderas y condicionales
   2.- Compleja: Por medio de funciones y algoritmos,Textura Animada.
   Adicional.- Texturizado con transparencia usando Blending: Requerimos dibujar lo que esta atras primero
 */
// para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// para probar el importer
// #include<assimp/Importer.hpp>

#include "Camera.h"
#include "Mesh.h"
#include "Model.h"
#include "Shader_light.h"
#include "Skybox.h"
#include "Sphere.h"
#include "Texture.h"
#include "Window.h"

// para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "Material.h"
#include "PointLight.h"
#include "SpotLight.h"
const float toRadians = 3.14159265f / 180.0f;

// variables para animaci�n
float movCocheX;
float movHelicopteroOffset;
float giroHelicoptero;
float giroHelicopteroOffset;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanzaDerecho;
bool avanzaHelicoptero;
bool reversaHelicoptero;
float toffsetu = 0.0f;
float toffsetv = 0.0f;
float movYDado;
float movYDadoOffset;
float giroDado;
float giroDadoOffset;

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
Texture octaedroTexture;

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Helice;
Model Dado_M;

// Coche propio (Vocho):
Model Vocho;
Model Llantas_Delanteras_Vocho;
Model Llantas_Traseras_Vocho;
Model PistaCarreras;

// Modelos Extra:
Model Globito;

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
SpotLight spotLights_aux[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char *vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char *fShader = "shaders/shader_light.frag";

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

void CrearDado8Caras()
{
    GLfloat verticesOctaedro[] = {
        // x        y        z        S        T        Nx        Ny        Nz
        // Piramide superior --------------------------------
        // Cara 1 - A, F, G
        0.f, 1.f, 0.f, 0.4940153616873, 0.7458299758063, 1.0f, -1.0f, -1.0f,   // 0 - A
        -1.f, 0.f, 0.f, 0.2436577972163f, 0.498299814814f, 1.0f, -1.0f, -1.0f, // 1 - F
        0.f, 0.f, 1.f, 0.7521710750057, 0.4978370540927, 1.0f, -1.0f, -1.0f,   // 2 - G
        // Cara 2 - A, G, H
        0.f, 1.f, 0.f, 0.0f, 0.25f, -1.0f, -1.0f, -1.0f,                        // 3 - A
        0.f, 0.f, 1.f, 0.4971259273646f, 0.2494817895031f, -1.0f, -1.0f, -1.0f, // 4 - G
        1.f, 0.f, 0.f, 0.2436577972163f, 0.498299814814f, -1.0f, -1.0f, -1.0f,  // 5 - H
        // Cara 3 - A, H, I
        0.f, 1.f, 0.f, 0, 0.7459840013939, -1.0f, -1.0f, 1.0f,                // 6 - A
        1.f, 0.f, 0.f, 0.24, 0.5, -1.0f, -1.0f, 1.0f,                         // 7 - H
        0.f, 0.f, -1.f, 0.4905520219476, 0.7464275747373, -1.0f, -1.0f, 1.0f, // 8 - I
        // Cara 4 - A, I, F
        0.f, 1.f, 0.f, 0.4971259273646, 0.2494817895031, 1.0f, -1.0f, 1.0f,  // 9 - A
        0.f, 0.f, -1.f, 0.7521710750057, 0.4978370540927, 1.0f, -1.0f, 1.0f, // 10 - I
        -1.f, 0.f, 0.f, 0.2447077950412, 0.4971758770058, 1.0f, -1.0f, 1.0f, // 11 - F
        // Piramide inferior --------------------------------
        // Cara 5 - B, F, G
        0.f, -1.f, 0.f, 0.7527437890022, 0.9949471586345, 1.0f, 1.0f, -1.0f, // 12 - B
        -1.f, 0.f, 0.f, 1.f, 0.75, 1.0f, 1.0f, -1.0f,                        // 13 - F
        0.f, 0.f, 1.f, 0.5, 0.75, 1.0f, 1.0f, -1.0f,                         // 14 - G
        // Cara 6 - B, G, H
        0.f, -1.f, 0.f, 0.9928426579919, 0.248680864686, -1.0f, 1.0f, -1.0f, // 15 - B
        0.f, 0.f, 1.f, 0.7515983809738, 0.495821007847, -1.0f, 1.0f, -1.0f,  // 16 - G
        1.f, 0.f, 0.f, 0.5, 0.25, -1.0f, 1.0f, -1.0f,                        // 17 - H
        // Cara 7 - B, H, I
        0.f, -1.f, 0.f, 0.9977169711349, 0.74578176779, -1.0f, 1.0f, 1.0f,   // 18 - B
        1.f, 0.f, 0.f, 0.7518823834043, 0.500154589373, -1.0f, 1.0f, 1.0f,   // 19 - H
        0.f, 0.f, -1.f, 0.4994164792683, 0.7452641052236, -1.0f, 1.0f, 1.0f, // 20 - I
        // Cara 8 - B, I, F
        0.f, -1.f, 0.f, 0.7524450151198, 0.0016302156309, 1.0f, 1.0f, 1.0f, // 21 - B
        0.f, 0.f, -1.f, 0.994108518509, 0.248172089342, 1.0f, 1.0f, 1.0f,   // 22 - I
        -1.f, 0.f, 0.f, 0.4997324975772, 0.2480514725787, 1.0f, 1.0f, 1.0f, // 23 - F
    };

    unsigned int indices[] = {
        // Piramide superior --------------------------------
        // Cara 1 - A, F, G
        0, 1, 2,
        // Cara 2 - A, G, H
        3, 4, 5,
        // Cara 3 - A, H, I
        6, 7, 8,
        // Cara 4 - A, I, F
        9, 10, 11,
        // Piramide inferior --------------------------------
        // Cara 5 - B, F, G
        12, 13, 14,
        // Cara 6 - B, G, H
        15, 16, 17,
        // Cara 7 - B, H, I
        18, 19, 20,
        // Cara 8 - B, I, F
        21, 22, 23};

    Mesh *dado = new Mesh();

    dado->CreateMesh(verticesOctaedro, indices, 192, 24);
    meshList.push_back(dado);
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
        0, 1, 2,
        0, 2, 3};

    GLfloat flechaVertices[] = {
        -0.5f, 0.0f, 0.5f,
        0.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.5f,
        0.0f, 0.5f, 1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.5f, 0.0f,
        -0.5f, 1.0f, 1.0f,
        0.0f, -1.0f, 0.0f,
        -0.5f, 0.0f, -0.5f,
        0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f};

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

int main()
{
    mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
    mainWindow.Initialise();

    CreateObjects();
    CrearDado8Caras();
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 2.5f, 0.5f);

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
    Llanta_M = Model();
    Llanta_M.LoadModel("Models/k_rueda.3ds");
    Blackhawk_M = Model();
    Blackhawk_M.LoadModel("Models/helicoptero.obj");
    Helice = Model();
    Helice.LoadModel("Models/Helice");
    Camino_M = Model();
    Camino_M.LoadModel("Models/railroad track.obj");
    Globito = Model();
    Globito.LoadModel("Models/globito.obj");

    // Coche propio (Vocho)
    Vocho = Model();
    Vocho.LoadModel("Models/vocho.obj");
    Llantas_Traseras_Vocho = Model();
    Llantas_Traseras_Vocho.LoadModel("Models/llantasvocho.obj");
    Llantas_Delanteras_Vocho = Model();
    Llantas_Delanteras_Vocho.LoadModel("Models/llantasvocho.obj");

    // Dado 8 caras
    octaedroTexture = Texture("Textures/Dado8Caras.tga");
    octaedroTexture.LoadTextureA();
    // Pista carrera
    PistaCarreras = Model();
    PistaCarreras.LoadModel("Models/pista_text.obj");

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
    //    pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
    //                                0.0f, 1.0f,
    //                                0.0f, 2.5f, 1.5f,
    //                                0.3f, 0.2f, 0.1f);
    //    pointLightCount++;

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
    //    spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
    //                              1.0f, 2.0f,
    //                              5.0f, 10.0f, 0.0f,
    //                              0.0f, -5.0f, 0.0f,
    //                              1.0f, 0.0f, 0.0f,
    //                              15.0f);
    //    spotLightCount++;

    // luz del coche
    spotLights[1] = SpotLight(0.5f, 0.f, 0.5f,   // R, G, B
                              1.0f, 2.0f,        // Light Intensity, Color Intensity
                              0.f, 1.0f, -1.0f,  // xpos, ypos, zpos
                              -1.0f, 0.0f, 0.0f, // xdir, ydir, zdir
                              0.5f, 0.0f, 0.0f,  // con, lin, exp
                              5.0f);             // edge
    spotLightCount++;

    // Copia de luz del coche mirando al frente
    spotLights_aux[0] = SpotLight(0.5f, 0.0f, 0.5f,  // R, G, B
                                  1.0f, 2.0f,        // Light Intensity, Color Intensity
                                  0.f, 1.0f, -1.0f,  // xpos, ypos, zpos
                                  -1.0f, 0.0f, 0.0f, // xdir, ydir, zdir
                                  0.5f, 0.0f, 0.0f,  // con, lin, exp
                                  5.0f);

    // Luz del coche mirando de reversa
    spotLights_aux[1] = SpotLight(0.5f, 0.0f, 0.5f, // R, G, B
                                  1.0f, 2.0f,       // Light Intensity, Color Intensity
                                  0.f, 1.0f, -4.0f, // xpos, ypos, zpos
                                  1.0f, 0.0f, 0.0f, // xdir, ydir, zdir
                                  0.5f, 0.0f, 0.0f, // con, lin, exp
                                  5.0f);            // edge

    // Luz del coche mirando hacia +Z
    spotLights_aux[2] = SpotLight(0.5f, 0.0f, 0.5f, // R, G, B
                                  1.0f, 2.0f,       // Light Intensity, Color Intensity
                                  0.f, 1.0f, -4.0f, // xpos, ypos, zpos
                                  0.0f, 0.0f, 1.0f, // xdir, ydir, zdir
                                  0.5f, 0.0f, 0.0f, // con, lin, exp
                                  5.0f);            // edge

    // luz de helic�ptero
    spotLights[2] = SpotLight(1.0f, 0.0f, 0.f,
                              1.0f, 2.0f,
                              5.0f, 12.0f, 0.0f,
                              0.0, -2.5f, 0.0f,
                              0.2f, 0.0f, 0.0f,
                              5.0f);
    spotLightCount++;

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
           uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
    GLuint uniformColor = 0;
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

    // Variables Helicoptero
    float movXHelicoptero = 0.0f;
    float movZHelicoptero = 0.0f;
    float movYHelicoptero = 0.0f;
    movHelicopteroOffset = 0.2f;
    giroHelicoptero = 0.0f;
    float giroHelices = 0.0f;
    giroHelicopteroOffset = 0.5f;
    float giroHelicesOffset = 0.8f;
    int direccionHelicoptero = 1;
    avanzaHelicoptero = true;
    int nVueltasHelicop = 0;
    int cuentaLuzHelicoptero = 0;
    int caseMovHelicop = 0;

    // Variables movimiento coche
    movCocheX = 0.0f;
    float movCocheZ = 0.0f;
    float movCocheY = 0.0f;
    movOffset = 0.4f;
    rotllanta = 0.0f;
    avanzaDerecho = true;
    rotllantaOffset = 2.5f;
    //    int nVueltasAuto = 0;
    int curvaPista = 0;
    float giroAutoY = 0.0f;
    float giroAutoX = 0.0f;
    float giroAutoZ = 0.0f;
    float giroAutoYAux = 0.0f;
    float giroAutoXAux = 0.0f;
    float giroAutoZAux = 0.0f;
    float giroAutoOffset = 0.5f;

    // Globito
    float movGlobitoX = 0.0f;
    float movGlobitoY = 0.0f;
    float movGlobitoZ = 0.0f;
    float anguloGlobito = 0.0f;
    float anguloGlobitoOffset = 0.1f;
    float aGlobito = 0.1f;  // Distancia entre las espirales
    float bGlobito = 0.02f; // Factor de crecimiento

    // Loop mientras no se cierra la ventana
    while (!mainWindow.getShouldClose())
    {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

        // Recibir eventos del usuario
        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

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

        // luz ligada a la camara de tipo flash
        glm::vec3 lowerLight = camera.getCameraPosition();
        lowerLight.y -= 0.3f;
        spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

        // informaci�n al shader de fuentes de iluminaci�n
        shaderList[0].SetDirectionalLight(&mainLight);
        shaderList[0].SetPointLights(pointLights, pointLightCount);
        //        shaderList[0].SetSpotLights(spotLights, spotLightCount);

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

        // Botón de reset (F5)
        if (mainWindow.resetCarro())
        {
            curvaPista = 0;
            mainWindow.setResetFlag(false);
        }

        switch (curvaPista)
        {
        case 0:
        {
            movCocheX = 0.0f;
            movCocheY = 0.0f;
            movCocheZ = 0.0f;
            giroAutoX = 0.0f;
            giroAutoY = 0.0f;
            giroAutoZ = 0.0f;
            curvaPista = 1;
        }
        break;

        // Recto en -X
        case 1:

            if (movCocheX > -40.0f)
            {
                movCocheX -= movOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1] = spotLights_aux[0]; // Direccion "de frente" del auto
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, movCocheZ));
            }
            else
            {
                curvaPista = 2;
                giroAutoYAux = giroAutoY;
            }

            break;

        // Inicio curva
        case 2:

            if (movCocheX > -70.0f && giroAutoY < 90.0f + giroAutoYAux)
            {
                movCocheX -= movOffset * deltaTime * .7;
                rotllanta += rotllantaOffset * deltaTime;
                giroAutoY += giroAutoOffset * deltaTime;
                movCocheZ += movOffset * deltaTime * .5;
                spotLights[1] = spotLights_aux[0]; // Direccion "de frente" del auto
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 3;
            }

            break;

        // Fin curva
        case 3:

            if (giroAutoY < 90.0f + giroAutoYAux)
            {
                movCocheZ += movOffset * deltaTime * .5;
                giroAutoY += giroAutoOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1] = spotLights_aux[2]; // Direccion "de frente" del auto
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 4;
            }

            break;

        // Recto en +Z
        case 4:

            if (movCocheZ < 70.0f)
            {
                movCocheZ += movOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 5;
                giroAutoYAux = giroAutoY;
            }

            break;

        // Inicio curva
        case 5:

            if (movCocheZ < 90.0f && giroAutoY > -90.0f + giroAutoYAux)
            {
                movCocheZ += movOffset * deltaTime * .7;
                movCocheX -= movOffset * deltaTime * .5;
                rotllanta += rotllantaOffset * deltaTime;
                giroAutoY -= giroAutoOffset * deltaTime;
            }
            else
            {
                curvaPista = 6;
            }

            break;

        // Fin curva
        case 6:

            if (giroAutoY > -90.0f + giroAutoYAux)
            {
                movCocheX -= movOffset * deltaTime * .5;
                giroAutoY -= giroAutoOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1] = spotLights_aux[0]; // Direccion "de frente" del auto
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 7;
            }

            break;

        // Recto en -X
        case 7:

            if (movCocheX > -146.f)
            {
                movCocheX -= movOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1] = spotLights_aux[0]; // Direccion "de frente" del auto
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 8;
                giroAutoYAux = giroAutoY;
            }

            break;

        // Inicio curva
        case 8:

            if (movCocheX > -165.f && giroAutoY < 90.0f + giroAutoYAux)
            {
                movCocheX -= movOffset * deltaTime * .7;
                rotllanta += rotllantaOffset * deltaTime;
                giroAutoY += giroAutoOffset * deltaTime;
                movCocheZ += movOffset * deltaTime * .5;
                spotLights[1] = spotLights_aux[0]; // Direccion "de frente" del auto
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 9;
            }

            break;

        // Fin curva
        case 9:

            if (giroAutoY < 90.0f + giroAutoYAux)
            {
                movCocheZ += movOffset * deltaTime * .5;
                giroAutoY += giroAutoOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1] = spotLights_aux[2]; // Direccion "de frente" del auto
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 10;
            }

            break;

        // Recto en +Z
        case 10:

            if (movCocheZ < 140.0f)
            {
                movCocheZ += movOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 11;
                giroAutoYAux = giroAutoY;
            }

            break;

        // Inicio curva
        case 11:

            if (movCocheZ < 150.0f && giroAutoY < 90.0f + giroAutoYAux)
            {
                movCocheZ += movOffset * deltaTime * .7;
                movCocheX += movOffset * deltaTime * .5;
                rotllanta += rotllantaOffset * deltaTime;
                giroAutoY += giroAutoOffset * deltaTime;
            }
            else
            {
                curvaPista = 12;
            }

            break;

        // Fin curva
        case 12:

            if (giroAutoY < 90.0f + giroAutoYAux)
            {
                movCocheX += movOffset * deltaTime * .5;
                giroAutoY += giroAutoOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1] = spotLights_aux[1]; // Direccion "de reversa" del auto
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 13;
            }

            break;

        // Recto en X
        case 13:

            if (movCocheX < -32.0f)
            {
                movCocheX += movOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0 + movCocheY, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 14;
                giroAutoZAux = giroAutoZ;
            }

            break;

        // Sube rampa
        case 14:

            if (movCocheX < -15.f && giroAutoZ < 45 + giroAutoZAux)
            {
                movCocheX += movOffset * deltaTime * .7;
                movCocheY += movOffset * deltaTime * .3;
                giroAutoZ += giroAutoOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0 + movCocheY, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 15;
            }

            break;

        // Sube rampa sin girar en Z
        case 15:

            if (movCocheX < -7.f)
            {
                movCocheX += movOffset * deltaTime * .7;
                movCocheY += movOffset * deltaTime * .3;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0 + movCocheY, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 16;
                giroAutoZAux = giroAutoZ;
            }

            break;

        // Se endereza
        case 16:

            if (giroAutoZ > -45 + giroAutoZAux)
            {
                giroAutoZ -= giroAutoOffset * deltaTime;
                movCocheX += movOffset * deltaTime * .7;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0 + movCocheY, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 17;
            }

            break;

        // Recto en +X
        case 17:

            if (movCocheX < 40.0f)
            {
                movCocheX += movOffset * deltaTime;
                rotllanta += rotllantaOffset * deltaTime;
                spotLights[1].SetPos(glm::vec3(0.0f + movCocheX, 0.0 + movCocheY, 0.0f + movCocheZ));
            }
            else
            {
                curvaPista = 18;
                giroAutoZAux = giroAutoZ;
            }

            break;

        default:
            break;
        }

        // Carroceria Vocho
        model = glm::mat4(1.0);

        if (mainWindow.isAutoManual())
        {
            model = glm::translate(model, glm::vec3(mainWindow.getMovimientoXAuto(), 0.f, mainWindow.getMovimientoZAuto())); //         Movimiento manual del auto
            spotLights[1].SetPos(glm::vec3(0.0f + mainWindow.getMovimientoXAuto(), 0.0, mainWindow.getMovimientoZAuto()));
        }
        else
        {
            model = glm::translate(model, glm::vec3(movCocheX, movCocheY, movCocheZ)); //         Movimiento manual del auto
        }

        model = glm::translate(model, glm::vec3(0.0f, -0.5f, -0.f));
        modelaux = model;
        model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
        model = glm::rotate(model, glm::radians(giroAutoZ), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(giroAutoY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(giroAutoX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        Vocho.RenderModel();

        // Llantas delanteras
        model = modelaux;
        model = glm::rotate(model, glm::radians(giroAutoZ), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(giroAutoY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(giroAutoX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(-6.5f, 1.5f, -0.f));
        model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
        model = glm::rotate(model, glm::radians(rotllanta), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        Llantas_Delanteras_Vocho.RenderModel();

        // Llantas traseras
        model = modelaux;
        model = glm::rotate(model, glm::radians(giroAutoZ), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, glm::radians(giroAutoY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(giroAutoX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(5.35f, 1.5f, -0.f));
        model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
        model = glm::rotate(model, glm::radians(rotllanta), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        Llantas_Traseras_Vocho.RenderModel();

        // Movimiento del helicoptero:

        // Luz del helicoptero
        if (cuentaLuzHelicoptero >= 60)
        {
            cuentaLuzHelicoptero = 0;
            mainWindow.alternHelicopteroEncendido();
        }

        if (mainWindow.isHelicopteroEncendido())
        {
            shaderList[0].SetSpotLights(spotLights, spotLightCount);
        }
        else
        {
            shaderList[0].SetSpotLights(spotLights, spotLightCount - 1); // Se resta -1 para no renderizar la luz del helicoptero
        }

        cuentaLuzHelicoptero++;

        // movimiento vertical:
        movYHelicoptero += giroHelicopteroOffset * deltaTime;

        // Avanza hacia adelante hasta topar con "pared"
        if (avanzaHelicoptero)
        {
            if (direccionHelicoptero == 1 && movZHelicoptero > -500.0f)
            {
                movZHelicoptero -= movHelicopteroOffset * deltaTime;
            }
            else if (direccionHelicoptero == -1 && movZHelicoptero < 500.0f)
            {
                movZHelicoptero += movHelicopteroOffset * deltaTime;
            }
            else
            {
                avanzaHelicoptero = false;
            }
        }

        // Giro del helicoptero al estar parado
        if (!avanzaHelicoptero)
        {
            if ((giroHelicoptero - nVueltasHelicop * 180) < 180.0f)
            {
                giroHelicoptero += giroHelicopteroOffset * deltaTime;
            }
            else
            {
                nVueltasHelicop++;
                avanzaHelicoptero = true;
                direccionHelicoptero = direccionHelicoptero * -1;
            }
        }

        giroHelices += giroHelicesOffset * deltaTime;

        // Helicoptero
        model = glm::mat4(1.0);
        // Si se multiplica por una constante afuera del sin se hace mas alta la onda, y si se multiplica dentro de la funcio radians, se hace el movimiento mas rapido
        model = glm::translate(model, glm::vec3(-1.0, 50.0f + (15 * sin(glm::radians(movZHelicoptero * 0.5)) + (15 * sin(glm::radians(movYHelicoptero)))), -1.0 - movZHelicoptero * 0.5));
        model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
        model = glm::rotate(model, giroHelicoptero * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
        // color = glm::vec3(0.0f, 1.0f, 0.0f);
        // glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        spotLights[2].SetPos(glm::vec3(0.0f, 7.0f + sin(glm::radians(movYHelicoptero)), 0.0f + movZHelicoptero));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Blackhawk_M.RenderModel();

        // Globito
        // recorrido de un tornado con ecuación del espiral de Arquimedes
        // Se topa la altura y distancia, para que sea facilmente visible
        if (movGlobitoY < 150.0f && movGlobitoZ < 150.0f)
        {
            movGlobitoX = (aGlobito + bGlobito * anguloGlobito) * cos(anguloGlobito);
            movGlobitoY = (aGlobito + bGlobito * anguloGlobito) * sin(anguloGlobito);
            movGlobitoZ = anguloGlobito * 2.0f;
            anguloGlobito += anguloGlobitoOffset * deltaTime;
        }
        else
        {
            movGlobitoX = 0.0f;
            movGlobitoY = 0.0f;
            movGlobitoZ = 0.0f;
            anguloGlobito = 0.1f;
        }

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(15.0f + movGlobitoX, 15.0f + movGlobitoY, 60.0f + movGlobitoZ));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Globito.RenderModel();

        // Pista de carreras
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(5.f, -2.0f, 30.0f));
        model = glm::scale(model, glm::vec3(0.85f, 0.85f, 0.85f));
        //        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        PistaCarreras.RenderModel();

        // Las texturas transparentes, se ponen al final xd
        // Agave �qu� sucede si lo renderizan antes del coche y de la pista?
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 0.5f, -20.0f));
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
        toffsetu += 0.001 * deltaTime;
        toffsetv += 0.0 * deltaTime;

        // para que no se desborde la variable
        if (toffsetu > 1.0)
        {
            toffsetu = 0.0;
        }

        // if (toffsetv > 1.0)
        //	toffsetv = 0;
        // printf("\ntfosset %f \n", toffsetu);
        // pasar a la variable uniform el valor actualizado
        toffset = glm::vec2(toffsetu, toffsetv);

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 0.2f, -15.0f));
        model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
        glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset)); // Esta linea es importante porque le "avisa" al shader que se va a actualizar el valor del offset
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
