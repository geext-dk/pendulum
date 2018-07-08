#version 330 core
layout (location = 0) in vec3 aPos;    // Первые тр элемента - вектор
layout (location = 1) in vec3 aNormal; // вторые - нормаль в точке

out vec3 FragPos;  // передаём позицию фрагмента во фрагментный шейдер
out vec3 Normal;  // и нормаль

uniform mat4 model;  // Матрица перехода из локальной в глобальную СК
uniform mat4 view;  // из глобальной в СК вида
uniform mat4 projection;  // из СК вида в проекционную
uniform mat3 normalMatrix;  // отдельная для нормалей

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
