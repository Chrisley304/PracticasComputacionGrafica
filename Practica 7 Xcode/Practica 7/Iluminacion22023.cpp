/*
   Semestre 2023-1
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

Window mainWindow;
std::vector<Mesh *> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;

Model Kitt_M;
Model Llanta_M;
Model Camino_M;
Model Blackhawk_M;
Model Dado_M;
// Coche propio (Cybertruck):
Model Vocho;
Model Llanta_Vocho_izq_frente;
Model Llanta_Vocho_izq_trasera;
Model Llanta_Vocho_der_frente;
Model Llanta_Vocho_der_trasera;
Model Cofre_Vocho;

// Objetos con luz estatica
Model SableLuz;
Model Farol;

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
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

        0.0f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};

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

    calcAverageNormals(indices, 12, vertices, 32, 8, 5);

    calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);
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
    CreateShaders();

    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

    brickTexture = Texture("Textures/brick.png");
    brickTexture.LoadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.LoadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.LoadTextureA();
    pisoTexture = Texture("Textures/piso.tga");
    pisoTexture.LoadTextureA();
    /*AgaveTexture = Texture("Textures/Agave.tga");
       AgaveTexture.LoadTextureA();*/

    Kitt_M = Model();
    Kitt_M.LoadModel("Models/kitt_optimizado.obj");
    Llanta_M = Model();
    Llanta_M.LoadModel("Models/k_rueda.3ds");
    Blackhawk_M = Model();
    Blackhawk_M.LoadModel("Models/uh60.obj");
    Camino_M = Model();
    Camino_M.LoadModel("Models/railroad track.obj");
    
    // Objetos fijos con luz
    Farol = Model();
    Farol.LoadModel("Models/faro.obj");
    SableLuz = Model();
    SableLuz.LoadModel("Models/LukeSkywalkerLightsaber.obj");

    // Coche propio (Vocho)
    Vocho = Model();
    Vocho.LoadModel("Models/vocho.obj");
    Llanta_Vocho_izq_frente = Model();
    Llanta_Vocho_izq_frente.LoadModel("Models/vocho_rueda1.obj");
    Llanta_Vocho_izq_trasera = Model();
    Llanta_Vocho_izq_trasera.LoadModel("Models/vocho_rueda2.obj");
    Llanta_Vocho_der_frente = Model();
    Llanta_Vocho_der_frente.LoadModel("Models/vocho_rueda3.obj");
    Llanta_Vocho_der_trasera = Model();
    Llanta_Vocho_der_trasera.LoadModel("Models/vocho_rueda4.obj");
