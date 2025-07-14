#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vFragPos;   // мировая позиция фрагмента
out vec3 vNormal;    // мировая нормаль

void main() {
    // Вычисляем мировую позицию и нормаль
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    // Для корректной нормали используем транспонированную обратную матрицу модели
    vNormal = mat3(transpose(inverse(uModel))) * aNormal;
    
    gl_Position = uProjection * uView * worldPos;
}