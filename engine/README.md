# Custom C++ & OpenGL Graphics Engine ⚙️

## Overview
This directory contains the custom-built C++ and OpenGL graphics engine utilized by the car simulator project. Designed to be lightweight, performant, and highly decoupled from specific game logic, the engine abstracts away the complexities of the OpenGL state machine and window management, providing a clean, modern API for the client simulation.

## Comprehensive Class Architecture

Below is the complete, exhaustive list of all classes and components implemented within the engine, categorized by their systemic roles.

### 1. Foundation & Scene Graph
The core structure of the engine relies on a hierarchical, node-based scene graph.
- **`Object` (`object.h`)**: The root base class for all logical and renderable entities in the engine.
- **`Node` (`node.h`)**: Inherits from `Object`. It forms the backbone of the scene graph. Each node maintains a local transformation matrix (`glm::mat4`) and propagates transformations (translation, rotation, scale) to its children. It computes global positions via `getWorldFinalMatrix()`. Memory is safely managed using `std::unique_ptr` for child nodes.
- **`List` (`list.h`)**: Inherits from `Object`. It manages a collection of rendering instances. Instead of traversing the tree during the render pass, the engine can flatten the hierarchy into a `List` of `Instance` structs (Node + WorldMatrix) to optimize and order the rendering process.

### 2. Geometry & Appearance
These classes manage 3D shapes and their visual surfaces.
- **`Mesh` (`mesh.h`)**: Inherits from `Node`. Encapsulates 3D geometry data including arrays of vertices, normals for lighting (`glm::vec3`), texture coordinates (`glm::vec2`), and face indices. It supports toggling rendering modes (e.g., `isWireframe`).
- **`Material` (`material.h`)**: Encapsulates surface shading properties (ambient, diffuse, specular colors, and shininess factor) to interact with the lighting system. Shared via `std::shared_ptr`.
- **`Texture` (`texture.h`)**: Handles the loading, OpenGL binding, and management of 2D image data that wraps around the `Mesh` via its `Material`.

### 3. Lighting System
All light classes inherit from a common base to provide uniform interaction with the rendering pipeline.
- **`Light` (`light.h`)**: The base class for all light sources. Inherits from `Node`, meaning lights can be attached to moving objects in the scene graph.
- **`OmnidirectionalLight` (`omnidirectionalLight.h`)**: A point light source that radiates energy uniformly in all directions from a specific coordinate.
- **`SpotLight` (`spotLight.h`)**: A light source that emits a cone of light in a specific direction, complete with cut-off angles and distance attenuation.
- **`InfiniteLight` (`infiniteLight.h`)**: A directional light simulating an infinitely distant source (like the sun), where all light rays are strictly parallel.

### 4. Cameras
Cameras inherit from `Node` and define how the 3D scene is projected onto the 2D window.
- **`Camera` (`camera.h`)**: Base class defining the view matrices and frustum properties.
- **`PerspectiveCamera` (`perspectiveCamera.h`)**: Implements realistic 3D perspective projection. Used primarily as the main tracking camera for the vehicle simulation.
- **`OrthographicCamera` (`orthographicCamera.h`)**: Implements 2D parallel projection without depth distortion. Ideal for rendering UI elements, HUDs, or minimaps.

### 5. Asset Pipeline
- **`OvoReader` (`ovoReader.h`)**: A sophisticated, recursive binary parser designed to load custom `.ovo` 3D model files (like the `macchina.ovo` vehicle model).
  - It parses binary chunks (`OvMesh`, `OvLight`, `OvMaterial`).
  - Automatically reconstructs the exact parent-child Node hierarchy defined in the 3D modeling software.
  - Utilizes a `std::map<std::string, std::shared_ptr<Material>>` to intelligently cache materials and textures, preventing redundant memory allocations and disk I/O.

### 6. System Control & Utilities
- **`Base` (`engine.h`)**: A Facade Singleton that centralizes the engine's lifecycle.
  - **PIMPL Idiom**: Uses the Pointer-to-Implementation pattern to hide specific windowing and OpenGL dependencies (e.g., GLUT/GLFW) from the client application.
  - **Main Loop**: Exposes `update()` and `render()` to drive the variable-framerate rendering pipeline while maintaining accurate delta-time.
  - **Callbacks**: Handles input and window events by allowing the client to register standard functions (`setDisplayCallback`, `setKeyboardCallback`, etc.).
  - **HUD / Overlay**: Provides built-in utility functions for rendering 2D text overlays (FPS, telemetry).
- **`libConfig.h`**: A utility header containing macro definitions for platform-specific configurations (like `ENG_API` for Windows DLL export/import rules).

## Interaction with the Client
The engine is intentionally kept strictly agnostic of the simulation's physics or game rules. The `client` relies on the engine's callback system to receive user input and execute fixed-timestep physics updates, ensuring that visual framerate fluctuations do not impact the deterministic nature of the vehicle dynamics.
