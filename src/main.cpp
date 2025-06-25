#ifndef XBOX
#error Must be built with nxdk
#endif

#include <SDL.h>
#include <hal/debug.h>
#include <hal/video.h>
#include <pbkit/pbkit.h>
#include <windows.h>

// clang-format off
static const DWORD kPassthroughVsh[] = {
#include "passthrough.vshinc"
};
// clang-format on

#define MASK(mask, val) (((val) << (__builtin_ffs(mask) - 1)) & (mask))

enum CombinerSource {
  SRC_ZERO = 0,     // 0
  SRC_C0,           // Constant[0]
  SRC_C1,           // Constant[1]
  SRC_FOG,          // Fog coordinate
  SRC_DIFFUSE,      // Vertex diffuse
  SRC_SPECULAR,     // Vertex specular
  SRC_6,            // ?
  SRC_7,            // ?
  SRC_TEX0,         // Texcoord0
  SRC_TEX1,         // Texcoord1
  SRC_TEX2,         // Texcoord2
  SRC_TEX3,         // Texcoord3
  SRC_R0,           // R0 from the vertex shader
  SRC_R1,           // R1 from the vertex shader
  SRC_SPEC_R0_SUM,  // Specular + R0
  SRC_EF_PROD,      // Combiner param E * F
};

enum CombinerDest {
  DST_DISCARD = 0,  // Discard the calculation
  DST_C0,           // Constant[0]
  DST_C1,           // Constant[1]
  DST_FOG,          // Fog coordinate
  DST_DIFFUSE,      // Vertex diffuse
  DST_SPECULAR,     // Vertex specular
  DST_6,            // ?
  DST_7,            // ?
  DST_TEX0,         // Texcoord0
  DST_TEX1,         // Texcoord1
  DST_TEX2,         // Texcoord2
  DST_TEX3,         // Texcoord3
  DST_R0,           // R0 from the vertex shader
  DST_R1,           // R1 from the vertex shader
  DST_SPEC_R0_SUM,  // Specular + R0
  DST_EF_PROD,      // Combiner param E * F
};

static const int kFramebufferWidth = 640;
static const int kFramebufferHeight = 480;
static const int kBitsPerPixel = 32;

static void LoadVertexShader() {
  auto p = pb_begin();
  p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_START, 0);

  p = pb_push1(
      p, NV097_SET_TRANSFORM_EXECUTION_MODE,
      MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_MODE_PROGRAM) |
          MASK(NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE, NV097_SET_TRANSFORM_EXECUTION_MODE_RANGE_MODE_PRIV));

  p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, 0);

  p = pb_push1(p, NV097_SET_TRANSFORM_PROGRAM_LOAD, 0);
  auto shader_program = kPassthroughVsh;
  for (auto i = 0; i < sizeof(kPassthroughVsh) / 16; ++i) {
    p = pb_push4v(p, NV097_SET_TRANSFORM_PROGRAM, shader_program);
    shader_program += 4;
  }
  pb_end(p);
}

static void SetupPixelShader() {
  // See https://github.com/abaire/nxdk_pgraph_tests/wiki/nv2a-pixel-shaders-(combiner-stages) for notes on what these
  // values mean.
  auto p = pb_begin();

  auto channel = [](CombinerSource src, bool alpha, bool invert) { return src + (alpha << 4) + (invert << 5); };

  uint32_t value = (channel(SRC_ZERO, false, false) << 24) + (channel(SRC_ZERO, false, false) << 16) +
                   (channel(SRC_ZERO, false, false) << 8) + channel(SRC_DIFFUSE, false, false);
  p = pb_push1(p, NV097_SET_COMBINER_SPECULAR_FOG_CW0, value);

  value = (channel(SRC_ZERO, false, false) << 24) + (channel(SRC_ZERO, false, false) << 16) +
          (channel(SRC_ZERO, true, true) << 8);
  p = pb_push1(p, NV097_SET_COMBINER_SPECULAR_FOG_CW1, value);

  pb_end(p);
}

