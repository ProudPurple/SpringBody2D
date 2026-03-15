# SpringBody2D

A Godot 4 GDExtension plugin that adds a `SpringBody2D` node, which applies a customizable spring effect to incoming `RigidBody2D` nodes using physics calculations.

![Image](https://github.com/user-attachments/assets/012780b7-403e-4426-b8b7-e1792f3fcb8d)

## Installation

1. Copy the `addons/springbody2d` folder into your project's `addons/` folder
2. Go to **Project → Project Settings → Plugins**
3. Enable the **SpringBody2D** plugin

> Requires a `CollisionPolygon2D` as a direct child to function properly.

## Properties

| Property | Description |
|----------|-------------|
| **Release Magnitude** | Multiplier applied to the release force |
| **Max Force** | Upper limit on the release force |
| **Min Buildup** | Minimum velocity buildup before release |
| **Threshold** | Minimum dot product of velocity against the polygon wall required to trigger release |
| **Growth Mult** | Multiplier on the rate of buildup growth |
| **Normal Weight** | Weighting between the surface normal direction and the RigidBody2D's entry direction |

## Requirements
- Godot 4.3+
- Windows x86_64 (other platforms require building from source)
