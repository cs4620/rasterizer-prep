#version 330
in vec3 position;
in vec3 normal;
in vec2 uv;

uniform mat4 model;       // world / model matrix
uniform mat4 view_proj;   // view-projection matrix

out vec3 v_normal;
out vec3 v_world_position;
out vec2 v_uv;

void main() {
    // Transform position into world space, then into clip space
    vec4 world_pos = model * vec4(position, 1.0);
    gl_Position = view_proj * world_pos;

    // Pass world-space position and normal to fragment shader
    v_world_position = world_pos.xyz;
    v_normal = normalize(mat3(model) * normal);
    v_uv = uv;
}
