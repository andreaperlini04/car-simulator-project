# car-simulator-project

The objective of this semester project is to develop a real-time 3D car simulator, with a
strong focus on vehicle dynamics and user interaction. To maximize the effort spent on
the simulation logic, the project leverages a pre-existing custom C++ and OpenGL graphics
engine, extending it with a newly developed, highly decoupled client architecture. The core
system utilizes a Facade pattern to strictly separate the hardware input polling, the fixed-
timestep physics solver, and the variable-framerate visual rendering.
The implemented physics model handles longitudinal and lateral forces, simulating realistic
acceleration, braking, and a dynamic steering response that adapts to the vehicle’s speed.
As a key advanced feature, the simulation accurately models vehicle inertia: the system cal-
culates tire friction limits and lateral forces, allowing the car to realistically lose traction and
perform drifts during high-speed cornering. Combined a dynamic tracking camera, the pro-
ject successfully delivers an immersive and performant driving experience while maintaining
a clean, modular, and renderer-agnostic codebase.



For more detailed information about the architecture, systems, and controls, please see the [Client Documentation](./client/README.md).