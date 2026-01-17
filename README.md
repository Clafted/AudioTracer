# 🔊 AudioTracer: 2D Audio Ray-Tracer | Made in Raylib

https://github.com/user-attachments/assets/9fb9c13f-1591-4f20-9cc0-5fc506fc54f6

## Table of Contents
- [Use-Case](#use-case-)
- [Inspiration](#inspiration-)
- [Controls](#controls-)
- [Development Progress](#development-progress-%EF%B8%8F)
  - [Multithreading](#multithreading)
  - [Ray-tracing Optimizations](#ray-tracing-optimizations)
  - [Material-based Rendering](#material-based-rendering)
  - [2D Sound](#2d-sound)

## Use-Case 🌍
As AudioTracer simulates sound propagation, it is fit for the analysis of **room acoustics**. Software such as Odeon A/S' [*ODEON Room Acoustics Software*](https://odeon.dk/) and Virtual Acoustic's [*RAVEN*](https://www.virtualacoustics.org/RAVEN/) leverage simulation to measure the aural qualities of building interiors.


## Inspiration 💭
I have always thought of implementing ray-tracing, but never got to it until now. I feel like the visual aspect of Computer Generated things has already been thoroughly developed over the years, and it would be nice for a change of perspective. Instead of implementing *light* ray-tracing, then, I chose to implement *sound* ray-tracing instead, to explore new grounds, and eventually apply a little bit of knowledge from our *light* ray-tracer friends.


## Controls 🎮
(Best tested with headphones 🎧)
- **Move Cursor**: Moves player (the green one)
- **SPACE/B Keys**: Play sound
- **UP/DOWN Keys**: In/Decrease sample-size (# rays)
- **RIGHT/LEFT Keys**: In/Decrease max number of bounces
- **Q/E Keys**: In/Decrease resolution

 
## Development Progress ☑️
### Multithreading
Real-time ray-tracing is infeasible within a single-threaded application. Thus there becomes a necessity for more processing power.
#### Solutions
<u>Mutlithreading/Concurrency</u> is the way to go. Confining the sampling algorithm to a single thread is incredibly limiting and fails to harness many available resources. Since the ray-traced samples are independent of one another, these samples can be delegated to multiple threads with the assurance of zero-conflicts between thread calculations.
#### Progress
- Functional multithreaded sampling AND rendering is currently supported. There is flickering/inconsistencies in ray counts that will be addressed

### Ray-Tracing Optimizations
It seems the ray-tracing algorithm needs to be improved. Although current performance seems fast, there is likely great potential for
further improvement in the algorithm. Thus I'm seeking optimizations to the basic algorithm.
#### Solutions
<u>Bounding Volume Hierarchies (BVH)</u> Bounding volumer hierarchies are a common way for minimizing collision detections.
It is an *acceleration structure* that separates (bounds) geometry into groups (volumes), which are ordered (hierarchies).
The resulting structure is a tree of large bounding volumes, each containing smaller bounding volumes, each containing... etc.
At the most basic level are the geometry, where rays would finally be tested for collision. The benefit of BVH is that rays can
first test collision with the largest bounding volumes, and--if there is no collision--conclude that none of the geometry contained
in said bounding volume will collide with it. Thus, numerous ray-geometry collisions are skipped, saving time and resources.
#### Progress
- WIP

### Material-based rendering
Having a working prototype, I hope to further improve the performance, possibly by some 
code refactoring or rethinking some algorithms. A bit more ambitious of a goal, however, 
is to support the use of different *materials*; akin to light ray-tracing--in which different materials absorb varying levels of light and result in different colours--I want to support different materials cause the sound to be absorbed and result in various effects.
#### Solutions
<u>Supporting materials</u>: In addition to positional data, LineObjects (the white lines) include
material properties which influence how much sound reflects, refracts, or is absorbed.
- <u>16/10/2025</u> Currently all lines have the same material, as the engine only stores positional data,
not yet material data
#### Progress
- Yet to begin (YTB) as of Jan 17, 2026

### 2D Sound
Currently, AudioTracer renders positional sound on a single axis ("1D audio"), left-to-right.
This leaves potential for the addition of front-to-back positional audio, i.e.
hearing things *in front of* or *behind* you. The Raylib library (on which this project
is built) abstracts the Miniaudio library which it uses for audio support. This
causes an issue in accessing the 3D audio features from Miniaudio.
#### Solutions
<u>Panning techniques</u>: Raylib has a 3D-positional audio program example in [the audio examples page](https://www.raylib.com/examples.html),
which utilizes a recipe of volume and panning configurations to simulate 3D audio.
Thus I hope to replicate this technique to acheive 2D audio.
#### Useful resources
- [How 3D sound is simulated with 2 speakers](https://catalyst-magazine.org/articles/the-science-behind-3d-sound/)
#### Progress
- YTB Jan 17, 2026