@tool
extends EditorPlugin

const ICON = preload("res://addons/springbody2d/springicon.png")

func _enter_tree():
	var theme = EditorInterface.get_editor_theme()
	theme.set_icon("SpringBody2D", "EditorIcons", ICON)

func _exit_tree():
	var theme = EditorInterface.get_editor_theme()
	theme.clear_icon("SpringBody2D", "EditorIcons")
