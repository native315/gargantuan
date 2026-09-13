#version 450

layout(location = 0) in vec3 FragmentDirection;

layout(location = 0) out vec4 OutputColor;

layout(set = 2, binding = 0) uniform samplerCube Skybox;

void main() {
    vec3 direction = normalize(FragmentDirection);

    OutputColor = texture(Skybox, vec3(-direction.x, direction.y, direction.z));
}
