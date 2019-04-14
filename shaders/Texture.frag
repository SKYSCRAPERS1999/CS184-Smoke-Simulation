#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform sampler2D u_texture_1;

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
  // YOUR CODE HERE
  vec4 tex = texture(u_texture_1, v_uv);
  
  // (Placeholder code. You will want to replace it.)
  out_color = tex;
  out_color.a = 1;
}
