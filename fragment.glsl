#version 330
in vec3 v_normal;
in vec3 v_world_position;
in vec2 v_uv;

out vec4 frag_color;

void main() {
    // Light direction (pointing from upper-right-front)
    vec3 light_dir = normalize(vec3(1.0, 1.0, 1.0));

    // Normalize the interpolated normal
    vec3 n = normalize(v_normal);

    // Diffuse lighting: max(dot(N, L), 0)
    float diffuse = max(dot(n, light_dir), 0.0);

    // Add a small ambient term so the dark side isn't pure black
    float ambient = 0.15;

    // Base color of the sphere
    vec3 base_color = vec3(v_uv, 1.0);

    vec3 color = base_color * (ambient + diffuse);
    frag_color = vec4(color, 1.0);
}
