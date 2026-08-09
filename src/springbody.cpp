#include "springbody.h"
#include <godot_cpp/core/class_db.hpp>
#include <algorithm>
#include <cmath>

using namespace godot;
using namespace std;

void SpringBody2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_on_body_entered", "body"), &SpringBody2D::_on_body_entered);
	ClassDB::bind_method(D_METHOD("_on_body_exited", "body"), &SpringBody2D::_on_body_exited);
	ClassDB::bind_method(D_METHOD("set_spring_force", "s_force"), &SpringBody2D::set_spring_force);
    ClassDB::bind_method(D_METHOD("get_spring_force"), &SpringBody2D::get_spring_force);
	ClassDB::bind_method(D_METHOD("set_max_force", "m_force"), &SpringBody2D::set_max_force);
    ClassDB::bind_method(D_METHOD("get_max_force"), &SpringBody2D::get_max_force);
	ClassDB::bind_method(D_METHOD("set_minimum_force", "m_force"), &SpringBody2D::set_minimum_force);
    ClassDB::bind_method(D_METHOD("get_minimum_force"), &SpringBody2D::get_minimum_force);
	ClassDB::bind_method(D_METHOD("set_growth_force", "g_force"), &SpringBody2D::set_growth_force);
    ClassDB::bind_method(D_METHOD("get_growth_force"), &SpringBody2D::get_growth_force);
	ClassDB::bind_method(D_METHOD("set_threshold", "thresh"), &SpringBody2D::set_threshold);
    ClassDB::bind_method(D_METHOD("get_threshold"), &SpringBody2D::get_threshold);
	ClassDB::bind_method(D_METHOD("set_normal_weight", "weight"), &SpringBody2D::set_normal_weight);
    ClassDB::bind_method(D_METHOD("get_normal_weight"), &SpringBody2D::get_normal_weight);
	ClassDB::bind_method(D_METHOD("get_buildup", "body"), &SpringBody2D::get_buildup);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "release_magnitude", PROPERTY_HINT_RANGE, "0,25,0.1"),"set_spring_force","get_spring_force");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_force", PROPERTY_HINT_RANGE, "0,1000,0.1"),"set_max_force","get_max_force");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_buildup", PROPERTY_HINT_RANGE, "0,1000,0.1"),"set_minimum_force","get_minimum_force");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "threshold", PROPERTY_HINT_RANGE, "0,1,0.01"),"set_threshold","get_threshold");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "growth_mult", PROPERTY_HINT_RANGE, "0,100,0.1"),"set_growth_force","get_growth_force");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "normal_weight", PROPERTY_HINT_RANGE, "0.6,1,0.01"),"set_normal_weight","get_normal_weight");
}



SpringBody2D::SpringBody2D() {
	// Initialize any variables here.
	set_notify_transform(true);
	set_process(true);
	MAX_FORCE = 100;
	SPRING_FORCE = 10;
	SPRING_GROWTH_RATE = 50;
	MINIMUM_FORCE = 250;
	activation = 0.5;
	normal_weight = 0.7;
	force_weight = 0.3;
	physics_delta = 1.0f / (float)Engine::get_singleton()->get_physics_ticks_per_second();
	geom = Geometry2D::get_singleton();
}

void SpringBody2D::set_minimum_force(float m_force) {MINIMUM_FORCE = m_force;}
float SpringBody2D::get_minimum_force() const {return MINIMUM_FORCE;}
void SpringBody2D::set_spring_force(float s_force) { SPRING_FORCE = s_force; }
float SpringBody2D::get_spring_force() const { return SPRING_FORCE; }
void SpringBody2D::set_threshold(float thresh) { activation = thresh; }
float SpringBody2D::get_threshold() const { return activation; }
void SpringBody2D::set_normal_weight(float weight) {normal_weight = weight, force_weight = 1 - weight;}
float SpringBody2D::get_normal_weight() const { return normal_weight; }
void SpringBody2D::set_max_force(float m_force) { MAX_FORCE = m_force; }
float SpringBody2D::get_max_force() const { return MAX_FORCE; }
void SpringBody2D::set_growth_force(float g_force) { SPRING_GROWTH_RATE = g_force; }
float SpringBody2D::get_growth_force() const { return SPRING_GROWTH_RATE; }
float SpringBody2D::get_buildup(RigidBody2D* rb) const {
    auto it = spring_targets.find(rb);
    if (it != spring_targets.end())
        return it->second.buildUp;
    return -1.0f;
}

void SpringBody2D::_ready() {
	for (int i = 0; i < get_child_count(); i++) {
		if (CollisionPolygon2D* candidate = Object::cast_to<CollisionPolygon2D>(get_child(i))) {
			poly = candidate;
			break;
		}
	}
	if (!poly) {
		print_error("SpringBody2D Requires a CollisionPolygon2D to Function Properly");
		return;
	}
	connect("body_entered", callable_mp(this, &SpringBody2D::_on_body_entered));
	connect("body_exited", callable_mp(this, &SpringBody2D::_on_body_exited));
}

