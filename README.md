# Game Engine

## Description
A c++ data driven game engine consisting of various subsystems including graphics, physics and multi-threading.


## How To Use
*See Game/run.cpp*

#### Singletons
The audio manager and message system singletons can be initialised using the namespace.

```cpp
	AudioManager::Initialise();
	MessageSystem::Initialise();
```

#### Database
A new instance of database must be created to house all assets and manage memory.
Tables are specified in TableCreation.

The following creats a database and initialises the added tables.

```cpp
	DataBase* database = new DataBase();

	TableCreation tables(database);
	tables.AddTablesToDatabase();

	database->ReserveMemoryForAllTables();
```

#### Launcher
The launcher starts up the game, setting up everything that is required.

```cpp
	Launcher* game = new Launcher("RendererResourceName", "WindowResourceName", databasePointer);
	game->Launch("../Data/startup.txt");
	
	game->InitProfilerTimers();
```

#### Levels
The list of levels for the game are sent to a level manager which handles the loading of levels.

```cpp
	LevelManager* levels = new LevelManager(database, "../Data/Levels/LevelList.txt");
	levels->LoadFirstLevel();
```

#### Subsystems
The SubsystemManager class retrieves subsystems from the database and handles the updating of them.
This will happen in the constructor, and therefore must happen after the database has been filled.

```cpp
	SubsystemManager subsystems(database);
```

#### Game Loop
The window class contains a game timer which can be used to send delta time to all subsystems.
The running flag will allow for the exit of the game loop. This is handled by the game logic.

```cpp
	while (window->UpdateWindow() && window->running)
	{
		float deltatime = window->GetTimer()->GetTimedMS();

		levels->Update(deltatime);
		subsystems.Update(deltatime);
	}
```



## How To Play
* Up to 4 players supported
* Player 1 will always use mouse and keyboard
* Xbox One controllers are supported as well the official wireless adapter.

### Controls
#### Mouse and Keyboard
* WASD = move
* Mouse = aim
* Left click = shoot.
* R = reload

#### Xbox One Controller
* Left stick = move
* Right stick = aim
* Right trigger = shoot
* Left trigger = reload

#### Utilities
* P = profiler *Profiler will not be displayed when paused*
* Escape = pause
* L = exit




## File I/O
The engine is wholly data driven, based off files in the "Data" folder. 


### Start Up
*See ../Data/startup.txt*

One file must handle the initialisation of the subsystems of the engine. They will not be initialised in the order they are read as this is multi-threaded and therefore non-deterministic.

#### Creating a resource
A subsystem must be specified with a number to create, the name of the database table to insert into, and the name of this resource.
For example:

`1 PhysicsEngineTable physicsEngineInstance`

If more than 1 of any resource is created, the number is appended onto the end of the resource name.

#### Specifying Parameters.
The keyword "params" ensures that all tokens after the name of the resource are parsed into parameters to be used in the creation of the resource.

For example, the Window class requires a resolution and a flag to set whether to run the game in fullscreen.

`params WindowTable WindowInstance 1920 1080 1`



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



### Game Logic Finite State Machine
Classes can inherit from FSMUnit to create a finite state machine. Properties can be added to the inherited unordered map, allowing the finite state machine to use them.

Finite state machines are also data driven. *See ../Data/GameLogic/player1.txt*
All values are floats. Strings can be hashed into floats by appending "s:" to the start.

For example:
`playerID = s:ThisIsAnID`


These files are split into two sections.

#### States
The first line must specify the number of states that are created here.

States consist of actions to carry out. The left hand side must be a property of the finite state machine (which is available in the unordered map), 
followed by = *some value*.

For example:
`state1`

`playerHealth = 100`

`--`

Other actions available:
* **-=**
* **+=**
* ***=**
* **transmit**
* **newevent**
* **endevent**
* **addpoints**

#### Transitions
Transitions consist of 3 lines.

1. Transition name
2. Start state and end state.
3. The condition, which if true, causes the state transition specified above.

For example:

`Transition1`

`0 -> 1`

`playerHealth == 0 && playerID == 1`

Other operators available:
* **!=**
* **>**
* **<**
* **exists_transmission**
* **!exists_transmission**
