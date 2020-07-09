#pragma once

enum class Object {
	rectangle = 0
};

constexpr int OBJECT_NUM = 1;

class Object_Manager {
public:
	unsigned VAOs[OBJECT_NUM];

	Object_Manager(){}
	void load(Object);
	void draw(Object);
};