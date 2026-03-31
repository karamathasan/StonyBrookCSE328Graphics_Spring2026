# HW2

Your Name Karamat Hasan
Your SBU ID 116425951

Your Email karamat.hasan@stonybrook.edu

## Overview

- Implemented a sample modern OpenGL program with GLFW as the windowing toolkit. 
- Implemented some basic geometries, including lines, triangles, and circles. Circles are implemented with tessellation shaders. 

## Notes

- All README files for future homework should also comply with the same format as this one. 
- This program template is just for your reference. Please feel free to code your own program (i.e., not using this template). However, the user interface (mouse and keyboard functionalities) should be the same as specified in the homework manual. 
- Please **comply with the submission requirements as detailed on the [TA Help Page](https://www3.cs.stonybrook.edu/~xihan1/courses/cse328/ta_help_page.html)**. Plesase rename the directory as instructed by the TA Help Page, and submit via Brightspace. 
- Please also make sure you have checked all implemented features with "x"s in the Markdown table below. As speficied on the TA Help Page, only checked features will be considered for grading!

## Hints on The Template

- Suggested order to read and understand this program: 
  - GLFW callbacks;
  - Triangle (ignore the code related to the self-spin effect);
  - Triangle (with self-spin; involves transformation matrices);
  - Circles (involves tessellation shaders, which are not necessary in the first half of this course). 
- In this program, the circle parameters are passed into tessellation shaders via generic vertex attribute arrays. 
  Note how this differs from the "pass-by-shader-uniforms" method for the sphere example; 
- Please do remember to play with the program as guided by the comments in the tessellation evaluation shader;
- If this program does not work on your VMWare virtual environment, 
  please try to [disable the 3D acceleration feature](https://kb.vmware.com/s/article/59146). 

## Dependencies

- OpenGL:
```bash
sudo add-apt-repository ppa:kisak/kisak-mesa
sudo apt update
sudo apt-get dist-upgrade
sudo reboot
```
- [GLAD](https://glad.dav1d.de/)
  - Configuration w.r.t. results of `sudo glxinfo | grep "OpenGL`
  - Command `glxinfo` needs `mesa-utils`
- Remaining dependencies could be installed via apt:
```bash
apt install libopencv-dev libglm-dev libglew-dev libglfw3-dev mesa-utils libx11-dev libxi-dev libxrandr-dev
```

## Compile & Run

- Run inside this directory, i.e., `hw1/`: 
```bash
mkdir build
cd build
cmake -DMAKE_BUILD_TYPE=Release ..
make 
cd ..
./build/hw2
```

## Features Implemented

Check all features implemented with "x" in "[ ]"s. 
Features or parts left unchecked here won't be graded! 

- [x] 1. Bouncing Ball
  - [x] Creation
  - [x] Dynamically reading config file
  - [x] Movement
  - [x] Collison detection
- [x] 2. 4+ Bouncing Balls
- [x] 3. Bouncing Face
  - [x] Creation
  - [x] Dynamically reading config file
  - [x] Movement
  - [x] Collison detection
  - [x] Generation Evolution
- [x] 4. More Bouncing Faces
  - [x] 8+ bouncing faces
  - [x] 16+ bonucing faces
- [x] 5. BONUS
  - [x] Please specify

## Usage

- If you have implemented extra functionalities not mentioned in the manual,
  you may specify them here.
- If your program failed to obey the required mouse/keyboard gestures,
  you may also specify your own setting here.
  In this case, penalties may apply.

## Appendix

To enter the Celestial Body simulation, press the 4 key. The Celestial Body simulation calculates the forces applied to each of the celestial bodies in the scene based on the distance to each body.

etc/celest_config.txt is a file containing initialization parameters for a scene of a celestial body simulation. Each line is given the parameters mass, radius, x-position, y-position, x-velocity, y-velocity (radius, position, and velocity are all in NDC).  

configurations to try:
```
2,0.05,0.4,0,0,0.25
2,0.05,-0.4,0,0,-0.25

```
100,0.5,0,0,0,0
1,0.025,-0.8,0,0,-2.35
```
