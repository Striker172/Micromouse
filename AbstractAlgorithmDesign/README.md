# Important Notes and requirements
## Do *NOT* edit the API.cpp or API.h files
### Main.cpp will be the run file for the algorithm for use solely with the simulator. It will later be implemented using the hardware API for use on the actual mouse.
### Do *not* push your a.exe to remote.
### Changed the build config on the simulator to (g++ Main.cpp API.cpp MouseMovementLibrary.cpp FloodfillLibrary.cpp) to have the libraries actually work.

*Exploration Mode:*
The mouse will not know the layout of the walls ahead of time. It will begin in exploration mode, updating it's own knowledge of the maze with every new cell it explores.
The mouse will move to the lowest scored adjacent cell which is not blocked by a wall.
Cells will be stored as structs in a 2d array for the maze layout.
Cells will be marked as explored or unexplored. Every cell will be assumed to have no walls until it is explored.
Cells will store their wall configurations, as well as their shortest path distance to the goal.
When the goal is reached(or the start is returned to), the mouse will evaluate (but not run) the most optimal path given current information. If the most optimal path has been entirely explored, the mouse will switch from exploration mode to speed mode.

*Speed Mode:*
In speed mode, the mouse will have sufficiently solved the maze, and can determine the proper path to take ahead of time.
The mouse may need to first move back to the start area.
The mouse will only make movements after it begins and will not perform any other unnecessary computations.
