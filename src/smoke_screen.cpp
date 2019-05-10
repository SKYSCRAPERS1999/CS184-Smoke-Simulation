//
// Created by impulse on 2019-05-10.
//
#include "common.h"
#include "smoke_screen.h"

void SmokeScreen::reset_parameters() {

  Con::size_smoke = 6;
  Con::amount_smoke = 90;
  Con::amount_temperature = 50;
  Con::ambient_temperature = 0;
  Con::temperature_parameter = 0.010;
  Con::smoke_density_parameter = 0.005;
  Con::external_force_parameter = 0.5;
  Con::num_iter = 16;
  Con::ALPHA = 0.7;

  s_size_smoke->setValue(Con::size_smoke);
  s_amount_smoke->setValue(Con::amount_smoke);
  s_amount_temperature->setValue(Con::amount_temperature);
  s_ambient_temperature->setValue(Con::ambient_temperature);
  s_temperature_parameter->setValue(Con::temperature_parameter);
  s_smoke_density_parameter->setValue(Con::smoke_density_parameter);
  s_external_force_parameter->setValue(Con::external_force_parameter);
  s_alpha->setValue(Con::ALPHA);

}

//nanogui::ref<Slider> s_size_smoke = new nanogui::Slider(window);
//nanogui::ref<Slider> s_amount_smoke = new nanogui::Slider(window);
//nanogui::ref<Slider> s_amount_temperature = new nanogui::Slider(window);
//nanogui::ref<Slider> s_ambient_temperature = new nanogui::Slider(window);
//nanogui::ref<Slider> s_temperature_parameter = new nanogui::Slider(window);
//nanogui::ref<Slider> s_smoke_density_parameter = new nanogui::Slider(window);
//nanogui::ref<Slider> s_external_force_parameter = new nanogui::Slider(window);
//nanogui::ref<Slider> s_num_iter = new nanogui::Slider(window);
//nanogui::ref<Slider> s_alpha = new nanogui::Slider(window);

//    gui->addVariable("Size", Con::size_smoke);
//    gui->addVariable("Density(0 to 100)", Con::amount_smoke);
//    gui->addVariable("Heat(0 to 100)", Con::amount_temperature);
//    gui->addVariable("Ambient(0 to 100)", Con::ambient_temperature);
//    gui->addVariable("Heat parameter", Con::temperature_parameter);
//    gui->addVariable("Density parameter", Con::smoke_density_parameter);
//    gui->addVariable("Force parameter", Con::external_force_parameter);
//    gui->addVariable("Diffusion iterations", Con::num_iter);