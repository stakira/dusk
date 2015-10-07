
// dusk
#include "dusk.h"
#include "dusk/utils/clock.h"
#include "dusk/window/window_native.h"
#include "dusk/graphics/graphics_native.h"

// std
#include <stdlib.h>
#include <stdio.h>

// love
#include "common/Module.h"
#include "modules/window/sdl/Window.h"
#include "modules/event/Event.h"
#include "modules/event/sdl/Event.h"
#include "modules/graphics/Color.h"
#include "modules/graphics/Graphics.h"
#include "modules/graphics/opengl/Graphics.h"
#include "modules/graphics/opengl/ShaderLanguage.h"
#include "modules/mouse/Mouse.h"
#include "modules/mouse/sdl/Mouse.h"
#include "modules/font/Font.h"
#include "modules/font/freetype/Font.h"

namespace dusk {

DuskDelegate::DuskDelegate() {}

void DuskDelegate::Initialize() {
  vmhost_ = new dusk::vm::Host();

  dusk::window::Register();
  dusk::graphics::Register();
  
  if (!vmhost_->Initialize()) {
    printf("Cannot initialize VM");
    exit(1);
  }

  // window
  love::window::Window *window = new love::window::sdl::Window();
  love::Module::registerInstance(window);

  // window settings
  love::window::WindowSettings *settings = new love::window::WindowSettings();
  
  settings->resizable = true;
  settings->centered = true;
  settings->vsync = false;
  settings->minwidth = 800;
  settings->minheight = 600;
  settings->msaa = 4;

  window->setWindow(
    800,
    600,
    settings);

  // event
  love::Module::registerInstance(new love::event::sdl::Event());

  // graphics
  love::graphics::opengl::setDefaultShaderCode();
  love::Module::registerInstance(new love::graphics::opengl::Graphics());

  // mouse
  love::Module::registerInstance(new love::mouse::sdl::Mouse());

  // font
  love::Module::registerInstance(new love::font::freetype::Font());
}

void DuskDelegate::Run() {

  love::event::Event *event =
    love::Module::getInstance<love::event::Event>(
      love::Module::ModuleType::M_EVENT);

  love::window::Window *window =
    love::Module::getInstance<love::window::Window>(
      love::Module::ModuleType::M_WINDOW);

  love::graphics::opengl::Graphics *graphics =
      love::Module::getInstance<love::graphics::opengl::Graphics>(
        love::Module::ModuleType::M_GRAPHICS);

  love::mouse::Mouse *mouse =
    love::Module::getInstance<love::mouse::Mouse>(
      love::Module::ModuleType::M_MOUSE);

  love::font::Font *font =
    love::Module::getInstance<love::font::Font>(
    love::Module::ModuleType::M_FONT);

  vmhost_->Start();
  vmhost_->OnLoad();

  // refresh clock
  dusk::utils::Clock::getDt();

  love::graphics::Colorf colorf(0, 0, 0, 0);

  float circleX = 100.f;
  float circleY = 100.f;

  bool exit = false;
  while (!exit) {
    event->pump();

    love::event::Message *msg;
    while (event->poll(msg)){
      // exit
      if (msg->getName() == "quit") {
        exit = true;
      }

      // event
      vmhost_->OnEvent(msg->getName(), 1, 1, 1, 1);
    }

    // exit event could be send twice, put it out of loop
    if (exit) printf("exiting");

    // update
    vmhost_->OnUpdate(dusk::utils::Clock::getDt());

    // graphics
    if (graphics->isActive()) {
      
      graphics->clear(colorf);
      
      vmhost_->OnDraw();

      graphics->present();
    }
  }
}

void DuskDelegate::Cleanup() {
  vmhost_->Stop();
  delete vmhost_;
}

}
