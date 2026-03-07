#include <SDL.h>

#include "game/graphics/visualizer.h"
#include "game/soccer_pitch.h"
#include "utils/constants.h"

int main(int argc, char *args[]) {
  (void)argc;
  (void)args;

  SoccerPitch* pitch = new SoccerPitch();

  pitch->Initialize();
  
  pitch->SetGameOn(true);

  SoccerVisualizer graphics;
  
  if (!graphics.Init()) {
    return -1;
  }

  bool quit = false;

  while (!quit) {
    graphics.ProcessEvents(quit);

    pitch->Update();
    graphics.RenderFrame(*pitch);

    SDL_Delay(constants::FRAME_DELAY_MS);
  }

  graphics.Cleanup();
  return 0;
}
