## Minesweeper
System Programming Team Project - Team 12<br>
> 2022115008	이정민<br>
> 2022116110	정민규<br>
> 2018110115	최지원<br>

## What is the Minesweeper and differences?
> Minesweeper is a logic puzzle video game where players clear a grid of squares without detonating hidden mines.
> The game provides clues about the number of neighboring mines in each field. - **wikipedia**
> Our Minesweeper game is aimed at two players competing.
> The first player who find all the mines can add their name to the scoreboard.

## Rule
1. Connect to server
2. Determine the number of rows, cols and mines.
3. After the two players have finished setting the mineboards, the game begins.
4. Enter the desired coordinates in x y format, o for open, and f for flag.<br>
   ex) 1 1 o = (1, 1) open  |  ex) 1 1 f = (1, 1) flag
5. The first player who clear all the mines is the winner.
6. Winner's name is added to the scoreboard and the game is over.

## How to play
1. Clone the repository
```
git clone http://github.com/KinsHishI/System-Programming---Team-Project
```

2. Execute ```Makefile```
```
./Makefile
```

3. For Server & Client
3-1. Execute the ```mserver```, Usage : ./mserver <Port>
```
./mserver 9190
```
  
3-2. Execute the ```mclinet```, Usage : ./mclinet <IP> <Port> <Name>
```
./mclient 127.0.0.1 9190 Dummy
```
  
## Demo Video
[Video Presentation](https://youtu.be/Dk2PLwLLhpo)
