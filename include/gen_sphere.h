#ifndef MY_GEN_SPHERE
#define MY_GEN_SPHERE

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>

glm::vec3 mirrors[] = {
    glm::vec3( 1.0f,  1.0f, -1.0f),
    glm::vec3( 1.0f, -1.0f,  1.0f),
    glm::vec3( 1.0f, -1.0f, -1.0f),
    glm::vec3(-1.0f,  1.0f,  1.0f),
    glm::vec3(-1.0f,  1.0f, -1.0f),
    glm::vec3(-1.0f, -1.0f,  1.0f),
    glm::vec3(-1.0f, -1.0f, -1.0f)
};

void splitTriangle(glm::vec3 vert1, glm::vec3 vert2, glm::vec3 vert3, std::vector<glm::vec3>& Q, int depth, int& index)
{
    if (depth == 0)
    {
        Q[3*index + 0] = vert1;
        Q[3*index + 1] = vert2;
        Q[3*index + 2] = vert3;
        index += 1;
    }
    else
    {
        glm::vec3 verts[] = {vert1, vert2, vert3};
        glm::vec3 centers[] = {
            (vert1+vert2) / 2.0f,
            (vert2+vert3) / 2.0f,
            (vert1+vert3) / 2.0f
        };
    
        // рекурсивно делим треугольники
        for (int i = 0; i < 3; i++)
            splitTriangle(verts[i], centers[i], centers[(i+2)%3], Q, depth-1, index);
        splitTriangle(centers[0], centers[1], centers[2], Q, depth-1, index);
    }
}

void genSphere(GLfloat* sphere_data, GLfloat radius, int depth)
{
    int size = 3;
    std::vector<glm::vec3> Q(3);
    Q[0] = glm::vec3(1.0f, 0.0f, 0.0f);
    Q[1] = glm::vec3(0.0f, 1.0f, 0.0f);
    Q[2] = glm::vec3(0.0f, 0.0f, 1.0f);
    // Сначала вершин 3 т.е. 1 треугольник
    // После одной итерации вершин 12 т.е. 4 треугольника
    //
    // каждую итерацию массив увеличивается
    size *= glm::pow(4, depth);
    std::vector<glm::vec3> newQ(size);
    int index = 0;
    splitTriangle(Q[0], Q[1], Q[2], newQ, depth, index);
    newQ.swap(Q);
    // Получили четверть. 

    for (int i = 0; i < size; i++)
    {
        Q[i] = radius * glm::normalize(Q[i]);
    }
    // нормализовали

    // теперь нужно отразить
    int oldsize = size;
    size *= 8;
    newQ.resize(size);
    for (int i = 0; i < oldsize; i++)
    {
        newQ[i] = Q[i];
        for (int j = 1; j < 8; j++)
            newQ[oldsize*j + i] = Q[i] * mirrors[j-1];
    }
    newQ.swap(Q);

    // сгенерируем нормали
    // берем по треугольнику
    glm::vec3 norms[size/3];
    for (int i = 0; i < oldsize/3; i++)
    {
        norms[i] = glm::normalize(((Q[3*i+0] + Q[3*i+1]) / 2.0f + Q[3*i+2]) / 2.0f);
        for (int j = 1; j < 8; j++)
            norms[oldsize/3*j + i] = norms[i] * mirrors[j-1];
    }
    // всё
    
    // записываем в выходной вектор по вершине
    for (int i = 0; i < size; i++)
    {
        sphere_data[6*i  ] = Q[i].x;
        sphere_data[6*i+1] = Q[i].y;
        sphere_data[6*i+2] = Q[i].z;
        sphere_data[6*i+3] = norms[i / 3].x;
        sphere_data[6*i+4] = norms[i / 3].y;
        sphere_data[6*i+5] = norms[i / 3].z;
    }
    /* Сначала определяем 6 вершин - концы основных треугольников
     * Потом обределяем сами треугольники
     * Потом рекурсивно на них вызываем функцию дробления и добавляем конечные треугольники в data
     * */
}

#endif  // MY_GEN_SPHERE
