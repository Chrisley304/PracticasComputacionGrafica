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
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;

// PARED STRANGER THINGS
Texture ParedST_Texture;
Texture ParedST_A_Texture;
Texture ParedST_B_Texture;
Texture ParedST_C_Texture;
Texture ParedST_D_Texture;
Texture ParedST_E_Texture;
Texture ParedST_F_Texture;
Texture ParedST_G_Texture;
Texture ParedST_H_Texture;
Texture ParedST_I_Texture;
Texture ParedST_J_Texture;
Texture ParedST_K_Texture;
Texture ParedST_L_Texture;
Texture ParedST_M_Texture;
Texture ParedST_N_Texture;
Texture ParedST_O_Texture;
Texture ParedST_P_Texture;
Texture ParedST_Q_Texture;
Texture ParedST_R_Texture;
Texture ParedST_S_Texture;
Texture ParedST_T_Texture;
Texture ParedST_U_Texture;
Texture ParedST_V_Texture;
Texture ParedST_W_Texture;
Texture ParedST_X_Texture;
Texture ParedST_Y_Texture;
Texture ParedST_Z_Texture;

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
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

bool* keys;

// c�lculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
    unsigned int vLength, unsigned int normalOffset)
{
    for (size_t i = 0; i < indiceCount; i += 3) {
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

    for (size_t i = 0; i < verticeCount / vLength; i++) {
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
        0, 1, 2
    };

    GLfloat vertices[] = {
        //	x      y      z			u	  v			nx	  ny    nz
        -1.0f, -1.0f, -0.6f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, -0.6f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f
    };

    unsigned int floorIndices[] = {
        0, 2, 1,
        1, 2, 3
    };

    GLfloat floorVertices[] = {
        -10.0f, 0.0f, -10.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, -10.0f, 10.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -10.0f, 0.0f, 10.0f, 0.0f, 10.0f, 0.0f, -1.0f, 0.0f,
        10.0f, 0.0f, 10.0f, 10.0f, 10.0f, 0.0f, -1.0f, 0.0f
    };
    unsigned int vegetacionIndices[] = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7
    };

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

    unsigned int paredSTIndices[] = {
        0, 1, 2,
        0, 2, 3
    };

    GLfloat paredSTVertices[] = {
        //	x      y      z			u	  v			nx	  ny    nz
        -0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, 0.0f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, 0.0f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };

    Mesh* obj1 = new Mesh();
    obj1->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj1);

    Mesh* obj2 = new Mesh();
    obj2->CreateMesh(vertices, indices, 32, 12);
    meshList.push_back(obj2);

    Mesh* obj3 = new Mesh();
    obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
    meshList.push_back(obj3);

    Mesh* obj4 = new Mesh();
    obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
    meshList.push_back(obj4);

    Mesh* obj5 = new Mesh();
    obj5->CreateMesh(paredSTVertices, paredSTIndices, 32, 6);
    meshList.push_back(obj5);
}

