# Game

## Description
Contains gameplay focused classes and integration of the different subsystems.



## File I/O

### Creating Levels
"../Data/Levels/Directories" contains files that specify where each levels assets are located. 
This is used in the level list file. The level list file lists which levels to load and thus points towards the assets required for each level.

#### Assets
*See ../Data/Levels/Arena1/assets.txt*

The assets file specifies what assets to load and of what quanitity. 

This follows the same syntax as the start up file.

#### Game Logic
*See ../Data/Levels/Arena1/GameLogic.txt*

A finite state machine can be allocated a configuration file to be built off.

#### Game Entities
*See ../Data/Levels/Arena1/GameObjects or PhysicsObjects.txt*

Game or physics objects that were loaded by the assets file can be configured using this file.
The first line must provide the asset name.
The following lines must start with the name of the attribute to configure, followed by the values to set it to.
The last line must use the delimiter "-".

For example:

`ball4`

`mass 300`

`-`

Entities can be positioned and attached to a mesh in the positions file.
*See ../Data/Levels/Arena1/positions.txt*


### Changing levels
The LevelManager class uses a finite state machine to manage the transition of levels. 
It must be updated every frame to check whether the level should be changed.

Upon exiting a level, all of its assets are unloaded from the database.



## Audio
AudioManager singleton allows sounds to be played including:

* Background music
* Playing stored music or sounds in loops
* Playing a temporary sound object once.

Requires a scene node as an audio listener. For the example game provided this is the camera.

```cpp
	AudioManager::GetInstance()->SetListener(camera->GetSceneNode());
```



## Game Entities

### Game Objects
Game objects consist of meshes. They are static unless their position or size is modified. 
They must be added to the renderer.

### Physics Objects
Physics objects use rigid bodies to allow for collision responses. 
They must be added to both the renderer and physics engine.



## Players

Each player is transformed based on inputs provided from the user input subsystem.
Simple placeholder animations, character models, and weapons are being used.

Player mechanics are managed both through code and through a finite state machine.
