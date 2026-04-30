# SpringBody2D

A Godot 4 GDExtension plugin that adds a `SpringBody2D` node — a physics-aware `Area2D` that stores kinetic energy from incoming `RigidBody2D` nodes and releases it as a directional impulse.

![demo](https://github.com/user-attachments/assets/012780b7-403e-4426-b8b7-e1792f3fcb8d)

## How it works

`SpringBody2D` wraps an `Area2D` with a `CollisionPolygon2D` boundary. When a `RigidBody2D` enters:

1. **Compression phase** — while the body moves into the surface (velocity dot surface-normal is negative), gravity is disabled and a `buildUp` counter grows. The body's velocity is damped exponentially each frame, so it slows down as if compressing a spring.
2. **Release phase** — once the body stops compressing (or `buildUp` exceeds `min_buildup`), an impulse fires. The direction is a weighted blend of the surface normal and the body's original entry direction; the magnitude is `max(min_buildup, buildUp) × release_magnitude`, capped at `max_force`.

Think of it as a pinball bumper: a slow graze barely moves the ball, a fast direct hit launches it hard.

## Installation

1. Copy the `addons/springbody2d` folder into your project's `addons/` folder.
2. Go to **Project → Project Settings → Plugins**.
3. Enable the **SpringBody2D** plugin.

> **Requires** a child `CollisionPolygon2D`. The editor shows a configuration warning if one is missing. At runtime a `print_error` is logged and the node becomes inactive.

## Typical starting values

| Property | Suggested start | Effect of increasing |
|----------|----------------|----------------------|
| `release_magnitude` | `5` | Stronger bounce |
| `max_force` | `500` | Raises the ceiling on impulse magnitude |
| `min_buildup` | `150` | More buildup required before release triggers |
| `threshold` | `0.3` | Responds to more glancing hits |
| `growth_mult` | `3` | Faster energy accumulation during compression |
| `normal_weight` | `0.8` | Closer to 1 = more mirror-like reflection |

## Properties

| Property | Description |
|----------|-------------|
| **Release Magnitude** | Multiplier on the outgoing impulse |
| **Max Force** | Hard cap on the impulse magnitude (prevents tunneling at high speeds) |
| **Min Buildup** | Minimum buildup required to trigger release; also the floor on outgoing impulse strength |
| **Threshold** | Dot-product threshold between velocity and surface normal — below this the body is considered to still be compressing |
| **Growth Mult** | Rate at which `buildUp` accumulates each frame during compression |
| **Normal Weight** | Blend between surface normal (`1.0`) and original entry velocity direction (`0.0`) for the release direction |

## Requirements

- Godot 4.3+
- Pre-built binaries are provided for **Windows x86\_64**, **Linux x86\_64**, and **macOS (universal arm64/x86\_64)**.
- Other platforms require building from source (see below).

## Building from source

`godot-cpp` is included as a git submodule. Clone with submodules, install SCons, then build:

```sh
git clone --recurse-submodules https://github.com/ProudPurple/softbody
cd softbody
pip install scons
scons platform=<windows|linux|macos>
```

The compiled library is written to `addons/springbody2d/bin/`.

**Dependencies:** Python 3, SCons, and a platform C++ compiler (MSVC on Windows, GCC or Clang on Linux, Xcode on macOS).

To build both debug and release targets:

```sh
scons platform=linux target=template_debug
scons platform=linux target=template_release
```

> If you already have the repo but cloned without `--recurse-submodules`, run `git submodule update --init --recursive` to pull in `godot-cpp`.
