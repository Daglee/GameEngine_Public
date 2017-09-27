# Game

## Description
Finite state machine creation system that can be used to manage gameplay mechanics which are updated every frame.



## File I/O
The engine is wholly data driven, based off files in the "Data" folder. 

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



## Creating a Class That Uses a Finite State Machine

### Finite State Machine
Inheriting from FSMUnit will create a finite state machine with a given name and provide an unordered map for properties to be accessed by the finite state machine.
All properties must be floats or use hashing.

These will be built based on their names with delayed construction.

#### Late Builds
Levels will build the finite state machines once their individual files have been read. 

For example:
```cpp
	while (getline(file, line))
	{
		vector<string> tokenisedLine = Log::tokenise(line);
		const string FSMName = tokenisedLine.at(0);
		const string FSMConfigurationFile = tokenisedLine.at(1);

		finiteStateMachineManager->LateBuildFSM(FSMName, FSMConfigurationFile);
	}
```



## Message System
Uses hashing, converts strings to floats.

### Messages
Messages last a single frame and are cleared at the end of the game loop, after subsystems have been updated.

### Events
Events will stay until manually stopped. 


