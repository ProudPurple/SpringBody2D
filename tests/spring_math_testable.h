#pragma once
#include <cmath>
#include <algorithm>

// Minimal Vector2 stand-in — no Godot dependency
struct Vec2 {
    float x, y;
    Vec2(float x = 0, float y = 0) : x(x), y(y) {}

    Vec2 operator+(const Vec2& o) const { return {x+o.x, y+o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x-o.x, y-o.y}; }
    Vec2 operator*(float s)       const { return {x*s,   y*s};   }

    float dot(const Vec2& o) const { return x*o.x + y*o.y; }
    float length()           const { return std::sqrt(x*x + y*y); }
    Vec2  normalized()       const {
        float len = length();
        return len > 0 ? Vec2{x/len, y/len} : Vec2{0,0};
    }
};

// --- Extracted formulas ---

// Should buildUp grow this frame?
inline bool should_build(Vec2 vel, Vec2 normal, double delta,
                         float activation, float buildUp, float MINIMUM_FORCE) {
    return (vel * (float)delta).dot(normal) < -activation
           || buildUp <= MINIMUM_FORCE;
}

// How much does buildUp grow?
inline float calc_buildup_delta(Vec2 vel, Vec2 normal, double delta,
                                float buildUp, float SPRING_GROWTH_RATE,
                                float activation, float MINIMUM_FORCE) {
    if ((vel * (float)delta).dot(normal) < -activation)
        return -vel.dot(normal) * (float)delta * SPRING_GROWTH_RATE * 10.0f;
    else if (buildUp <= MINIMUM_FORCE)
        return 0.0f;  // floor is enforced by the caller clamping to MINIMUM_FORCE
    return 0.0f;
}

// Exponential decay factor
inline float calc_decay(double delta, float buildUp) {
    return std::exp(-(float)delta * buildUp);
}

// Clamp impulse to MAX_FORCE
inline Vec2 clamp_impulse(Vec2 impulse, float MAX_FORCE) {
    if (impulse.length() > MAX_FORCE)
        return impulse.normalized() * MAX_FORCE;
    return impulse;
}

// Final release impulse direction + magnitude
inline Vec2 calc_release_impulse(Vec2 force_dir, Vec2 collision_normal,
                                 float force_weight, float normal_weight,
                                 float buildUp, float MINIMUM_FORCE,
                                 float SPRING_FORCE, double delta) {
    Vec2 dir = (force_dir * force_weight + collision_normal * normal_weight).normalized();
    return dir * std::max(MINIMUM_FORCE, buildUp) * SPRING_FORCE * (float)delta;
}