void CreateShaders()
{
    Shader* shader1 = new Shader();
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
int i_curr_steps = 0;
typedef struct _frame {
    // Variables para GUARDAR Key Frames
    float movAvion_x; // Variable para PosicionX
    float movAvion_y; // Variable para PosicionY
    float movAvion_xInc; // Variable para IncrementoX
    float movAvion_yInc; // Variable para IncrementoY

    float giroAvion;
    float giroAvionInc;
} FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0; // introducir datos
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
    if (play) {
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
            } else // Next frame interpolations
            {
                // printf("entro aqu�\n");
                i_curr_steps = 0; // Reset counter
                // Interpolation
                interpolation();
            }
        } else {
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

    // Pared Stranger Things
    ParedST_Texture = Texture("Textures/ParedStrangerThings.jpg");
    ParedST_Texture.LoadTextureA();
    ParedST_A_Texture = Texture("Textures/ParedStrangerThings-A.jpg");
    ParedST_A_Texture.LoadTextureA();
    ParedST_B_Texture = Texture("Textures/ParedStrangerThings-B.jpg");
    ParedST_B_Texture.LoadTextureA();
    ParedST_C_Texture = Texture("Textures/ParedStrangerThings-C.jpg");
    ParedST_C_Texture.LoadTextureA();
    ParedST_D_Texture = Texture("Textures/ParedStrangerThings-D.jpg");
    ParedST_D_Texture.LoadTextureA();
    ParedST_E_Texture = Texture("Textures/ParedStrangerThings-E.jpg");
    ParedST_E_Texture.LoadTextureA();
    ParedST_F_Texture = Texture("Textures/ParedStrangerThings-F.jpg");
    ParedST_F_Texture.LoadTextureA();
    ParedST_G_Texture = Texture("Textures/ParedStrangerThings-G.jpg");
    ParedST_G_Texture.LoadTextureA();
    ParedST_H_Texture = Texture("Textures/ParedStrangerThings-H.jpg");
    ParedST_H_Texture.LoadTextureA();
    ParedST_I_Texture = Texture("Textures/ParedStrangerThings-I.jpg");
    ParedST_I_Texture.LoadTextureA();
    ParedST_J_Texture = Texture("Textures/ParedStrangerThings-J.jpg");
    ParedST_J_Texture.LoadTextureA();
    ParedST_K_Texture = Texture("Textures/ParedStrangerThings-K.jpg");
    ParedST_K_Texture.LoadTextureA();
    ParedST_L_Texture = Texture("Textures/ParedStrangerThings-L.jpg");
    ParedST_L_Texture.LoadTextureA();
    ParedST_M_Texture = Texture("Textures/ParedStrangerThings-M.jpg");
    ParedST_M_Texture.LoadTextureA();
    ParedST_N_Texture = Texture("Textures/ParedStrangerThings-N.jpg");
    ParedST_N_Texture.LoadTextureA();
    ParedST_O_Texture = Texture("Textures/ParedStrangerThings-O.jpg");
    ParedST_O_Texture.LoadTextureA();
    ParedST_P_Texture = Texture("Textures/ParedStrangerThings-P.jpg");
    ParedST_P_Texture.LoadTextureA();
    ParedST_Q_Texture = Texture("Textures/ParedStrangerThings-Q.jpg");
    ParedST_Q_Texture.LoadTextureA();
    ParedST_R_Texture = Texture("Textures/ParedStrangerThings-R.jpg");
    ParedST_R_Texture.LoadTextureA();
    ParedST_S_Texture = Texture("Textures/ParedStrangerThings-S.jpg");
    ParedST_S_Texture.LoadTextureA();
    ParedST_T_Texture = Texture("Textures/ParedStrangerThings-T.jpg");
    ParedST_T_Texture.LoadTextureA();
    ParedST_U_Texture = Texture("Textures/ParedStrangerThings-U.jpg");
    ParedST_U_Texture.LoadTextureA();
    ParedST_V_Texture = Texture("Textures/ParedStrangerThings-V.jpg");
    ParedST_V_Texture.LoadTextureA();
    ParedST_W_Texture = Texture("Textures/ParedStrangerThings-W.jpg");
    ParedST_W_Texture.LoadTextureA();
    ParedST_X_Texture = Texture("Textures/ParedStrangerThings-X.jpg");
    ParedST_X_Texture.LoadTextureA();
    ParedST_Y_Texture = Texture("Textures/ParedStrangerThings-Y.jpg");
    ParedST_Y_Texture.LoadTextureA();
    ParedST_Z_Texture = Texture("Textures/ParedStrangerThings-Z.jpg");
    ParedST_Z_Texture.LoadTextureA();

    std::vector<std::string>
        skyboxFaces;
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

    KeyFrame[0].movAvion_x = 0.0f;
    KeyFrame[0].movAvion_y = 0.0f;
    KeyFrame[0].giroAvion = 0;

    // Loop mientras no se cierra la ventana
    while (!mainWindow.getShouldClose()) {
        GLfloat now = glfwGetTime();
        deltaTime = now - lastTime;
        deltaTime += (now - lastTime) / limitFPS;
        lastTime = now;

        keys = mainWindow.getsKeys();

        // Recibir eventos del usuario
        glfwPollEvents();
        camera.keyControl(mainWindow.getsKeys(), deltaTime);
        camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        //        animate();

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

//        // luz ligada a la c�mara de tipo flash
//        glm::vec3 lowerLight = camera.getCameraPosition();
//        lowerLight.y -= 0.3f;
//        spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
//
//        // informaci�n al shader de fuentes de iluminaci�n
        shaderList[0].SetDirectionalLight(&mainLight);
//        shaderList[0].SetPointLights(pointLights, pointLightCount);
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

        // Pared Stranger Things
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(20.0f, 10.0f, 5.0f));
        model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));

        if (keys[GLFW_KEY_A]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_A_Texture.UseTexture();
        } else if (keys[GLFW_KEY_B]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_B_Texture.UseTexture();
        } else if (keys[GLFW_KEY_C]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_C_Texture.UseTexture();
        } else if (keys[GLFW_KEY_D]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_D_Texture.UseTexture();
        } else if (keys[GLFW_KEY_E]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_E_Texture.UseTexture();
        } else if (keys[GLFW_KEY_F]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_F_Texture.UseTexture();
        } else if (keys[GLFW_KEY_G]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_G_Texture.UseTexture();
        } else if (keys[GLFW_KEY_H]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_H_Texture.UseTexture();
        } else if (keys[GLFW_KEY_I]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_I_Texture.UseTexture();
        } else if (keys[GLFW_KEY_J]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_J_Texture.UseTexture();
        } else if (keys[GLFW_KEY_K]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_K_Texture.UseTexture();
        } else if (keys[GLFW_KEY_L]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_L_Texture.UseTexture();
        } else if (keys[GLFW_KEY_M]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_M_Texture.UseTexture();
        } else if (keys[GLFW_KEY_N]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_N_Texture.UseTexture();
        } else if (keys[GLFW_KEY_O]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_O_Texture.UseTexture();
        } else if (keys[GLFW_KEY_P]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_P_Texture.UseTexture();
        } else if (keys[GLFW_KEY_Q]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_Q_Texture.UseTexture();
        } else if (keys[GLFW_KEY_R]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_R_Texture.UseTexture();
        } else if (keys[GLFW_KEY_S]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_S_Texture.UseTexture();
        } else if (keys[GLFW_KEY_T]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_T_Texture.UseTexture();
        } else if (keys[GLFW_KEY_U]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_U_Texture.UseTexture();
        } else if (keys[GLFW_KEY_V]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_V_Texture.UseTexture();
        } else if (keys[GLFW_KEY_W]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_W_Texture.UseTexture();
        } else if (keys[GLFW_KEY_X]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_X_Texture.UseTexture();
        } else if (keys[GLFW_KEY_Y]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_Y_Texture.UseTexture();
        } else if (keys[GLFW_KEY_Z]) {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_Z_Texture.UseTexture();
        } else {
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(uniformColor, 1, glm::value_ptr(color));
            ParedST_Texture.UseTexture();
        }
        meshList[4]->RenderMesh();

        //		glDisable(GL_BLEND);
        glUseProgram(0);
        mainWindow.swapBuffers();
    }

    return 0;
}
