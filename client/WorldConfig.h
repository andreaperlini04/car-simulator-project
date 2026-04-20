#pragma once

// WorldConfig.h - World scale and physics constants
/*
    All real-world dimensions come from the 3D Studio Max scene.
    The conversion factor translates between 3DS Max units and
    meters so that in-game physics behave realistically.
*/

// --- World geometry (from 3D Studio Max) ---
constexpr double PLANE_REAL_SIZE_M =
    2000.0;                               // Plane: 2000 m × 2000 m (real world)
constexpr double PLANE_UNITS = 78740.195; // Plane: 78 740.195 units (3DS Max)
constexpr double CAR_REAL_SIZE_M = 1.351; // Car length: 1.351 m (real world)
constexpr double CAR_UNITS = 53.196;      // Car length: 53.196 units (3DS Max)

// --- Unit conversion ---
// 3DS Max default unit is 1 inch = 0.0254 m
constexpr double METERS_PER_UNIT =
    PLANE_REAL_SIZE_M / PLANE_UNITS; // ~0.0254 m/unit

// --- Realistic physics parameters (SI: m/s, m/s^2) ---
constexpr double MAX_SPEED_KMH = 100.0;              // Top speed: 100 km/h
constexpr double MAX_SPEED_MS = MAX_SPEED_KMH / 3.6; // = 27.778 m/s
constexpr double ACCEL_MS2 =
    MAX_SPEED_MS / 10.0; // 0->100 in ~10 s => 2.778 m/s^2
constexpr double BRAKE_MS2 =
    MAX_SPEED_MS / 3.0;                  // 100->0 in ~3 s  => 9.259 m/s^2
constexpr double FRICTION_MS2 = 0.5;     // Rolling resistance
constexpr double REVERSE_MAX_KMH = 30.0; // Reverse top speed: 30 km/h
constexpr double REVERSE_MAX_MS = REVERSE_MAX_KMH / 3.6;

// --- Converted to game units (units/s, units/s^2) ---
constexpr double GAME_MAX_SPEED = MAX_SPEED_MS / METERS_PER_UNIT; // ~1093 u/s
constexpr double GAME_ACCEL = ACCEL_MS2 / METERS_PER_UNIT;        // ~109  u/s²
constexpr double GAME_BRAKE = BRAKE_MS2 / METERS_PER_UNIT;        // ~364  u/s²
constexpr double GAME_FRICTION = FRICTION_MS2 / METERS_PER_UNIT;  // ~19.7 u/s²
constexpr double GAME_REVERSE_MAX =
    REVERSE_MAX_MS / METERS_PER_UNIT; // ~328  u/s

constexpr double PHYSICS_FIXED_DT = 1.0 / 60.0;  // 60 Hz
constexpr double PHYSICS_MAX_FRAME = 0.25;         // spiral-of-death guard: max frameTime accettato