PackedStringArray SpringBody2D::_get_configuration_warnings() const {
	PackedStringArray warnings;
	for (int i = 0; i < get_child_count(); i++) {
		Node* child = get_child(i);
		if (Object::cast_to<CollisionPolygon2D>(child)) 
			return warnings;
	}
	warnings.push_back("This node requires a CollisionPolygon2D to function properly");
	return warnings;
}

void SpringBody2D::_notification(int p_what) {
	if (p_what == NOTIFICATION_CHILD_ORDER_CHANGED || p_what == NOTIFICATION_READY)
		update_configuration_warnings();
}

void SpringBody2D::_physics_process(double delta) {
	for (auto& it : spring_targets) {
		SpringTarget& spring = it.second;
		Vector2 vel = it.first->get_linear_velocity();
		Vector2 normal = spring.collision_normal;	

		if (normal == Vector2(0,0)) {
			update_configuration_warnings();
			spring.collision_normal = _calculate_surface_normal(it.first, spring);
		} else {
			Vector2 gravity = it.first->get_gravity();
			it.first->apply_central_force(-gravity * it.first->get_mass());

			Vector2 temp = _calculate_surface_normal(it.first, spring);

			if (temp != Vector2(0,0))
				spring.collision_normal = normal = temp;

			Vector2 vel_normal = normal * vel.dot(normal);
			Vector2 vel_tangent = vel - vel_normal;

			if ((vel * delta).dot(normal) < -activation)
				spring.buildUp += -vel.dot(normal) * delta * SPRING_GROWTH_RATE;
			else if (spring.buildUp <= MINIMUM_FORCE)
				spring.buildUp = MINIMUM_FORCE;
			
			float decay = exp(-delta * spring.buildUp);
			vel_normal *= decay;
			vel_tangent *= decay;

			Vector2 impulse = (vel_normal + vel_tangent - vel);
			if ((vel * delta).dot(normal) >= -activation) {
				Vector2 force_dir = Vector2(spring.init_force[0], spring.init_force[1]).normalized();
				Vector2 dir = (force_dir * force_weight + spring.collision_normal * normal_weight).normalized();
				impulse = dir * max(MINIMUM_FORCE, spring.buildUp) * SPRING_FORCE * delta;
			}

			// Clamp maximum force
			if (impulse.length() > MAX_FORCE)
				impulse = impulse.normalized() * MAX_FORCE;
			
			// Apply impulse
			it.first->apply_central_impulse(impulse);
		}
    }
}

void SpringBody2D::_on_body_entered(Node *body) {
	if (body->is_class("RigidBody2D")) {
		RigidBody2D *rb = Object::cast_to<RigidBody2D>(body);
		Vector2 prev_pos = rb->get_global_position() - rb->get_linear_velocity() * physics_delta;
		spring_targets[rb] = SpringTarget{rb->get_linear_velocity(), prev_pos, Vector2(0,0), 0};
	}
}

void SpringBody2D::_on_body_exited(Node *body) {
    if (body->is_class("RigidBody2D")) {
        RigidBody2D *rb = Object::cast_to<RigidBody2D>(body);
        if (spring_targets.find(rb) != spring_targets.end()) {
            spring_targets.erase(rb);
        } else {
            print_error("SpringBody2D: body exited but was not in spring_targets — " + rb->get_name());
        }
    }
}

Vector2 SpringBody2D::_calculate_surface_normal(RigidBody2D* rb, const SpringTarget& spring) {
	if (!poly) {
		print_error("SpringBody2D: _calculate_surface_normal called but poly is null — ensure a CollisionPolygon2D is a direct child");
		return Vector2(0,0);
	}
    Vector2 p0 = spring.init_pos, p1 = rb->get_global_position();
    Vector2 vel = rb->get_linear_velocity().normalized();

    if (vel.length() == 0)
        return Vector2(0,0);

    PackedVector2Array poly_points = poly->get_polygon();
    Transform2D xf = poly->get_global_transform();

	if (!geom) {
		print_error("SpringBody2D: Geometry2D singleton unavailable");
    	return Vector2(0,0);
	}

    Vector2 best_normal = Vector2(0,0);
    float best_dist = FLT_MAX;

	for (int i = 0; i < poly_points.size(); i++) {
		Vector2 a = xf.xform(poly_points[i]);
		Vector2 b = xf.xform(poly_points[(i+1)%poly_points.size()]);

		if (!geom->segment_intersects_segment(p0, p1, a, b))
			continue;
		Vector2 edge = b - a;
		Vector2 normal = Vector2(-edge.y, edge.x).normalized();

		Vector2 edge_mid = (a + b) * 0.5f;
		Vector2 to_body = rb->get_global_position() - edge_mid;

		// Enforce correct normal orientation
		if (normal.dot(to_body) < 0)
			normal = -normal;

		float dist = p0.distance_to(edge_mid);
		if (dist < best_dist) {
			best_dist = dist;
			best_normal = normal;
		}
	}

    return best_normal * -1;
}