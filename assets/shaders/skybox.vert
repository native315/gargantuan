#version 450

layout(set = 1, binding = 0) uniform SkyUniforms {
    mat4 InverseViewProjection;
} sky;

layout(location = 0) out vec3 FragmentDirection;

void main() {
    vec2 uv = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
    vec2 position = (uv * 2.0) - 1.0;

    gl_Position = vec4(position, 1.0, 1.0);

    vec4 world = sky.InverseViewProjection * vec4(position, 1.0, 1.0);
    FragmentDirection = world.xyz / world.w;
}
