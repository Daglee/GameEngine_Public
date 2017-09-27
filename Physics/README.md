# Physics Engine

## Description
Physics engine that updates rigid bodies that have plane or sphere colliders with an impulse and projection response.
Semi-implicit Euler is used.



## File I/O

#### Game Entities
*See ../Data/Levels/Arena1/GameObjects or PhysicsObjects.txt*

Physics objects that were loaded by the assets file can be configured using this file.
The first line must provide the asset name.
The following lines must start with the name of the attribute to configure, followed by the values to set it to.
The last line must use the delimiter "-".

For example:

`ball4`

`mass 300`

`-`

Entities can be positioned and attached to a mesh in the positions file.
*See ../Data/Levels/Arena1/positions.txt*