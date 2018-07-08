#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/shader.h"
#include "../include/camera.h"
#include "../include/gen_sphere.h"
#include <iostream>
#include <vector>

const GLfloat SPHERE_MASS = 100.0f; // масса сферы
const GLfloat THREAD_LENGTH = 5.0f;  // длинна нити

// Начальный вектор скорости и точка сферы
glm::vec3 speedVec(0.0f);
glm::vec3 basePos(0.0f, 6.0f, 0.0f);
glm::vec3 spherePos(4.0f, 3.0f, 0.0f);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const GLuint SCR_WIDTH = 1280;
const GLuint SCR_HEIGHT = 720;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
GLfloat lastX = SCR_WIDTH / 2.0f;
GLfloat lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pendulum", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("shaders/shader.vert", "shaders/shader.frag");
    Shader lampShader("shaders/light.vert", "shaders/light.frag");

    GLfloat cube[] = {
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.0f, -0.5f,  0.0f,  0.0f,  1.0f, 
         0.5f,  1.0f, -0.5f,  0.0f,  0.0f,  1.0f, 
         0.5f,  1.0f, -0.5f,  0.0f,  0.0f,  1.0f, 
        -0.5f,  1.0f, -0.5f,  0.0f,  0.0f,  1.0f, 
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f,  1.0f, 
            
        -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  1.0f,  0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  1.0f,  0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  1.0f,  0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f,  1.0f,  0.5f,  1.0f,  0.0f,  0.0f,
        -0.5f,  1.0f, -0.5f,  1.0f,  0.0f,  0.0f,
        -0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,
        -0.5f,  0.0f, -0.5f,  1.0f,  0.0f,  0.0f,
        -0.5f,  0.0f,  0.5f,  1.0f,  0.0f,  0.0f,
        -0.5f,  1.0f,  0.5f,  1.0f,  0.0f,  0.0f,

         0.5f,  1.0f,  0.5f, -1.0f,  0.0f,  0.0f,
         0.5f,  1.0f, -0.5f, -1.0f,  0.0f,  0.0f,
         0.5f,  0.0f, -0.5f, -1.0f,  0.0f,  0.0f,
         0.5f,  0.0f, -0.5f, -1.0f,  0.0f,  0.0f,
         0.5f,  0.0f,  0.5f, -1.0f,  0.0f,  0.0f,
         0.5f,  1.0f,  0.5f, -1.0f,  0.0f,  0.0f,
    
        -0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.0f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.0f, -0.5f,  0.0f,  1.0f,  0.0f,
    
        -0.5f,  1.0f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f,  1.0f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f,  1.0f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f,  1.0f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f,  1.0f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f,  1.0f, -0.5f,  0.0f, -1.0f,  0.0f
    };


    int iter = 4;
    // sphere должен быть размера 8 * pow(4, n_iter) * 3 * 6
    int size = 8 * glm::pow(4, iter) * 3 * 6;
    GLfloat* sphere = new GLfloat[size];
    genSphere(sphere, 0.5f, iter);

    GLuint VBO_sphere, VBO_cube, ballVAO, lightVAO, roomVAO, threadVAO;
    glGenBuffers(1, &VBO_sphere);
    glGenBuffers(1, &VBO_cube);
    glGenVertexArrays(1, &ballVAO);
    glGenVertexArrays(1, &lightVAO);
    glGenVertexArrays(1, &roomVAO);
    glGenVertexArrays(1, &threadVAO);

// Ball
    glBindVertexArray(ballVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_sphere);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere[0]) * size, sphere, GL_STATIC_DRAW);
    delete[] sphere;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

// light source
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

