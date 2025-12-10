#pragma once

#include "LineObject.hpp"
#include "Buffer.hpp"

#define MAX_LINE_COUNT MAX_VERTEX_COUNT/2

class LineBuffer : public Buffer {
public:

	LineObject lines[MAX_LINE_COUNT] = {};
	int lineCount = 0;

	/*
	Load LineObject data from a buffer of Vec2 objects vertices, given the number of vertices vertexCount.
	Returns 0 upon success.
	*/
	int loadData(const Vec2 vertices[MAX_VERTEX_COUNT], int vertexCount) {
		for (int i = 0; i < MAX_LINE_COUNT; i++) lines[i] = LineObject{};
		for (int i = 0; i < vertexCount - 1; i += 2) {
			lines[lineCount] = LineObject(vertices[i], vertices[i + 1]);;
			lineCount++;
		}
		return 0;
	}
};