#version 450 core
in vec3 vNormal;
out vec4 FragColor;

void main()
{
    vec3 lightDir = normalize(vec3(1.0, 1.2, 0.8));
    float diff = max(dot(normalize(vNormal), lightDir), 0.2);
    vec3 color = vec3(0.2, 0.8, 0.25) * diff;
    FragColor = vec4(color, 1.0);
}