// Room
    glBindVertexArray(roomVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

// Thread
    glBindVertexArray(threadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_cube);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    glm::vec3 g = glm::vec3(0.0f, -1.0f * 9.8f * SPHERE_MASS, 0.0f);
    glm::vec3 baseDir(basePos - spherePos);
    glm::vec3 t_vec(glm::normalize(-1.0f * baseDir));
    float g_angle = glm::dot(t_vec, glm::normalize(g));

    while (!glfwWindowShouldClose(window))
    {
        // расчитываем время между кадрами
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        lightPos = spherePos;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// room
        lightingShader.use();
        // Позиция источника света
        lightingShader.setVec3("lightPos", lightPos);
        // Проекция
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);
        // Вид
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("view", view);
        // Перенос объекта
        glm::mat4 model(1.0f);
        model = glm::scale(model, glm::vec3(30.0f, 6.0f, 30.0f));
        //model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
        lightingShader.setMat4("model", model);
        // Перенос нормалей
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        lightingShader.setMat3("normalMatrix", normalMatrix);

        
        // Положение камеры
        lightingShader.setVec3("viewPos", camera.Position);

        // свойства материала
        lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);  // цвет в темноте
        lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);  // рассеивание света
        lightingShader.setVec3("material.specular", 0.0f, 0.0f, 0.0f);  // отражение света
        lightingShader.setFloat("material.shininess", 32.0f);  // яркость (степень двойки)

        // цвет источника света
        glm::vec3 lightColor;
        lightColor.r = 1.0f;
        lightColor.g = 1.0f;
        lightColor.b = 1.0f;

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f);
        lightingShader.setVec3("light.ambient", ambientColor);
        lightingShader.setVec3("light.diffuse", diffuseColor);
        lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        // Затухание света с расстоянием
        lightingShader.setFloat("light.constant", 1.0f);
        lightingShader.setFloat("light.linear", 0.09f);
        lightingShader.setFloat("light.quadratic", 0.032f);

        glBindVertexArray(roomVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

// sphere
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);

        // установим отражение света
        //lightingShader.setVec3("material.specular", 0.3f, 0.3f, 0.3f);

        // Маятник - как он двигается?
        // 1) Есть сила, которая равна массе на ускорение.
        // 2) К вектору скорости прибавляется вектор силы, деленный на массу (т. е. ускорение)
        // 3) Есть вектор скорости - к положению шара каждый раз прибавляется вектор скорости
        // Силы:
        // 1. Сила тяжести. Направлена всегда вниз
        // 2. Сила натяжения нити. Равна сумме всех сил, действующих на нить, и противоположна по направлению
        // (Но только если расстояние между опорой и шаром не меньше длинны нити)
        glm::vec3 T(0.0f);

        // a) Находим проекцию силы тяжести на ось нити
        glm::vec3 g_cos = g_angle * glm::length(g) * t_vec;

        // б) присваиваем всё к вектору силы натяжения и инвертируем 
        T = -1.0f * g_cos;

        // 3. Расчитываем каждую итерацию ускорение
        glm::vec3 accel = (g + T) / SPHERE_MASS;  // получили вектор ускорения
        glm::vec3 oldSpherePos(spherePos);
        speedVec += accel;
        spherePos += speedVec * 0.001f;

        // Приближаем шар к основанию нити на длинну нити
        glm::vec3 Base_Sphere_Vec(spherePos - basePos);
        glm::vec3 rightBSVec(glm::normalize(Base_Sphere_Vec) * THREAD_LENGTH);
        glm::vec3 true_position(rightBSVec + basePos);
        spherePos = true_position;
        speedVec = (true_position - oldSpherePos) * 1000.0f;
        
        model = glm::translate(glm::mat4(1.0f), spherePos);
        lightingShader.setMat4("model", model);
        
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        lightingShader.setMat3("normalMatrix", normalMatrix);

        glBindVertexArray(ballVAO);
        glDrawArrays(GL_TRIANGLES, 0, size / 6);
    
// thread
        lightingShader.use();

        model = glm::mat4(1.0f);

        baseDir = basePos - spherePos;
        t_vec = glm::normalize(-1.0f * baseDir);
        g_angle = glm::dot(t_vec, glm::normalize(g));
        
        float angle = glm::acos(-g_angle);
        if (spherePos.x > 0)
            angle *= -1.0f;
        model = glm::translate(model, glm::vec3(0.0f, 6.0f, 0.0f));
        model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f, THREAD_LENGTH, 0.05f));
        lightingShader.setMat4("model", model);
        normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
        lightingShader.setMat3("normalMatrix", normalMatrix);
        lightingShader.use();

        glBindVertexArray(threadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &ballVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteVertexArrays(1, &roomVAO);
    glDeleteVertexArrays(1, &threadVAO);
    glDeleteBuffers(1, &VBO_sphere);
    glDeleteBuffers(1, &VBO_cube);

    // Выключаемся
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* /*window*/, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
