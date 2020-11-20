<img src="https://github.com/zbigos/PGK2020/blob/master/lista_4/screen.png" width="500" height="500">

## Compiling
Just run `make`. If you need to recompile after some changes you'll need to `make clean` first.  
Due to the fact that I cannot use c++, fiels in `shapes/` are simply jammed into `zadanie4.cpp` by preprocessor.  

## Gameplay.
At every point of the game you can press esc key to release mouse pointer from the game. If you wish to bind it again use tab.  
If you wish to leave the game press escape while your cursor is not bound to the window.  

To start the game just press enter and wait untill the animation plays.  

Movement is done by w/s/a/d and mouse pointer. Your goal is to reach the ball at the end of the labirynth. Each time you get it, a new one will spawn at the other corner of the map. Collect it 8 times to win the game.  
Each keypress (enter/tab/esc) is processed with 1 second delay. So don't worry if spamming the key does not do what you want.  

## Debugging
Keys o/p advance/roll back game by one stage. 

## Changing the game parameters.
If (for some reason) you wish to make the game any longer you can change  
 - `labsize = 8;` in line 259 of zadanie4.cpp  
 - `maxlabstage = 10;` in line 146 of zadanie4.cpp  

These affect the size of labirynth and how many times you need to run through the labirynth to win the game.  
Currently the game does not have any way of making you loose, so it's better to keep these small.  

## Troubleshooting
Currently in zadanie4.cpp there is a snippet
```
if (type == GL_DEBUG_TYPE_ERROR) {
	int *a = NULL; // halt gdb. or entire system if kernel is not fast enough;
	int payload = 2137;
	*a = payload;
}
```

if for any reason you get this to execute (and this code will force your machine to SIGSEGV this process) please send me result of gdb bt on this code. This will help greatly with debugging any issues.  
