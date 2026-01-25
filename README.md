# Mango
Vulkan based Game Engine in C++

## Project Structure
![Structure](MangoProject.png)

* **MangoEngine (`DLL` / `.so`)**: The core shared library containing the renderer, ECS, and low-level systems.
* **Hot-Reloadable Game Code (`DLL` / `.so`)**: A lightweight module for gameplay logic. It can be recompiled and reloaded while the app is running for instant feedback.
* **Game App (`EXE`)**: The shipping client that hosts the engine and manages the lifecycle of the game logic.
* **Dev Test (`EXE`)**: An isolated sandbox for stress-testing engine features in isolation.
* **Editor (`EXE`)**: A separate application for world-building, kept out of the final shipping build.

## Features
* Lightweight build system - no cmake, avoiding complexities
* Low-level utilities - binary trees, helpers, etc
* Platform Layer (Windows and MacOS for now)
* Logger 
* File I/O
* Application Layer
* Renderer/API Abstraction Layer (Sits over Vulkan)
* allocators - ecs, mimalloc, etc
* scripting - hot reloading
* Physics 

## Engine Architecture
![Arch](EngineArch.png)