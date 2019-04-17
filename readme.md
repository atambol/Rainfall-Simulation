## Rainfall-Simulation
Simulation of rainfall using Cuda and Pthreads (assumes 4 cores are available)

#### Description
There are only three forces at play:
*Gravitational pull towards the earth
*Surface tension of water droplets holding it together
*Shear due to air in the atmosphere which pulls the droplet apart
All droplet split into two exact halves if the threshold is exceeded
Droplets split or merge horizontally
Rainfall is restricted to a two dimensional plane

#### Simulation
```
make
./rainmaker 100 0 30 30 0
```
