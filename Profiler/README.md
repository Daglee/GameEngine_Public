# Profiler

## Description
Displays the following as text on-screen:

* Frames rendered per second.
* Memory usage.
* Time taken to update each subsystem (ms).



## How To Use
Press 'p' to display the profiler. It will not be displayed when the game is paused.



### Frame Rate Counter
Only updated on every *n* frames after an given elapsed period of time to prevent unreadable counter that changes too fast.


### Memory Usage
* Displays total bytes used by the database which contains all resources.
* Displays percentage of allowed memory that is currently being used.


### Subsystem Timers
Each subsystem contains a timer that is is updated every frame.

The time taken to fully update the subsystem is displayed in ms.

Timers must be manually added to the Profiler.