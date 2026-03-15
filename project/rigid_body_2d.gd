extends RigidBody2D

var dir_vert = "N/A"
var dir_hor = "N/A"
@onready var arrow: Polygon2D = get_node("../Polygon2D")
func _process(_delta: float) -> void:
	if (Input.is_action_just_pressed("press_down") or dir_vert == "down"):
		apply_central_impulse(Vector2(0, 10))
		arrow.visible = true
		dir_vert = "down"
	if (Input.is_action_just_pressed("press_up") or dir_vert == "up"):
		apply_central_impulse(Vector2(0,-10))
		arrow.visible = true
		dir_vert = "up"
	if (Input.is_action_just_pressed("press_right") or dir_hor == "right"):
		apply_central_impulse(Vector2(10,0))
		arrow.visible = true
		dir_hor = "right"
	if (Input.is_action_just_pressed("press_left") or dir_hor == "left"):
		apply_central_impulse(Vector2(-10,0))
		arrow.visible = true
		dir_hor = "left"
	if (Input.is_action_just_pressed("press_shift")):
		dir_hor = "N/A"
		arrow.visible = false
		dir_vert = "N/A"
		
		
	if (dir_vert == "N/A" or dir_hor == "N/A"):
		if (dir_vert == "up"): arrow.rotation_degrees = 0
		if (dir_vert == "down"): arrow.rotation_degrees = 180
		if (dir_hor == "right"): arrow.rotation_degrees = 90
		if (dir_hor == "left"): arrow.rotation_degrees = 270
	else: if (dir_vert != "N/A" and dir_hor != "N/A"):
		if (dir_vert == "down"):
			if (dir_hor == "right"): arrow.rotation_degrees = 135
			if (dir_hor == "left"): arrow.rotation_degrees = 225
		else: if (dir_vert == "up"):
			if (dir_hor == "right"): arrow.rotation_degrees = 45
			if (dir_hor == "left"): arrow.rotation_degrees = 315
