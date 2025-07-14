#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 uView;         // видовая матрица
uniform mat4 uProjection;   // проекционная матрица

out vec3 vDir; // направлениe от камеры к вершине куба

void main() {
    // Убираем перенос, чтобы куб оставался на месте при перемещении камеры
    mat4 rotView = mat4(mat3(uView));
    vDir = aPos; 
    gl_Position = uProjection * rotView * vec4(aPos, 1.0);
}