// window.c

#include <assert.h>

#include "common.h"
#include "config.h"
#include "renderer_common.h"
#include "math_util.h"
#include "renderer.h"
#include "window.h"

i8 mouse_state = 0;
i8 key_down[GLFW_KEY_LAST] = {0};
i8 key_pressed[GLFW_KEY_LAST] = {0};

const u8* gamepad_button_states = NULL;
i32 gamepad_button_count = 0;
const float* gamepad_axes = NULL;
i32 gamepad_axis_count = 0;

u8 gamepad_button_down[MAX_GAMEPAD_BUTTONS];
u8 gamepad_button_pressed[MAX_GAMEPAD_BUTTONS];

v2 joysticks[MAX_JOYSTICK] = {0};
u32 joystick_count = 0;

float joystick_1_x = 0.0f;
float joystick_1_y = 0.0f;
float joystick_2_x = 0.0f;
float joystick_2_y = 0.0f;

i32 joystick_present = 0;

static void opengl_configure();
static void framebuffer_size_callback(GLFWwindow* window, i32 width, i32 height);

void opengl_configure() {
  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glShadeModel(GL_FLAT);
  glEnable(GL_TEXTURE_2D);
  glAlphaFunc(GL_GREATER, 1);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// NOTE(lucas): This callback function is called when resizing the window.
void framebuffer_size_callback(GLFWwindow* glfw_window, i32 width, i32 height) {
  glViewport(0, 0, width, height);
  glfwGetWindowSize(glfw_window, &width, &height);
  window.width = width;
  window.height = height;
  if (window.framebuffer_callback) {
    window.framebuffer_callback();
  }
}

i32 window_open(i32 width, i32 height, u8 fullscreen, const char* title, callback framebuffer_callback) {
  window.title = title;
  window.width = width;
  window.height = height;
  window.init_width = width;
  window.init_height = height;
  window.fullscreen = fullscreen;
  window.windowed_fullscreen = 0;
  window.mouse_x = 0;
  window.mouse_y = 0;
  window.framebuffer_callback = framebuffer_callback;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_FOCUSED, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#if defined(__APPLE__)
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  window.window = glfwCreateWindow(width, height, title, fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
  if (!window.window) {
    fprintf(stderr, "Failed to create window\n");
    return -1;
  }
  glfwMakeContextCurrent(window.window);
  glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);

  i32 glew_err = glewInit();
  if (glew_err != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW: %s\n", glewGetErrorString(glew_err));
    return -1;
  }
  glfwSwapInterval(VSYNC);
  opengl_configure();
  renderer_init();
  return 0;
}

void window_toggle_fullscreen() {
  window.windowed_fullscreen = !window.windowed_fullscreen;
  if (window.windowed_fullscreen) {
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window.width = mode->width;
    window.height = mode->height;
  }
  else {
    window.width = window.init_width;
    window.height = window.init_height;
  }
  glfwSetWindowSize(window.window, window.width, window.height);
}

void window_clear() {
  glClearColor(0, 0, 0, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window_swapbuffers() {
  glfwSwapBuffers(window.window);
}

u8 window_should_close() {
  return glfwWindowShouldClose(window.window);
}

i32 window_pollevents() {
  glfwPollEvents();
  return 0;
}

i32 window_process_input() {
  glfwGetCursorPos(window.window, &window.mouse_x, &window.mouse_y);
  for (u16 i = 0; i < GLFW_KEY_LAST; i++) {
    i32 key_state = glfwGetKey(window.window, i);
    if (key_state == GLFW_PRESS) {
      key_pressed[i] = !key_down[i];
      key_down[i] = 1;
    }
    else {
      key_down[i] = 0;
      key_pressed[i] = 0;
    }
  }

  i32 left_mouse_button_state = glfwGetMouseButton(window.window, 0);
  i32 right_mouse_button_state = glfwGetMouseButton(window.window, 1);
  i32 middle_mouse_button_state = glfwGetMouseButton(window.window, 2);

  (left_mouse_button_state && !(mouse_state & (1 << 7))) ? mouse_state |= (1 << 6) : (mouse_state &= ~(1 << 6));
  left_mouse_button_state ? mouse_state |= (1 << 7) : (mouse_state &= ~(1 << 7));
  (right_mouse_button_state && !(mouse_state & (1 << 5))) ? mouse_state |= (1 << 4) : (mouse_state &= ~(1 << 4));
  right_mouse_button_state ? mouse_state |= (1 << 5) : (mouse_state &= ~(1 << 5));

  (middle_mouse_button_state && !(mouse_state & (1 << 3))) ? mouse_state |= (1 << 2) : (mouse_state &= ~(1 << 2));
  middle_mouse_button_state ? mouse_state |= (1 << 3) : (mouse_state &= ~(1 << 3));

  if (key_pressed[GLFW_KEY_F11]) {
    window_toggle_fullscreen();
  }
  joystick_present = glfwJoystickPresent(GLFW_JOYSTICK_1);
  if (joystick_present) {
    gamepad_button_states = glfwGetJoystickButtons(0, &gamepad_button_count);
    gamepad_axes = glfwGetJoystickAxes(0, &gamepad_axis_count);
    for (u8 i = 0; i < gamepad_button_count; ++i) {
      u8 key_state = gamepad_button_states[i];
      if (key_state == GLFW_PRESS) {
        gamepad_button_pressed[i] = !gamepad_button_down[i];
        gamepad_button_down[i] = 1;
      }
      else {
        gamepad_button_down[i] = 0;
        gamepad_button_pressed[i] = 0;
      }
    }
    if (gamepad_axes) {
      joystick_count = gamepad_axis_count / 2;
      for (i32 i = 0; i < gamepad_axis_count; i += 2) {
        v2* joystick = &joysticks[i];
        joystick->x = gamepad_axes[i];
        joystick->y = gamepad_axes[i + 1];
      }
    }
  }
  return 0;
}

void window_close() {
  renderer_free();
  if (window.window) {
    glfwDestroyWindow(window.window);
    glfwTerminate();
  }
}
