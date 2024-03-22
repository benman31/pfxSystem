# About the project
A simple pfx system written in C++ and OpenGL using a mini-engine / 3D framework called **wolf** by UPEI instructor and former Technical Director at Other Ocean, Gordon Wood

This project was created as part of a university graphics programming assignment, where the focus was on using an efficient data structure to render a large volume of particles in as few draw calls as possible.

A simple XML-based text interface was also created to enable the creation and design of new particle effects, exemplified [here](https://github.com/benman31/pfxSystem/blob/main/data/smokey-fire.effect)
and [here](https://github.com/benman31/pfxSystem/blob/main/data/smoke.emitter)

I would like to return to this at some point to clean up some of the unused files, build a UI Editor that writes to the XML files for ease of use, and also start to replace the original wolf dependancies with my own code.

# Install Instructions DISCLAIMER: MILEAGE MAY VARY, INSTRUCTIONS BASED ON STARTER CODE BY GORDON WOOD 
## Windows
### Setting up Dev Environment
If dev environment not set up yet follow these steps:
1) Install Visual Studio Community Edition 
2) Install Visual Studio Code
3) Install CMake
4) Ensure CMake is on your PATH
5) Install C/C++ VS Code Extension (0.28.3 or above)
6) Install CMake Tools VS Code extension (version 1.4.1 or above)
7) Install CMake VS Code extension (0.0.17 or above)
8) Install TortoiseGit (or another Git client, but I'll assume tortoise git)
### Building samples
1) Clone this repository.
~~2) Go into the folder in Explorer, right click and go to TortoiseGit->Submodule Update~~ (Testiing needed, this step may not be necessary)
3) Open the folder in VS Code
4) Ctrl+Shift+P then type/choose CMake: Configure
5) Choose one of the available options (e.g. Visual Studio Community 2017 Release - amd64)
5) Ctrl+Shift+P then type/choose CMake: Build
6) Hit F5 to run in debugger if all compiled well
7) You may need to choose "sample" from the dropdown the first time
## Linux
### Setting up Dev Environment
If dev environment not set up yet, follow these steps:
1) Install gcc or clang
2) Install Visual Studio Code
3) Install CMake
4) Ensure CMake is on your PATH
5) Install C/C++ VS Code Extension (0.28.3 or above)
6) Install CMake Tools VS Code extension (version 1.4.1 or above)
7) Install CMake VS Code extension (0.0.17 or above)
8) Install git
### Building samples
1) Clone this repository.
2) cd into the folder and run:
```git submodule update --init```
3) Open the folder in VS Code
4) Ctrl+Shift+P then type/choose CMake: Configure
5) Choose one of the available options (e.g. gcc)
5) Ctrl+Shift+P then type/choose CMake: Build
6) Hit F5 to run in debugger if all compiled well
7) You may need to choose "sample" from the dropdown the first time
