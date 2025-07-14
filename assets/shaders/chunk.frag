#version 330 core

in vec3 vFragPos;
in vec3 vNormal;

uniform vec3 uSunDirection;   // направление на солнце (единичный вектор)
uniform vec3 uAmbientColor;   // цвет фонового (ambient) освещения
uniform sampler3D uVoxelMap;  // 3D-текстура блоков мира (occupancy)
uniform vec3 uWorldOrigin;    // смещение мира (начало отсчёта voxel-сетки)
uniform vec3 uWorldSize;      // размер мира (в блоках) для границ трассировки
uniform float uTimeOfDay;     // время суток (может использоваться для эффектов)

out vec4 FragColor;

// Функция трассировки луча (ray marching) от точки p в направлении dir.
// Возвращает 1.0, если луч не пересёк блоки (свет проходит), и 0.0 при затенении.
float TraceShadowRay(vec3 p, vec3 dir) {
    float maxDist = 100.0;       // макс. расстояние трассировки
    float step = 0.5;            // шаг (можно настраивать для быстродействия)
    for(float t = 0.0; t < maxDist; t += step) {
        vec3 pos = p + dir * t;
        // Преобразуем мировую координату в индекс блока в объёме
        vec3 local = (pos - uWorldOrigin) / 1.0; // предполагаем размер блока = 1
        ivec3 idx = ivec3(floor(local));
        // Проверяем границы массива
        if(any(lessThan(idx, ivec3(0))) || any(greaterThanEqual(idx, ivec3(uWorldSize)))) {
            break; // вышли за мир – считаем неокружённым
        }
        // Читаем из 3D-текстуры: значение >0.5 означает наличие блока
        if(texture(uVoxelMap, (local + 0.5) / uWorldSize).r > 0.5) {
            return 0.0; // блок на пути – точка в тени
        }
    }
    return 1.0; // свет дошёл до фрагмента
}

void main() {
    vec3 normal = normalize(vNormal);
    vec3 lightDir = normalize(uSunDirection); // от фрагмента к свету

    // Ламбертовское диффузное освещение: ∝ cosθ (угол между normal и светом):contentReference[oaicite:3]{index=3}.
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1.0); // цвет солнца = белый
    
    // Вычисляем фактор тени (soft shadows): усредняем несколько лучей с небольшим отклонением
    float shadow = 0.0;
    int samples = 4;
    // Небольшие смещения направления для имитации мягкости
    vec3 jitter[4] = vec3[](
        vec3( 0.001,  0.002, -0.001),
        vec3(-0.002,  0.001,  0.002),
        vec3( 0.002, -0.001,  0.001),
        vec3(-0.001, -0.002, -0.002)
    );
    for(int i = 0; i < samples; ++i) {
        // Модифицируем направление света для каждого луча
        vec3 dir = normalize(lightDir + jitter[i]);
        shadow += TraceShadowRay(vFragPos + normal * 0.01, dir);
    }
    shadow /= float(samples);

    // Итоговый цвет: комбинируем ambient и diffuse с учётом тени
    vec3 ambient = uAmbientColor;
    vec3 color = ambient + shadow * diffuse * (1.0 - ambient);
    FragColor = vec4(color, 1.0);
}