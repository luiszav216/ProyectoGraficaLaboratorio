#include <iostream>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include "stb_image.h"

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Load Models
#include "SOIL2/SOIL2.h"

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "modelAnim.h"

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void DoMovement();
void Animation();

// Window dimensions
const GLuint WIDTH = 1800, HEIGHT = 1600;
int SCREEN_WIDTH, SCREEN_HEIGHT;

// Camera
Camera camera(glm::vec3(80.0f, 20.0f, -50.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    180.0f,
    0.0f);

GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;

// Light attributes
glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
bool active;

// Positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.0f,2.0f, 0.0f),
    glm::vec3(0.0f,0.0f, 0.0f),
    glm::vec3(0.0f,0.0f,  0.0f),
    glm::vec3(0.0f,0.0f, 0.0f)
};

// === VAO del “cubo luz” ===
float vertices[] = {
     -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
       -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

glm::vec3 Light1 = glm::vec3(0.0f);

//Anim ball
float rotBall = 0.0f;

// --- Cuadro ladeado + animación de acomodo ---
float cuadroAngle = 8.0f;
bool  cuadroReturn = false;
float cuadroDegPerSec = 60.0f;
glm::vec3 cuadroTopLocal = glm::vec3(2.0f, 40.0f, 10.0f);

// --- Puerta (apertura/cierre con P) ---
float puertaAngle = 0.0f;
float puertaTargetAngle = 0.0f;
float puertaOpenDeg = 90.0f;
float puertaDegPerSec = 90.0f;
bool  puertaAnimating = false;
glm::vec3 puertaHingeLocal = glm::vec3(32.5f, -2.0f, -48.7f);

// --- Cajón (traslación con O) ---
float cajonOffset = 0.0f;
float cajonTarget = 0.0f;
float cajonOpenDist = 3.0f;
float cajonSpeed = 0.8f;
bool  cajonAnimating = false;
glm::vec3 cajonDirLocal = glm::vec3(-1.0f, 0.0f, 0.0f);

// --- SARTÉN + VOLTEADOR ---
glm::vec3 sartenPosInitLocal = glm::vec3(-33.4f, 30.6f, -68.65f);
glm::vec3 volteadorPosInitLocal = glm::vec3(-29.5f, 32.6f, -68.0f);

glm::vec3 sartenPosLocal = sartenPosInitLocal;
glm::vec3 volteadorPosLocal = volteadorPosInitLocal;

glm::vec3 sartenPivotLocalAire = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 sartenPivotLocalSuelo = glm::vec3(0.0f, -0.8f, 0.0f);
glm::vec3 sartenPivotLocalActual = sartenPivotLocalAire;

glm::vec3 volteadorPivotLocalAire = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 volteadorPivotLocalSuelo = glm::vec3(0.0f, -0.5f, 0.0f);
glm::vec3 volteadorPivotLocalActual = volteadorPivotLocalAire;

float sartenRotZ = 0.0f;
float volteadorRotZ = 0.0f;

float sartenRotX = 0.0f;
float sartenRotY = 0.0f;

float volteadorRotX = 0.0f;
float volteadorRotY = 0.0f;

float sartenTargetRotZ = 0.0f;
float sartenTargetRotX = 0.0f;
float sartenTargetRotY = 0.0f;

float volteadorTargetRotZ = 0.0f;
float volteadorTargetRotX = 0.0f;
float volteadorTargetRotY = 0.0f;

float sartenRotSpeed = 180.0f;
float volteadorRotSpeed = 180.0f;
bool  sartenRotating = false;
bool  volteadorRotating = false;

float sartenVelX = 0.0f;
float sartenVelZ = 0.0f;

float sartenInitialSlideSpeedX = 3.5f;
float sartenInitialSlideSpeedZ = 3.8f;

float sartenGroundFriction = 0.6f;

float volteadorVelX = 0.0f;
float volteadorVelZ = 0.0f;

float volteadorInitialSlideSpeedX = 2.5f;
float volteadorInitialSlideSpeedZ = 3.0f;

float volteadorGroundFriction = 0.6f;

bool  utensiliosAnimActive = false;
bool  volteadorAnimActive = false;

float volteadorStartDelay = 3.0f;
float volteadorDelayTimer = 0.0f;
bool  volteadorPendingStart = false;

float alturaSueloPrimeraCaida = 12.0f;
float alturaSueloFinalSarten = 11.4f;
float alturaSueloFinalVolteador = 12.0f;

float sartenVelY = 0.0f;
float sartenGravity = -9.8f;
float sartenBounceFactor = 0.6f;
float sartenMinBounceSpeed = 0.5f;

float volteadorVelY = 0.0f;
float volteadorGravity = -9.8f;
float volteadorBounceFactor = 0.55f;
float volteadorMinBounceSpeed = 0.4f;

bool sartenFirstImpact = false;
bool volteadorFirstImpact = false;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// ===================== CUBO TEXTURIZADO (VAO/VBO + textura) =====================
// Formato: pos(3), normal(3), uv(2)
static const float cubeTexVertices[] = {
    // --- Front (+Z)
    -0.5f,-0.5f, 0.5f,   0,0,1,   0,0,
     0.5f,-0.5f, 0.5f,   0,0,1,   1,0,
     0.5f, 0.5f, 0.5f,   0,0,1,   1,1,
     0.5f, 0.5f, 0.5f,   0,0,1,   1,1,
    -0.5f, 0.5f, 0.5f,   0,0,1,   0,1,
    -0.5f,-0.5f, 0.5f,   0,0,1,   0,0,

    // --- Back (-Z)
    -0.5f,-0.5f,-0.5f,   0,0,-1,  1,0,
    -0.5f, 0.5f,-0.5f,   0,0,-1,  1,1,
     0.5f, 0.5f,-0.5f,   0,0,-1,  0,1,
     0.5f, 0.5f,-0.5f,   0,0,-1,  0,1,
     0.5f,-0.5f,-0.5f,   0,0,-1,  0,0,
    -0.5f,-0.5f,-0.5f,   0,0,-1,  1,0,

    // --- Right (+X)
     0.5f,-0.5f, 0.5f,   1,0,0,   0,0,
     0.5f,-0.5f,-0.5f,   1,0,0,   1,0,
     0.5f, 0.5f,-0.5f,   1,0,0,   1,1,
     0.5f, 0.5f,-0.5f,   1,0,0,   1,1,
     0.5f, 0.5f, 0.5f,   1,0,0,   0,1,
     0.5f,-0.5f, 0.5f,   1,0,0,   0,0,

     // --- Left (-X)
     -0.5f,-0.5f,-0.5f,  -1,0,0,   0,0,
     -0.5f,-0.5f, 0.5f,  -1,0,0,   1,0,
     -0.5f, 0.5f, 0.5f,  -1,0,0,   1,1,
     -0.5f, 0.5f, 0.5f,  -1,0,0,   1,1,
     -0.5f, 0.5f,-0.5f,  -1,0,0,   0,1,
     -0.5f,-0.5f,-0.5f,  -1,0,0,   0,0,

     // --- Bottom (-Y)
      0.5f,-0.5f,-0.5f,   0,-1,0,  1,1,
     -0.5f,-0.5f,-0.5f,   0,-1,0,  0,1,
     -0.5f,-0.5f, 0.5f,   0,-1,0,  0,0,
      0.5f,-0.5f,-0.5f,   0,-1,0,  1,1,
     -0.5f,-0.5f, 0.5f,   0,-1,0,  0,0,
      0.5f,-0.5f, 0.5f,   0,-1,0,  1,0,

      // --- Top (+Y)
      -0.5f, 0.5f, 0.5f,   0,1,0,   0,0,
      -0.5f, 0.5f,-0.5f,   0,1,0,   0,1,
       0.5f, 0.5f,-0.5f,   0,1,0,   1,1,
       0.5f, 0.5f,-0.5f,   0,1,0,   1,1,
       0.5f, 0.5f, 0.5f,   0,1,0,   1,0,
      -0.5f, 0.5f, 0.5f,   0,1,0,   0,0
};

// Handles: declarar SOLO una vez (incluye mesaTexture)
GLuint cubeVAO = 0, cubeVBO = 0, cubeTexture = 0, mesaTexture = 0;

// =======================================================================================

int main()
{
    // Init GLFW
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Animacion maquina de estados", nullptr, nullptr);

    if (nullptr == window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);

    // Callbacks
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return EXIT_FAILURE;
    }

    // Viewport
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    // Shaders
    Shader lightingShader("Shader/lighting.vs", "Shader/lighting.frag");
    Shader lampShader("Shader/lamp.vs", "Shader/lamp.frag");
    Shader animShader("Shader/anim.vs", "Shader/anim.frag");

    // Models estáticos
    Model casa((char*)"Models/casacompleta.obj");
    Model Ball((char*)"Models/ball.obj");
    Model LamparaTecho((char*)"Models/lamparatecho.obj");
    Model Cuadro((char*)"Models/cuadro.obj");
    Model Puerta((char*)"Models/puerta.obj");
    Model Mueble((char*)"Models/mueble.obj");
    Model Cajon((char*)"Models/cajon.obj");

    // Nuevos modelos
    Model Sarten((char*)"Models/sarten.obj");
    Model Volteador((char*)"Models/volteador.obj");

    // Modelo animado por huesos
    ModelAnim Marceline((char*)"Models/marceline3.dae");
    Marceline.initShaders(animShader.Program);

    // === VAO/VBO para cubo luz (ya existente) ===
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Posición
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Material para lightingShader (como lo tenías)
    lightingShader.Use();
    glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.difuse"), 0);
    glUniform1i(glGetUniformLocation(lightingShader.Program, "Material.specular"), 1);

    glm::mat4 projection = glm::perspective(
        camera.GetZoom(),
        (GLfloat)SCREEN_WIDTH / (GLfloat)SCREEN_HEIGHT,
        0.1f,
        100.0f
    );

    // === VAO/VBO del cubo texturizado ===
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexVertices), cubeTexVertices, GL_STATIC_DRAW);

    // Atributos: pos(0), normal(1), texcoord(2)
    GLsizei stride = 8 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // === textura Models/lampara2.png ===
    {
        int w, h, channels;
        unsigned char* data = SOIL_load_image("Models/lampara2.png", &w, &h, &channels, SOIL_LOAD_AUTO);
        if (!data) {
            std::cout << "Error cargando textura Models/lampara2.png" << std::endl;
        }
        else {
            glGenTextures(1, &cubeTexture);
            glBindTexture(GL_TEXTURE_2D, cubeTexture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            GLint internalFormat = (channels == 4) ? GL_RGBA : GL_RGB;
            GLenum  format = (channels == 4) ? GL_RGBA : GL_RGB;

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);
            SOIL_free_image_data(data);
        }
    }

    // === textura Models/mesa.png ===
    {
        int w, h, channels;
        unsigned char* data = SOIL_load_image("Models/mesa.png", &w, &h, &channels, SOIL_LOAD_AUTO);
        if (!data) {
            std::cout << "Error cargando textura Models/mesa.png" << std::endl;
        }
        else {
            glGenTextures(1, &mesaTexture);
            glBindTexture(GL_TEXTURE_2D, mesaTexture);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            GLint internalFormat = (channels == 4) ? GL_RGBA : GL_RGB;
            GLenum  format = (channels == 4) ? GL_RGBA : GL_RGB;

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glBindTexture(GL_TEXTURE_2D, 0);
            SOIL_free_image_data(data);
        }
    }
    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // DeltaTime
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        glfwPollEvents();
        DoMovement();
        Animation();

        // Clear
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // OpenGL options
        glEnable(GL_DEPTH_TEST);

        // === SHADER DE ILUMINACIÓN ===
        lightingShader.Use();

        glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);

        GLint viewPosLoc = glGetUniformLocation(lightingShader.Program, "viewPos");
        glUniform3f(viewPosLoc, camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);

        // Dir light
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.ambient"), 0.6f, 0.6f, 0.6f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.diffuse"), 0.6f, 0.6f, 0.6f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "dirLight.specular"), 0.3f, 0.3f, 0.3f);

        // Point light 1
        glm::vec3 lightColor;
        lightColor.x = std::abs(std::sin(glfwGetTime() * Light1.x));
        lightColor.y = std::abs(std::sin(glfwGetTime() * Light1.y));
        lightColor.z = std::abs(std::sin(glfwGetTime() * Light1.z)); // ← evita negativos

        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].position"),
            pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].ambient"),
            lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].diffuse"),
            lightColor.x, lightColor.y, lightColor.z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "pointLights[0].specular"),
            1.0f, 0.2f, 0.2f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].linear"), 0.045f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "pointLights[0].quadratic"), 0.075f);

        // SpotLight
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.position"),
            camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.direction"),
            camera.GetFront().x, camera.GetFront().y, camera.GetFront().z);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.ambient"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.diffuse"), 0.2f, 0.2f, 0.8f);
        glUniform3f(glGetUniformLocation(lightingShader.Program, "spotLight.specular"), 0.0f, 0.0f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.linear"), 0.3f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.quadratic"), 0.7f);
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.cutOff"), glm::cos(glm::radians(12.0f)));
        glUniform1f(glGetUniformLocation(lightingShader.Program, "spotLight.outerCutOff"), glm::cos(glm::radians(18.0f)));

        glUniform1f(glGetUniformLocation(lightingShader.Program, "material.shininess"), 5.0f);

        glm::mat4 view = camera.GetViewMatrix();

        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc = glGetUniformLocation(lightingShader.Program, "view");
        GLint projLoc = glGetUniformLocation(lightingShader.Program, "projection");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 model(1.0f);

        // ===================== DIBUJAR CUBO(S) TEXTURIZADO(S) =====================
        {
            // Sampler en la unidad 0 (compatibilidad con tu shader)
            glActiveTexture(GL_TEXTURE0);

            // Por si tu shader usa "diffuse" o "material.diffuse"
            glUniform1i(glGetUniformLocation(lightingShader.Program, "diffuse"), 0);
            GLint matDiff = glGetUniformLocation(lightingShader.Program, "material.diffuse");
            if (matDiff >= 0) glUniform1i(matDiff, 0);

            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);

            glBindVertexArray(cubeVAO);

            // --- CUBO 1: lampara2 ---
            glBindTexture(GL_TEXTURE_2D, cubeTexture);
            glm::mat4 m1(1.0f);
            m1 = glm::translate(m1, glm::vec3(20.0f, 17.0f, -71.0f));
            m1 = glm::scale(m1, glm::vec3(15.0f, 15.0f, 3.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m1));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // --- CUBO 2: lampara2 ---
            glBindTexture(GL_TEXTURE_2D, cubeTexture);
            glm::mat4 m2(1.0f);
            m2 = glm::translate(m2, glm::vec3(20.0f, 12.5f, -67.0f));
            m2 = glm::scale(m2, glm::vec3(15.0f, 6.0f, 6.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m2));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // --- CUBO 3: lampara2 (pilar izq) ---
            glBindTexture(GL_TEXTURE_2D, cubeTexture);
            glm::mat4 m3(1.0f);
            m3 = glm::translate(m3, glm::vec3(12.0f, 14.0f, -67.0f));
            m3 = glm::scale(m3, glm::vec3(3.0f, 9.5f, 6.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m3));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // --- CUBO 4: lampara2 (pilar der) ---
            glBindTexture(GL_TEXTURE_2D, cubeTexture);
            glm::mat4 m4(1.0f);
            m4 = glm::translate(m4, glm::vec3(28.0f, 14.0f, -67.0f));
            m4 = glm::scale(m4, glm::vec3(3.0f, 9.5f, 6.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m4));
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // --- CUBO 5: mesa (usa la textura mesaTexture) ---
            glBindTexture(GL_TEXTURE_2D, mesaTexture);
            glm::mat4 m5(1.0f);
            m5 = glm::translate(m5, glm::vec3(-30.0f, 13.0f, -9.0f));
            m5 = glm::scale(m5, glm::vec3(1.0f, 6.0f, 1.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m5)); // ← CORREGIDO (antes subías m3)
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // --- CUBO 6: mesa (usa la textura mesaTexture) ---
            glBindTexture(GL_TEXTURE_2D, mesaTexture);
            glm::mat4 m6(1.0f);
            m6 = glm::translate(m6, glm::vec3(-32.0f, 15.75f, 0.0f));
            m6 = glm::scale(m6, glm::vec3(6.0f, 1.0f, 20.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m6)); // ← CORREGIDO (antes subías m3)
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // --- CUBO 5: mesa (usa la textura mesaTexture) ---
            glBindTexture(GL_TEXTURE_2D, mesaTexture);
            glm::mat4 m7(1.0f);
            m7 = glm::translate(m7, glm::vec3(-34.0f, 13.0f, -9.0f));
            m7 = glm::scale(m7, glm::vec3(1.0f, 6.0f, 1.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m7)); // ← CORREGIDO (antes subías m3)
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // --- CUBO 5: mesa (usa la textura mesaTexture) ---
            glBindTexture(GL_TEXTURE_2D, mesaTexture);
            glm::mat4 m8(1.0f);
            m8 = glm::translate(m8, glm::vec3(-34.0f, 13.0f, 9.0f));
            m8 = glm::scale(m8, glm::vec3(1.0f, 6.0f, 1.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m8)); // ← CORREGIDO (antes subías m3)
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // --- CUBO 5: mesa (usa la textura mesaTexture) ---
            glBindTexture(GL_TEXTURE_2D, mesaTexture);
            glm::mat4 m9(1.0f);
            m9 = glm::translate(m9, glm::vec3(-30.0f, 13.0f, 9.0f));
            m9 = glm::scale(m9, glm::vec3(1.0f, 6.0f, 1.0f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m9)); // ← CORREGIDO (antes subías m3)
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glBindVertexArray(0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        // ===========================================================================

        // --- CASA ---
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, -3.0f));
        model = glm::rotate(model, glm::radians(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f));

        glm::mat4 casaModel = model;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(casaModel));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        casa.Draw(lightingShader);

        // --- LÁMPARA DE TECHO ---
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(casaModel));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        LamparaTecho.Draw(lightingShader);

        // --- MUEBLE ---
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(casaModel));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
        Mueble.Draw(lightingShader);

        // --- CAJÓN ---
        {
            glm::mat4 cajonLocal = glm::translate(glm::mat4(1.0f), cajonDirLocal * cajonOffset);
            glm::mat4 modelCajon = casaModel * cajonLocal;
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCajon));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            Cajon.Draw(lightingShader);
        }

        // --- PUERTA ---
        {
            glm::vec4 hingeWorld = casaModel * glm::vec4(puertaHingeLocal, 1.0f);

            glm::mat4 puertaLocal = glm::mat4(1.0f);
            puertaLocal = glm::translate(puertaLocal, puertaHingeLocal);
            puertaLocal = glm::rotate(puertaLocal, glm::radians(puertaAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            puertaLocal = glm::translate(puertaLocal, -puertaHingeLocal);

            glm::mat4 modelPuerta = casaModel * puertaLocal;
            glm::vec4 newHingeWorld = modelPuerta * glm::vec4(puertaHingeLocal, 1.0f);

            glm::vec3 deltaH = glm::vec3(hingeWorld - newHingeWorld);
            modelPuerta = glm::translate(glm::mat4(1.0f), deltaH) * modelPuerta;

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPuerta));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            Puerta.Draw(lightingShader);
        }

        // --- CUADRO ---
        {
            glm::vec4 anchorWorld = casaModel * glm::vec4(cuadroTopLocal, 1.0f);
            glm::mat4 cuadroLocal = glm::mat4(1.0f);
            cuadroLocal = glm::translate(cuadroLocal, cuadroTopLocal);
            cuadroLocal = glm::rotate(cuadroLocal, glm::radians(cuadroAngle), glm::vec3(0.0f, 0.0f, 1.0f));
            cuadroLocal = glm::translate(cuadroLocal, -cuadroTopLocal);

            glm::mat4 modelCuadro = casaModel * cuadroLocal;
            glm::vec4 newAnchorWorld = modelCuadro * glm::vec4(cuadroTopLocal, 1.0f);
            glm::vec3 delta = glm::vec3(anchorWorld - newAnchorWorld);
            modelCuadro = glm::translate(glm::mat4(1.0f), delta) * modelCuadro;

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCuadro));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            Cuadro.Draw(lightingShader);
        }

        // --- SARTÉN & VOLTEADOR ---
        {
            // Sartén
            glm::mat4 modelSarten = casaModel;
            modelSarten = glm::translate(modelSarten, sartenPosLocal);
            modelSarten = glm::translate(modelSarten, sartenPivotLocalActual);
            modelSarten = glm::rotate(modelSarten, glm::radians(sartenRotX), glm::vec3(1.0f, 0.0f, 0.0f));
            modelSarten = glm::rotate(modelSarten, glm::radians(sartenRotY), glm::vec3(0.0f, 1.0f, 0.0f));
            modelSarten = glm::rotate(modelSarten, glm::radians(sartenRotZ), glm::vec3(0.0f, 0.0f, 1.0f));
            modelSarten = glm::translate(modelSarten, -sartenPivotLocalActual);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSarten));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            Sarten.Draw(lightingShader);

            // Volteador
            glm::mat4 modelVolteador = casaModel;
            modelVolteador = glm::translate(modelVolteador, volteadorPosLocal);
            modelVolteador = glm::translate(modelVolteador, volteadorPivotLocalActual);
            modelVolteador = glm::rotate(modelVolteador, glm::radians(volteadorRotX), glm::vec3(1.0f, 0.0f, 0.0f));
            modelVolteador = glm::rotate(modelVolteador, glm::radians(volteadorRotY), glm::vec3(0.0f, 1.0f, 0.0f));
            modelVolteador = glm::rotate(modelVolteador, glm::radians(volteadorRotZ), glm::vec3(0.0f, 0.0f, 1.0f));
            modelVolteador = glm::translate(modelVolteador, -volteadorPivotLocalActual);
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelVolteador));
            glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 0);
            Volteador.Draw(lightingShader);
        }

        // --- MARCELINE (huesos) ---
        {
            animShader.Use();

            GLint aModelLoc = glGetUniformLocation(animShader.Program, "model");
            GLint aViewLoc = glGetUniformLocation(animShader.Program, "view");
            GLint aProjLoc = glGetUniformLocation(animShader.Program, "projection");

            glUniformMatrix4fv(aViewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(aProjLoc, 1, GL_FALSE, glm::value_ptr(projection));

            glUniform3f(glGetUniformLocation(animShader.Program, "light.direction"), -0.2f, -1.0f, -0.3f);
            glUniform3f(glGetUniformLocation(animShader.Program, "light.ambient"), 0.6f, 0.6f, 0.6f);
            glUniform3f(glGetUniformLocation(animShader.Program, "light.diffuse"), 0.6f, 0.6f, 0.6f);
            glUniform3f(glGetUniformLocation(animShader.Program, "light.specular"), 0.3f, 0.3f, 0.3f);

            glUniform3f(glGetUniformLocation(animShader.Program, "viewPos"),
                camera.GetPosition().x,
                camera.GetPosition().y,
                camera.GetPosition().z);

            glUniform3f(glGetUniformLocation(animShader.Program, "material.specular"), 0.5f, 0.5f, 0.5f);
            glUniform1f(glGetUniformLocation(animShader.Program, "material.shininess"), 16.0f);

            glm::mat4 modelMarceline = glm::mat4(1.0f);
            modelMarceline = glm::translate(modelMarceline, glm::vec3(6.0f, 10.0f, -5.0f));
            modelMarceline = glm::scale(modelMarceline, glm::vec3(14.0f));

            glUniformMatrix4fv(aModelLoc, 1, GL_FALSE, glm::value_ptr(modelMarceline));
            Marceline.Draw(animShader);
        }

        // Volvemos a lightingShader para BALL y lámpara
        lightingShader.Use();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // --- BALL ---
        model = glm::mat4(1.0f);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        model = glm::rotate(model, glm::radians(rotBall), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(glGetUniformLocation(lightingShader.Program, "transparency"), 1);
        Ball.Draw(lightingShader);
        glDisable(GL_BLEND);

        glBindVertexArray(0);

        // --- LAMP SHADER ---
        lampShader.Use();
        GLint lampModelLoc = glGetUniformLocation(lampShader.Program, "model");
        GLint viewLoc2 = glGetUniformLocation(lampShader.Program, "view");
        GLint projLoc2 = glGetUniformLocation(lampShader.Program, "projection");

        glUniformMatrix4fv(viewLoc2, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc2, 1, GL_FALSE, glm::value_ptr(projection));

        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[0]);
        model = glm::scale(model, glm::vec3(0.2f));
        glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Dibuja el cubito de la lámpara (VBO/VAO “vertices” original)
     //   glBindVertexArray(VAO);
       // glDrawArrays(GL_TRIANGLES, 0, 36);
        //glBindVertexArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Limpieza
    if (cubeVBO) glDeleteBuffers(1, &cubeVBO);
    if (cubeVAO) glDeleteVertexArrays(1, &cubeVAO);
    if (cubeTexture) glDeleteTextures(1, &cubeTexture);
    if (mesaTexture) glDeleteTextures(1, &mesaTexture); // ← añadido

    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions based on user input
void DoMovement()
{
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) { camera.ProcessKeyboard(FORWARD, deltaTime); }
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) { camera.ProcessKeyboard(BACKWARD, deltaTime); }
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) { camera.ProcessKeyboard(LEFT, deltaTime); }
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) { camera.ProcessKeyboard(RIGHT, deltaTime); }

    // Luz
    if (keys[GLFW_KEY_T]) { pointLightPositions[0].x += 0.01f; }
    if (keys[GLFW_KEY_G]) { pointLightPositions[0].x -= 0.01f; }
    if (keys[GLFW_KEY_Y]) { pointLightPositions[0].y += 0.01f; }
    if (keys[GLFW_KEY_H]) { pointLightPositions[0].y -= 0.01f; }
    if (keys[GLFW_KEY_U]) { pointLightPositions[0].z -= 0.01f; }
    if (keys[GLFW_KEY_J]) { pointLightPositions[0].z += 0.01f; }

    // Movimiento vertical de la cámara
    if (keys[GLFW_KEY_SPACE]) { camera.ProcessKeyboard(UP, deltaTime); }
    if (keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT]) { camera.ProcessKeyboard(DOWN, deltaTime); }
}


// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // Tecla C: acomodo cuadro
    if (keys[GLFW_KEY_C]) {
        cuadroReturn = true;
    }

    // Tecla P: puerta
    if (keys[GLFW_KEY_P]) {
        if (puertaTargetAngle == 0.0f) puertaTargetAngle = puertaOpenDeg;
        else                           puertaTargetAngle = 0.0f;
        puertaAnimating = true;
    }

    // Tecla O: cajón
    if (keys[GLFW_KEY_O]) {
        if (cajonTarget == 0.0f) cajonTarget = cajonOpenDist;
        else                     cajonTarget = 0.0f;
        cajonAnimating = true;
    }

    // Tecla K: disparar caída/rebote del sartén y volteador
    if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        // Sartén
        utensiliosAnimActive = true;

        sartenPosLocal = sartenPosInitLocal;
        sartenVelY = 0.0f;
        sartenVelX = 0.0f;
        sartenVelZ = 0.0f;

        sartenRotX = 0.0f;
        sartenRotY = 0.0f;
        sartenRotZ = 0.0f;

        sartenTargetRotX = 0.0f;
        sartenTargetRotY = 0.0f;
        sartenTargetRotZ = 0.0f;

        sartenRotating = false;
        sartenFirstImpact = false;
        sartenPivotLocalActual = sartenPivotLocalAire;

        // Volteador: se resetea pero comenzará después (delay)
        volteadorPosLocal = volteadorPosInitLocal;
        volteadorVelY = 0.0f;
        volteadorVelX = 0.0f;
        volteadorVelZ = 0.0f;

        volteadorRotX = 0.0f;
        volteadorRotY = 0.0f;
        volteadorRotZ = 0.0f;

        volteadorTargetRotX = 0.0f;
        volteadorTargetRotY = 0.0f;
        volteadorTargetRotZ = 0.0f;

        volteadorRotating = false;
        volteadorFirstImpact = false;
        volteadorPivotLocalActual = volteadorPivotLocalAire;

        volteadorAnimActive = false;
        volteadorPendingStart = true;
        volteadorDelayTimer = volteadorStartDelay;
    }

    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)        keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }

    if (keys[GLFW_KEY_SPACE])
    {
        active = !active;
        if (active)
            Light1 = glm::vec3(0.2f, 0.8f, 1.0f);
        else
            Light1 = glm::vec3(0.0f);
    }
}

