# Car Simulator – Client

A real-time 3D car simulator written in C++ using an OpenGL-based engine. The client is responsible for loading the scene, simulating vehicle physics, handling player input, and rendering the car in the scene graph.

---

## Project Structure

```
client/
├── main.cpp                  # Entry point
├── Application.h / .cpp      # Root controller – scene lifecycle & render loop
├── Car.h / .cpp              # Facade aggregating the three car subsystems
├── CarInputController.h/.cpp # Input translation and steering smoothing
├── CarPhysics.h / .cpp       # Fixed-step physics integration
├── CarRenderer.h / .cpp      # Scene-graph synchronisation and wheel animation
├── CarState.h                # Shared data structs (CarInputState, CarPhysicsState)
├── Wheel.h / .cpp            # Per-wheel transform and rolling animation
├── WorldConfig.h             # Global constants: physics units, timestep, limits
└── macchina.ovo              # 3D scene asset (OVO format, exported from 3ds Max)
```

---

## Architecture Overview

The design separates concerns into three clearly bounded layers:

```
┌──────────────────────────────────────────┐
│                Application               │  ← Root controller
│  (scene init, main loop, camera, input)  │
└────────────────────┬─────────────────────┘
                     │ owns
                     ▼
┌──────────────────────────────────────────┐
│                   Car                    │  ← Facade
│     (public API, orchestrates steps)     │
└───────┬─────────────────┬───────────────┘
        │                 │                 \
        ▼                 ▼                  ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐
│ CarInput-    │  │  CarPhysics  │  │ CarRenderer  │
│ Controller   │  │              │  │              │
│              │→ │              │→ │              │
│ (smoothing,  │  │ (fixed-step  │  │ (scene-graph │
│  clamping)   │  │  integrator) │  │  sync, Wheel │
└──────────────┘  └──────────────┘  │  animation)  │
                                    └──────┬───────┘
                                           │ owns (x4)
                                           ▼
                                    ┌──────────────┐
                                    │    Wheel     │
                                    │ (per-wheel   │
                                    │  transform)  │
                                    └──────────────┘
```

---

## Class Descriptions

### `Application`

The top-level singleton-style controller. It owns the engine reference, the scene graph (`Node` tree), the OVO reader, the `Car` instance, and all camera state.

**Responsibilities:**
- Initialises the OpenGL engine and loads the scene from `macchina.ovo`.
- Reparents light nodes under the car chassis so they remain aligned in turns.
- Manages the **fixed-timestep physics loop** using a `physicsAccumulator` to decouple the physics tick rate (60 Hz) from the render framerate.
- Handles all GLUT/engine callbacks (`onDisplay`, `onKeyboard`, `onKeyboardUp`, `onMouseMotion`, `onReshape`) and delegates them to the appropriate subsystems.
- Implements three camera modes: fixed-behind, fixed-left/right, and an **orbit camera** driven by mouse drag.

**Fixed-timestep loop (inside `onDisplay`):**
```
accumulator += frameDelta (capped at PHYSICS_MAX_FRAME)
while accumulator >= PHYSICS_FIXED_DT:
    car.stepPhysics(PHYSICS_FIXED_DT)
    accumulator -= PHYSICS_FIXED_DT
car.updateRenderer(frameDelta)
```
This prevents the "spiral of death" on lag spikes and keeps the physics deterministic.

---

### `Car`

A **Facade** that exposes a single, clean API to `Application` while hiding the three internal subsystems. It owns `CarInputController`, `CarPhysics`, and `CarRenderer` by value (no heap allocation needed).

The two core methods enforce the fixed-step / variable-render separation:

| Method | Purpose | Called from |
|---|---|---|
| `stepPhysics(fixedDt)` | Update steering smoothing + physics integration | Physics accumulator loop |
| `updateRenderer(frameDt)` | Sync scene-graph transforms + wheel animation | Once per frame |

---

### `CarInputController`

Translates raw hardware signals (key up/down flags, mouse delta) into the `CarInputState` struct consumed by `CarPhysics`. It never touches the physics or scene graph directly.

**Key behaviours:**
- **Keyboard steering**: Linear interpolation at `STEERING_SPEED` deg/s with auto-centering when no key is held.
- **Mouse steering**: Tracks a `mouseSteeringTarget` with the same linear interpolation to avoid snap jumps.
- Clamps all steering output to `±MAX_STEERING_ANGLE_DEG` (35°, defined in `WorldConfig.h`).

---

### `CarPhysics`

Runs a **semi-implicit Euler integration** at a fixed timestep. All state is stored in `CarPhysicsState` (position, heading, speed, velocity vector).

**Update pipeline (per tick):**

```
1. applyGas      – accelerate / brake / reverse gear logic
   └─ applyFriction – rolling resistance when coasting
2. applyTireScrub – bleed speed proportional to steering angle (cornering drag)
3. applyTurnAndGrip
   ├─ turnRate   = steeringAngle × speed × TURN_RATE_COEFF (× driftTurnBoost if handbrake)
   └─ grip       = friction / (|speed| × GRIP_SPEED_SCALE + 1)  [understeer model]
4. applyInertia  – lerp velocity vector toward the forward direction via lateralGrip
   └─ handbrake path: reduce lateralGrip (oversteer/drift) + extra kinetic dissipation
5. Integrate position: posX += velX × dt,  posZ += velZ × dt
```

