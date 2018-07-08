#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

out vec4 FragColor;

in vec3 Normal;  // Вектор нормали
in vec3 FragPos;

uniform vec3 lightPos; // позиция источника света
uniform vec3 viewPos;  // позиция камеры
uniform Material material; // структура со свойствами материала
uniform Light light;       // структура со свойствами света

void main()
{   
    // attenuation - затухание света
    // вектор от фрагмента до позиции света
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // ambient - цвет при отсутствии света. Перемножаем амбиент материала и света и умножаем на затухание
    vec3 ambient = light.ambient * material.ambient * attenuation;

    // diffuse - рассеивание света
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); // направлен в сторону источника света от фрагмента
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse) * attenuation;

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);  // направлен в сторону камеры от фрагмента
    vec3 reflectDir = reflect(-lightDir, norm);  // требует, чтобы первый аргумент был направлен от источника света к фрагменту
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular) * attenuation;

  
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