void Animation()
{
    // Cuadro
    if (cuadroReturn) {
        if (cuadroAngle > 0.0f) {
            cuadroAngle -= cuadroDegPerSec * deltaTime;
            if (cuadroAngle < 0.0f) cuadroAngle = 0.0f;
        }
        else {
            cuadroReturn = false;
        }
    }

    // Puerta
    if (puertaAnimating) {
        float diff = puertaTargetAngle - puertaAngle;
        float step = puertaDegPerSec * deltaTime;
        if (std::abs(diff) <= step) {
            puertaAngle = puertaTargetAngle;
            puertaAnimating = false;
        }
        else {
            puertaAngle += (diff > 0.0f ? step : -step);
        }
    }

    // Cajón
    if (cajonAnimating) {
        float diff = cajonTarget - cajonOffset;
        float step = cajonSpeed * deltaTime;
        if (std::abs(diff) <= step) {
            cajonOffset = cajonTarget;
            cajonAnimating = false;
        }
        else {
            cajonOffset += (diff > 0.0f ? step : -step);
        }
    }

    // Delay para que el VOLTEADOR empiece después del sartén
    if (volteadorPendingStart) {
        volteadorDelayTimer -= deltaTime;
        if (volteadorDelayTimer <= 0.0f) {
            volteadorPendingStart = false;
            volteadorAnimActive = true;
        }
    }

    // Sartén: movimiento con rebote
    if (utensiliosAnimActive)
    {
        sartenVelY += sartenGravity * deltaTime;
        sartenPosLocal.y += sartenVelY * deltaTime;

        sartenPosLocal.x += sartenVelX * deltaTime;
        sartenPosLocal.z += sartenVelZ * deltaTime;

        float sueloActualY = (!sartenFirstImpact ? alturaSueloPrimeraCaida : alturaSueloFinalSarten);

        if (sartenPosLocal.y <= sueloActualY)
        {
            sartenPosLocal.y = sueloActualY;

            if (!sartenFirstImpact)
            {
                sartenTargetRotZ = sartenRotZ + -90.0f;
                sartenTargetRotX = sartenRotX + 90.0f;
                sartenTargetRotY = sartenRotY + -5.0f;

                sartenRotating = true;
                sartenPivotLocalActual = sartenPivotLocalSuelo;

                sartenVelX = sartenInitialSlideSpeedX;
                sartenVelZ = sartenInitialSlideSpeedZ;

                sartenFirstImpact = true;
            }

            sartenVelX *= sartenGroundFriction;
            sartenVelZ *= sartenGroundFriction;

            if (std::abs(sartenVelX) < 0.05f) sartenVelX = 0.0f;
            if (std::abs(sartenVelZ) < 0.05f) sartenVelZ = 0.0f;

            sartenVelY = -sartenVelY * sartenBounceFactor;

            if (std::abs(sartenVelY) < sartenMinBounceSpeed)
            {
                sartenVelY = 0.0f;
                utensiliosAnimActive = false;
                sartenPosLocal.y = alturaSueloFinalSarten;
            }
        }
    }

    // VOLTEADOR: movimiento con rebote
    if (volteadorAnimActive)
    {
        volteadorVelY += volteadorGravity * deltaTime;
        volteadorPosLocal.y += volteadorVelY * deltaTime;

        volteadorPosLocal.x += volteadorVelX * deltaTime;
        volteadorPosLocal.z += volteadorVelZ * deltaTime;

        float sueloActualY = (!volteadorFirstImpact ? alturaSueloPrimeraCaida : alturaSueloFinalVolteador);

        if (volteadorPosLocal.y <= sueloActualY)
        {
            volteadorPosLocal.y = sueloActualY;

            if (!volteadorFirstImpact)
            {
                volteadorTargetRotZ = volteadorRotZ + -80.0f;
                volteadorTargetRotX = volteadorRotX + 75.0f;
                volteadorTargetRotY = volteadorRotY + -30.0f;

                volteadorRotating = true;
                volteadorPivotLocalActual = volteadorPivotLocalSuelo;

                volteadorVelX = volteadorInitialSlideSpeedX;
                volteadorVelZ = volteadorInitialSlideSpeedZ;

                volteadorFirstImpact = true;
            }

            volteadorVelX *= volteadorGroundFriction;
            volteadorVelZ *= volteadorGroundFriction;

            if (std::abs(volteadorVelX) < 0.05f) volteadorVelX = 0.0f;
            if (std::abs(volteadorVelZ) < 0.05f) volteadorVelZ = 0.0f;

            volteadorVelY = -volteadorVelY * volteadorBounceFactor;

            if (std::abs(volteadorVelY) < volteadorMinBounceSpeed)
            {
                volteadorVelY = 0.0f;
                volteadorAnimActive = false;
                volteadorPosLocal.y = alturaSueloFinalVolteador;
            }
        }
    }

    // Rotación suave del sartén hacia los ángulos objetivo
    if (sartenRotating)
    {
        float step = sartenRotSpeed * deltaTime;

        float diffZ = sartenTargetRotZ - sartenRotZ;
        if (std::abs(diffZ) <= step) sartenRotZ = sartenTargetRotZ;
        else                         sartenRotZ += (diffZ > 0.0f ? step : -step);

        float diffX = sartenTargetRotX - sartenRotX;
        if (std::abs(diffX) <= step) sartenRotX = sartenTargetRotX;
        else                         sartenRotX += (diffX > 0.0f ? step : -step);

        float diffY = sartenTargetRotY - sartenRotY;
        if (std::abs(diffY) <= step) sartenRotY = sartenTargetRotY;
        else                         sartenRotY += (diffY > 0.0f ? step : -step);

        if (sartenRotZ == sartenTargetRotZ &&
            sartenRotX == sartenTargetRotX &&
            sartenRotY == sartenTargetRotY)
        {
            sartenRotating = false;
        }
    }

    // Rotación suave del volteador hacia los ángulos objetivo
    if (volteadorRotating)
    {
        float step = volteadorRotSpeed * deltaTime;

        float diffZ = volteadorTargetRotZ - volteadorRotZ;
        if (std::abs(diffZ) <= step) volteadorRotZ = volteadorTargetRotZ;
        else                         volteadorRotZ += (diffZ > 0.0f ? step : -step);

        float diffX = volteadorTargetRotX - volteadorRotX;
        if (std::abs(diffX) <= step) volteadorRotX = volteadorTargetRotX;
        else                         volteadorRotX += (diffX > 0.0f ? step : -step);

        float diffY = volteadorTargetRotY - volteadorRotY;
        if (std::abs(diffY) <= step) volteadorRotY = volteadorTargetRotY;
        else                         volteadorRotY += (diffY > 0.0f ? step : -step);

        if (volteadorRotZ == volteadorTargetRotZ &&
            volteadorRotX == volteadorTargetRotX &&
            volteadorRotY == volteadorTargetRotY)
        {
            volteadorRotating = false;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xOffset, yOffset);
}
