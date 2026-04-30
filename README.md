# Real-Time 3D Car Simulator 🏎️

The objective of this semester project is to develop a real-time 3D car simulator, with a strong focus on vehicle dynamics and user interaction. 

To maximize the effort spent on the simulation logic, the project leverages a pre-existing custom C++ and OpenGL graphics engine, extending it with a newly developed, highly decoupled client architecture. The core system utilizes a Facade pattern to strictly separate the hardware input polling, the fixed-timestep physics solver, and the variable-framerate visual rendering.

## Key Features
* **Advanced Physics Model**: Handles longitudinal and lateral forces, simulating realistic acceleration, braking, and a dynamic steering response that adapts to the vehicle’s speed.
* **Vehicle Inertia & Drifting**: As a key advanced feature, the simulation accurately models vehicle inertia. The system calculates tire friction limits and lateral forces, allowing the car to realistically lose traction and perform drifts during high-speed cornering.
* **Dynamic Tracking Camera**: Combined with a dynamic tracking camera, the project successfully delivers an immersive and performant driving experience.
* **Modular Architecture**: Maintains a clean, modular, and renderer-agnostic codebase.

## Project Structure

The repository is divided into two primary modules:

- **[Client](./client)**: The core simulation logic, handling car physics, input processing, and state management.
- **[Engine](./engine)**: The custom C++ and OpenGL graphics engine handling the 3D scene graph, rendering, and resource loading.

---

### 📖 Documentation

For more detailed information about the architecture, systems, and controls, please see the dedicated documentation files:

- 🎮 **[Client Documentation (Simulation & Physics)](./client/README.md)**
- ⚙️ **[Engine Documentation (Rendering & Core System)](./engine/README.md)**