#version 330 core

in vec3 vDir;
uniform vec3 uSunDirection;   // направление солнца
out vec4 FragColor;

void main() {
    vec3 dir = normalize(vDir);
    // Горизонтальный и верхний цвета неба
    vec3 horizonColor = vec3(1.0, 0.7, 0.3); // тёплый оранжевый
    vec3 zenithColor  = vec3(0.2, 0.5, 1.0); // голубой
    // Интерполяция цвета по высоте взгляда (видимая высота неба)
    float t = pow(max(dir.y, 0.0), 0.5);
    vec3 skyColor = mix(horizonColor, zenithColor, t);

    // Подсветка области вокруг солнца «диском»:
    float sunDot = max(dot(dir, normalize(uSunDirection)), 0.0);
    // Чем ближе взгляд к направлению солнца, тем ярче цвет
    skyColor = mix(skyColor, vec3(1.0, 0.9, 0.6), pow(sunDot, 100.0));

    FragColor = vec4(skyColor, 1.0);
}