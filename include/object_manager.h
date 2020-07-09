#pragma once

enum class Object {
	rectangle = 0,
	sphere_normal = 1,
	cube_normal_texture = 2
};

constexpr int OBJECT_NUM = 3;

class Object_Manager {
public:
	unsigned VAOs[OBJECT_NUM];

	Object_Manager(){}
	void load(Object);
	void draw(Object);
};