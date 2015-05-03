# CADP2015

## Building:
`make`

## Running:
`./crossing <configuration> [K] [MAX_SPAWNS]`
`<configuration>` is one of `simple`, `ext[ended]` or `busy`
`K` and `MAX_SPAWNS` are optional. If K > MAX_SPAWNS, the value is swapped.
The arguments can be positioned anywhere (`./crossing 10 simple 1000` is equivalent to `./crossing simple 1000 10`).

### Visualization
Running the program will produce an ASCII art visualization:
 ```
 _____________________________________________________________________________
|                              |    |    |                                   |
|                              |         |                                   |
|                              |    |    |                                   |
|                             _|_________|_                                  |
|                              |    |    |                                   |
|                              |    |    |                                   |
|                             _|_________|_                                  |
|                              |         | p:    0                           |
|______________________________|       / |___________________________________|
|                 |V    0|           /    /    |      |                      |
| __  __  __  __  |P  __0|__  __   /    / __  _|  __  |_  __  __  __  __  __ |
|           v:   0|      |       /P    0       |      |                      |
|_________________|______|_____/    /     _____|______|______________________|
|                 p:    0      |  /      |                                   |
|                             _|_________|_                                  |
|                              |V       0| p:    0                           |
|                              |P   |   0|                                   |
|                             _|_________|_                                  |
|                              |v:      0|                                   |
|                              |         |                                   |
|                              |    |    |                                   |
|____________________________________________________________________________|
```

Capital letters represent agents that are currently crossing; lower-case letters represent agents that are waiting to cross. `P` stands for pedestrians and `V` stands for vehicles. The number to the right represents the number of agents in the location/state. The position of the letters in the ASCII art represents the location/state of those agents.

## The log
The program saves logging output of the simulation to `log.txt`. ***If the file exists, it will be overwritten.***
The log will indicate at the top how many agents will be spawned in the simulation and may optionally summarize at the bottom what kinds of agents were spawned. The log contains events for agents crossing (`+CROSS`), exiting (`-CROSS`), waiting (`+wait`) and no longer waiting (`-wait`). In the non-simple configurations, the log will also print out the state of the structs and semaphores. The format of the state is like so:

`[P, V]vertical crossing, [P, V]horizontal crossing, [p, v]vertical waitig, [p, v]horizontal waiting, {[P, p]diagonal}, k: [0, 1], Semaphores:[light, vP, vV, hP, hV, d]`