//    Cofre_Vocho = Model();
//    Cofre_Vocho.LoadModel("Models/cofre_cybertruck.obj");

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
                                 0.5f, 0.3f,
                                 1.0f, 0.0f, -1.0f);
    // contador de luces puntuales
    unsigned int pointLightCount = 0;
    // Declaraci�n de primer luz puntual
    //    pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
    //                                0.5f, 0.5f,
    //                                20.0f, 1.5f, 1.5f,
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

    // luz del coche
    spotLights[1] = SpotLight(0.0f, 1.0f, 0.0f,  // R, G, B
                              1.0f, 2.0f,        // Light Intensity, Color Intensity
                              0.f, 1.0f, -1.0f,  // xpos, ypos, zpos
                              -1.0f, 0.0f, 0.0f, // xdir, ydir, zdir
                              0.5f, 0.0f, 0.0f,  // con, lin, exp
                              5.0f);             // edge
    spotLightCount++;

    // luz de helicóptero
    spotLights[2] = SpotLight(1.0f, 0.0f, 0.f,
                              1.0f, 2.0f,
                              5.0f, 12.0f, 0.0f,
                              -3.0f, -2.5f, 0.0f,
                              0.2f, 0.0f, 0.0f,
                              5.0f);
    spotLightCount++;
    
    // Luz del faro
    pointLights[0] = PointLight(1.f, 1.f, 0.f, // Amarillo
                                5.0f, 5.0f,
                                5.0f,27.0f,-25.0f, //Posición X, Y, Z
                                0.5f, 0.5f, 0.5f);//Intensidad
    pointLightCount++;
    
    //luz del sable de luz
    pointLights[1] = PointLight(0.f, 1.f, 0.f, // Verde
                                5.0f, 5.0f,
                                -14.0f,7.0f,20.0f, //Posición X, Y, Z
                                0.5f, 0.5f, 0.5f);
    pointLightCount++;

    GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
           uniformSpecularIntensity = 0, uniformShininess = 0;
    GLuint uniformColor = 0;
    glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

    ////Loop mientras no se cierra la ventana
    while (!mainWindow.getShouldClose()){
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

        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
        model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));

        pisoTexture.UseTexture();
        Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

        meshList[2]->RenderMesh();

        Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

        // Farol
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(5.0f,0.0f,-25.0f));
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Farol.RenderModel();
        
        // Sable de luz
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(-14.0f,0.0f,20.0f));
        model = glm::scale(model, glm::vec3(1.f, 1.f, 1.f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        SableLuz.RenderModel();
        
        // Helicoptero
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(mainWindow.getMovimientoXHelicoptero(), 15.0f + mainWindow.getMovimientoYHelicoptero(), -1.0 + mainWindow.getMovimientoZHelicoptero()));
        model = glm::scale(model, glm::vec3(2.f, 2.f, 2.f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
        spotLights[2].SetPos(glm::vec3(mainWindow.getMovimientoXHelicoptero(), mainWindow.getMovimientoYHelicoptero(), mainWindow.getMovimientoZHelicoptero()));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        Blackhawk_M.RenderModel();

        // Carroceria
        model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(mainWindow.getMovimientoXAuto(), 0.f, 0.f + mainWindow.getMovimientoZAuto())); // Movimiento del auto
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, -0.f));
        modelaux = model;
        model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
//        spotLights[1].SetPos(glm::vec3(0.0f + mainWindow.getMovimientoXAuto(), 0.0, 0.0f + mainWindow.getMovimientoZAuto()));
        spotLights[1].SetFlash(glm::vec3(0.0f + mainWindow.getMovimientoXAuto(), 0.0, 0.0f + mainWindow.getMovimientoZAuto()), glm::vec3(mainWindow.getDireccionAuto() * 1.0f,0.0f,0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        Vocho.RenderModel();

        // Llanta frente derecha
        model = modelaux;
        model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        //        model = glm::rotate(model, glm::radians(mainWindow.getMovimientoRuedas()), glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        Llanta_Vocho_der_frente.RenderModel();

        // Llanta frente izquierda
        model = modelaux;
        model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        //        model = glm::rotate(model, glm::radians(mainWindow.getMovimientoRuedas()), glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        Llanta_Vocho_izq_frente.RenderModel();

        // Llanta trasera derecha
        model = modelaux;
        model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        //        model = glm::rotate(model, glm::radians(mainWindow.getMovimientoRuedas()), glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        Llanta_Vocho_der_trasera.RenderModel();

        // Llanta trasera izquierda
        model = modelaux;
        model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        //        model = glm::rotate(model, glm::radians(mainWindow.getMovimientoRuedas()), glm::vec3(-1.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        Llanta_Vocho_izq_trasera.RenderModel();

        // Cofre
        //        model = modelaux;
        //        model = glm::translate(model, glm::vec3(-9.5f, 1.6f, -0.f));
        //        model = glm::scale(model, glm::vec3(5.f, 5.f, 5.f));
        //        model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
        //        model = glm::rotate(model, glm::radians(mainWindow.getGiroCofre()), glm::vec3(1.0f, 0.0f, 0.0f));
        //        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        //        glUniform3fv(uniformColor, 1, glm::value_ptr(color));
        //        Vocho.RenderModel();

        glUseProgram(0);

        mainWindow.swapBuffers();
    }

    return 0;
}