All physics constants are derived from real-world SI values in `WorldConfig.h` and then converted to engine units using the 3ds Max → scene scale factor.

---

### `CarRenderer`

Bridges the physics state and the engine's scene graph. It does **not** contain any physics logic.

**Init phase:**
- Walks the scene graph to find the `"Car"` node.
- Extracts the OVO spawn position and forwards it to `CarPhysics::setInitialPosition`.
- Reparents the four wheel nodes (`RuotaAD`, `RuotaAS`, `RuotaPD`, `RuotaPS`) under the car node so they inherit chassis transforms.
- Constructs four `Wheel` instances with their relative offsets.

**Per-frame update:**
- Builds a `translate × rotateY(heading)` matrix from the physics state and writes it to the car node.
- Computes tangential speed from the velocity vector (not scalar speed) so wheel roll is correct even during drift.
- Applies steering yaw only to the front wheels (`[0]`, `[1]`); rear wheels are locked to 0°.

---

### `Wheel`

Manages the local transform of a single wheel node inside the scene graph.

**Transform composition order (critical):**
```
M = baseMatrix
  × rotateY(steeringAngle)   ← yaw first (redefines local frame)
  × rotateX(-rollingAngle)   ← pitch in the new frame
```
Reversing this order would cause the wheel to roll on the wrong axis during turns.

The rolling angle is accumulated from `distanceMoved / radius` and wrapped to `[-360°, 360°]` to prevent floating-point drift over long sessions.

---

### Data Structs (`CarState.h`)

Two plain structs serve as the **inter-subsystem contracts**:

```cpp
struct CarInputState {
    bool   isAccelerating, isBraking, isHandbrake;
    double steeringAngle;   // deg, positive = left
};

struct CarPhysicsState {
    double posX, posY, posZ;
    double carHeading;      // deg, world-space
    double currSpeed;
    double velX, velZ;      // Actual velocity vector (differs from forward during drift)
};
```

`CarPhysicsState` is the key decoupling point: `CarPhysics` writes it, `CarRenderer` reads it — neither subsystem knows about the other.

---

### `WorldConfig.h`

Single source of truth for all tunable constants. Physics parameters are defined in **SI units** (m, m/s, m/s²) and then converted to engine units using the scene scale factor derived from the OVO geometry:

```
METERS_PER_UNIT = PLANE_REAL_SIZE_M / PLANE_UNITS   (~0.0254 m/unit ≈ 1 inch)
```

Key constants:

| Constant | Value | Description |
|---|---|---|
| `PHYSICS_FIXED_DT` | 1/60 s | Physics tick rate |
| `PHYSICS_MAX_FRAME` | 0.25 s | Accumulator cap (prevents spiral of death) |
| `MAX_STEERING_ANGLE_DEG` | 35° | Hardware/software steering limit |
| `MAX_SPEED_KMH` | 100 km/h | Top speed |
| `ACCEL_MS2` | ~2.78 m/s² | Derived: 0→100 km/h in 10 s |
| `BRAKE_MS2` | ~9.26 m/s² | Derived: 100→0 km/h in 3 s |

---

## Key Design Decisions

### Physics / Render Decoupling
Physics runs at a **fixed 60 Hz** inside an accumulator loop, while rendering runs at the native framerate. The `Car` facade enforces this split with its two separate methods (`stepPhysics` / `updateRenderer`), making the simulation deterministic and framerate-independent.

### Facade Pattern (`Car`)
`Application` never interacts directly with `CarPhysics`, `CarRenderer`, or `CarInputController`. This keeps `Application` focused on orchestration and makes each subsystem independently testable.

### Static C-style Callback Workaround
GLUT requires C-style function pointers for callbacks. `Application` registers static wrapper functions that forward calls to a global `Application` instance pointer, preserving the OOP design without changing the engine's callback API.

### Scene Graph Ownership
Wheel nodes are **reparented** under the car node during init so that the engine's matrix propagation handles the parent→child transform chain automatically. `CarRenderer` holds raw (non-owning) `Node*` pointers; the scene graph itself manages lifetime.

---

## Input Controls

| Key | Action |
|---|---|
| `E` | Start engine |
| `T` | Stop engine |
| `W` / `S` | Accelerate / Brake & Reverse |
| `A` / `D` | Steer left / right |
| `Space` | Handbrake (triggers drift) |
| `M` | Toggle orbit (mouse-look) camera |
| `N` | Toggle mouse steering |
| `1` / `2` / `3` | Switch camera position (behind / left / right) |
| `U` | Toggle wireframe on the ground mesh |

---

## Build

The project supports multiple build systems:

- **Visual Studio**: `client.vcxproj`
- **Code::Blocks**: `client.cbp`
- **Make**: `Makefile`


