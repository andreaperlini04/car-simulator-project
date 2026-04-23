#pragma once

/**
 * @brief Mapping dimensionale tra la scena 3ds Max e il solver fisico in SI (metri).
 * Garantisce che forze, accelerazioni e velocità scalino realisticamente
 * indipendentemente dalle unità di export del modello.
 */

 // Geometria di riferimento exportata
constexpr double PLANE_REAL_SIZE_M = 2000.0;
constexpr double PLANE_UNITS = 78740.195;
constexpr double CAR_REAL_SIZE_M = 1.351;
constexpr double CAR_UNITS = 53.196;

// L'unità di default di 3ds Max è 1 inch (0.0254 m)
constexpr double METERS_PER_UNIT = PLANE_REAL_SIZE_M / PLANE_UNITS;

// --- Parametri dinamici base (SI: m/s, m/s^2) ---
constexpr double MAX_SPEED_KMH = 100.0;
constexpr double MAX_SPEED_MS = MAX_SPEED_KMH / 3.6;

// Target prestazionali per derivare l'accelerazione lineare
constexpr double ACCEL_MS2 = MAX_SPEED_MS / 10.0; // Target: 0-100 km/h in ~10s
constexpr double BRAKE_MS2 = MAX_SPEED_MS / 3.0;  // Target: 100-0 km/h in ~3s

constexpr double FRICTION_MS2 = 0.5;           // Resistenza base al rotolamento
constexpr double REVERSE_MAX_KMH = 30.0;
constexpr double REVERSE_MAX_MS = REVERSE_MAX_KMH / 3.6;

// --- Conversioni finali per il solver (units/s, units/s^2) ---
constexpr double GAME_MAX_SPEED = MAX_SPEED_MS / METERS_PER_UNIT;
constexpr double GAME_ACCEL = ACCEL_MS2 / METERS_PER_UNIT;
constexpr double GAME_BRAKE = BRAKE_MS2 / METERS_PER_UNIT;
constexpr double GAME_FRICTION = FRICTION_MS2 / METERS_PER_UNIT;
constexpr double GAME_REVERSE_MAX = REVERSE_MAX_MS / METERS_PER_UNIT;

// --- Timestep ---
constexpr double PHYSICS_FIXED_DT = 1.0 / 60.0;
constexpr double PHYSICS_MAX_FRAME = 0.25; // Previene la "spiral of death" dell'accumulatore su lag spike

// Single source of truth per il clamp hardware/software dello sterzo
constexpr double MAX_STEERING_ANGLE_DEG = 35.0;