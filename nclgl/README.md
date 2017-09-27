# NCLGL OpenGL Renderer

## Description
* Based on Newcastle University MSc and BSc Games Engineering OpenGL framework.
* Uses OBJ Meshes and custom meshes for game objects.


## How To Use
Renderer class iterates through vector of meshes that have been added and renders them.

* Screen overlays can be added and managed with a finite state machine.
* Text can be pushed onto a buffer to be rendered on top of the scene. This is cleared every frame.

### Loading Screen
Loading screen takes a max value and the current progress towards that value. A red bar will extend across the screen to indicate progress.