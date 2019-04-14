#version 330

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

uniform vec4 u_color;

uniform sampler2D u_texture_2;
uniform vec2 u_texture_2_size;

uniform float u_normal_scaling;
uniform float u_height_scaling;

in vec4 v_position;
in vec4 v_normal;
in vec4 v_tangent;
in vec2 v_uv;

out vec4 out_color;


float hf(vec2 uv) {
  // You may want to use this helper function...
  return texture(u_texture_2, uv).r;
}

void main() {
  // YOUR CODE HERE
  float kh = u_height_scaling;
  float kn = u_normal_scaling;
  
  float w = u_texture_2_size.x;
  float h = u_texture_2_size.y;
  float u = v_uv.x;
  float v = v_uv.y;
  
  float du = (hf(vec2(u+1/w, v)) - hf(vec2(u, v))) * kh * kn;
  float dv = (hf(vec2(u, v+1/h)) - hf(vec2(u, v))) * kh * kn;
  vec3 n_local = vec3(-du, -dv, 1);
  
  vec3 n_a = v_normal.xyz;
  vec3 t_a = v_tangent.xyz;
  vec3 b_a = cross(n_a, t_a);
  
  vec3 n_world = mat3(t_a, b_a, n_a) * n_local;
  
  float ka = 1;
  float kd = 1;
  float ks = 2;
  vec3 Ia = vec3(0.0,0.0,0.0);
  int p = 1;
  
  vec3 l_vec = normalize(-u_light_pos + v_position.xyz);
  vec3 v_vec = normalize(-u_cam_pos + v_position.xyz);
  float r = length(u_light_pos - v_position.xyz);
  vec3 h_a = (l_vec + v_vec)/length(l_vec + v_vec);
  
  vec3 L = ka*Ia + kd*(u_light_intensity / (r*r)) * max(0, dot(n_world, l_vec)) + ks*(u_light_intensity / (r*r))*pow(max(0, dot(n_world, h_a)), p);
  
  // (Placeholder code. You will want to replace it.)
  out_color = vec4(L, 0)*u_color;
  out_color.a = 1;
}

