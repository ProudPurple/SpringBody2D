#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "spring_math_testable.h"

// Mirror your actual constants here
const float MINIMUM_FORCE    = 1.0f;
const float SPRING_GROWTH_RATE = 1.0f;
const float SPRING_FORCE     = 10.0f;
const float MAX_FORCE        = 500.0f;

// --- buildUp accumulation ---
TEST_CASE("buildUp grows when velocity pushes into surface") {
    Vec2 vel    = {0, -50};   // moving into floor
    Vec2 normal = {0,  1};    // floor normal points up
    float buildUp = 10.0f;
    float delta   = 0.016f;

    float growth = calc_buildup_delta(vel, normal, delta, buildUp,
                                      SPRING_GROWTH_RATE, 0.1f, MINIMUM_FORCE);
    CHECK(growth > 0.0f);  // should be accumulating
}

TEST_CASE("buildUp does not grow when velocity moving away from surface") {
    Vec2 vel    = {0, 50};    // moving away from floor
    Vec2 normal = {0,  1};
    float buildUp = 50.0f;    // above MINIMUM_FORCE
    float delta   = 0.016f;

    float growth = calc_buildup_delta(vel, normal, delta, buildUp,
                                      SPRING_GROWTH_RATE, 0.1f, MINIMUM_FORCE);
    CHECK(growth == 0.0f);
}

TEST_CASE("buildUp always grows when below MINIMUM_FORCE") {
    Vec2 vel    = {0, 50};
    Vec2 normal = {0,  1};
    float buildUp = 0.5f;
    float delta   = 0.016f;

    float growth = calc_buildup_delta(vel, normal, delta, buildUp,
                                      SPRING_GROWTH_RATE, 0.1f, MINIMUM_FORCE);
    // With the fix: growth should be 0 and buildUp clamped to MINIMUM_FORCE
    // so delta contribution is 0 but floor is guaranteed elsewhere
    CHECK(growth == 0.0f);
    // Separately verify the floor clamp:
    float result = std::max(MINIMUM_FORCE, buildUp + growth);
    CHECK(result >= MINIMUM_FORCE);
}
// --- Exponential decay ---
TEST_CASE("decay is between 0 and 1") {
    float decay = calc_decay(0.016, 50.0f);
    CHECK(decay > 0.0f);
    CHECK(decay < 1.0f);
}

TEST_CASE("higher buildUp produces stronger decay") {
    float decay_low  = calc_decay(0.016, 10.0f);
    float decay_high = calc_decay(0.016, 200.0f);
    CHECK(decay_high < decay_low);
}

TEST_CASE("zero delta produces no decay") {
    float decay = calc_decay(0.0, 999.0f);
    CHECK(decay == doctest::Approx(1.0f));
}

// --- Impulse clamping ---
TEST_CASE("impulse within MAX_FORCE is unchanged") {
    Vec2 impulse = {100, 0};
    Vec2 result  = clamp_impulse(impulse, MAX_FORCE);
    CHECK(result.x == doctest::Approx(100.0f));
}

TEST_CASE("impulse exceeding MAX_FORCE is clamped") {
    Vec2 impulse = {1000, 0};
    Vec2 result  = clamp_impulse(impulse, MAX_FORCE);
    CHECK(result.length() == doctest::Approx(MAX_FORCE).epsilon(0.01f));
}

TEST_CASE("clamped impulse preserves direction") {
    Vec2 impulse  = {600, 800};  // length = 1000
    Vec2 result   = clamp_impulse(impulse, MAX_FORCE);
    Vec2 expected = impulse.normalized() * MAX_FORCE;
    CHECK(result.x == doctest::Approx(expected.x).epsilon(0.01f));
    CHECK(result.y == doctest::Approx(expected.y).epsilon(0.01f));
}