/* Main program function */
int main() {
  debugPrint("Set video mode");
  if (!XVideoSetMode(kFramebufferWidth, kFramebufferHeight, kBitsPerPixel, REFRESH_DEFAULT)) {
    debugPrint("Failed to set video mode\n");
    Sleep(2000);
    return 1;
  }

  int status = pb_init();
  if (status) {
    debugPrint("pb_init Error %d\n", status);
    Sleep(2000);
    return 1;
  }

  debugPrint("Initializing...");
  pb_show_debug_screen();

  if (SDL_Init(SDL_INIT_GAMECONTROLLER)) {
    debugPrint("Failed to initialize SDL_GAMECONTROLLER.");
    debugPrint("%s", SDL_GetError());
    pb_show_debug_screen();
    Sleep(2000);
    return 1;
  }

  pb_show_front_screen();
  debugClearScreen();

  LoadVertexShader();
  SetupPixelShader();

  static constexpr float kQuadSize = 250.f;
  static constexpr float kQuadLeft = (kFramebufferWidth - kQuadSize) * 0.5f;
  static constexpr float kQuadRight =kQuadLeft + kQuadSize;
  static constexpr float kQuadTop = (kFramebufferHeight - kQuadSize) * 0.5f;
  static constexpr float kQuadBottom = kQuadTop + kQuadSize;
  static constexpr float kQuadZ = 1.f;
  static constexpr float kQuadW = 1.f;

  bool running = true;
  while (running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_CONTROLLERDEVICEADDED: {
          SDL_GameController *controller = SDL_GameControllerOpen(event.cdevice.which);
          if (!controller) {
            debugPrint("Failed to handle controller add event.");
            debugPrint("%s", SDL_GetError());
            running = false;
          }
        } break;

        case SDL_CONTROLLERDEVICEREMOVED: {
          SDL_GameController *controller = SDL_GameControllerFromInstanceID(event.cdevice.which);
          SDL_GameControllerClose(controller);
        } break;

        case SDL_CONTROLLERBUTTONUP:
          running = false;
          break;

        default:
          break;
      }
    }

    pb_wait_for_vbl();
    pb_reset();
    pb_target_back_buffer();

    /* Clear depth & stencil buffers */
    pb_erase_depth_stencil_buffer(0, 0, kFramebufferWidth, kFramebufferHeight);
    pb_fill(0, 0, kFramebufferWidth, kFramebufferHeight, 0x00000000);
    pb_erase_text_screen();

    while (pb_busy()) {
      /* Wait for completion... */
    }

    {
      auto p = pb_begin();
      p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_QUADS);
      p = pb_push1(p, NV097_SET_DIFFUSE_COLOR4I, 0xFFFF0000);
      p = pb_push4f(p, NV097_SET_VERTEX4F, kQuadLeft, kQuadTop, kQuadZ, kQuadW);

      p = pb_push1(p, NV097_SET_DIFFUSE_COLOR4I, 0xFF00FF00);
      p = pb_push4f(p, NV097_SET_VERTEX4F, kQuadRight, kQuadTop, kQuadZ, kQuadW);

      p = pb_push1(p, NV097_SET_DIFFUSE_COLOR4I, 0xFF0000FF);
      p = pb_push4f(p, NV097_SET_VERTEX4F, kQuadRight, kQuadBottom, kQuadZ, kQuadW);

      p = pb_push1(p, NV097_SET_DIFFUSE_COLOR4I, 0xFF7F7F7F);
      p = pb_push4f(p, NV097_SET_VERTEX4F, kQuadLeft, kQuadBottom, kQuadZ, kQuadW);

      p = pb_push1(p, NV097_SET_BEGIN_END, NV097_SET_BEGIN_END_OP_END);
      pb_end(p);
    }

    pb_print("Press any button to exit");

    pb_draw_text_screen();

    while (pb_busy()) {
    }
    while (pb_finished()) {
    }
  }

  pb_kill();
  return 0;
}
