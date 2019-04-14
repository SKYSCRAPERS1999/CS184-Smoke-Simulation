#version 330

uniform vec4 u_color;
uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform vec3 u_light_intensity;

in vec4 v_position;
in vec4 v_normal;
in vec2 v_uv;

out vec4 out_color;

void main() {
  // YOUR CODE HERE
  float ka = 1;
  float kd = 1;
  float ks = 1;
  vec3 Ia = vec3(0.2,0.2,0.2);
  int p = 1;
  
  vec3 l_vec = normalize(-u_light_pos + v_position.xyz);
  vec3 v_vec = normalize(-u_cam_pos + v_position.xyz);
  float r = length(u_light_pos - v_position.xyz);
  vec3 h = (l_vec + v_vec)/length(l_vec + v_vec);
  
  vec3 L = ka*Ia + kd*(u_light_intensity / (r*r)) * max(0, dot(v_normal.xyz, l_vec)) + ks*(u_light_intensity / (r*r))*pow(max(0, dot(v_normal.xyz, h)), p);
    
  
  // (Placeholder code. You will want to replace it.)
  out_color = u_color * vec4(L, 0);
  out_color.a = 1;
}
