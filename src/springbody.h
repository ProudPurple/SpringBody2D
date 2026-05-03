#pragma once
#include <unordered_map>
#include <cfloat>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/geometry2d.hpp>
#include <godot_cpp/classes/rigid_body2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/collision_polygon2d.hpp>
#include <godot_cpp/classes/area2d.hpp>

namespace godot {
	struct SpringTarget {
		Vector2 init_force;
		Vector2 init_pos;
		Vector2 collision_normal;
		float buildUp;
	};

	class SpringBody2D : public Area2D{
		GDCLASS(SpringBody2D, Area2D);

	private:
		CollisionPolygon2D* poly = nullptr;
		float SPRING_GROWTH_RATE;
		float MAX_FORCE;
		float SPRING_FORCE;
		float normal_weight;
		float force_weight;
		float activation;
		float MINIMUM_FORCE;
		std::unordered_map<RigidBody2D*, SpringTarget> spring_targets;
		Geometry2D* geom = nullptr;
		float physics_delta;

	protected:
		static void _bind_methods();

	public:
		SpringBody2D();

		void _ready() override;
		void set_spring_force(float s_force);
		void set_minimum_force(float m_force);
		float get_minimum_force() const;
		float get_spring_force() const;
		void set_threshold(float thresh);
		float get_threshold() const;
		void set_normal_weight(float weight);
		float get_normal_weight() const;
		void set_max_force(float m_force);
		float get_max_force() const;
		void set_growth_force(float g_force);
		float get_growth_force() const;
		void _physics_process(double delta) override;
		void _notification(int p_what);
		void _on_body_entered(Node *body);
		void _on_body_exited(Node *body);
		virtual PackedStringArray _get_configuration_warnings() const override;
		Vector2 _calculate_surface_normal(RigidBody2D* rb, const SpringTarget& spring);

	};

}