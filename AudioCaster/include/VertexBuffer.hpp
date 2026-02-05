#pragma once

#include "raylib.h"
#include "Buffer.hpp"
#include "Vec2.h"
#include <fstream>
#include <string>

#define MAX_FILE_LENGTH 10000

// Buffer to store Vec2
struct VertexBuffer : public Buffer {

	Vec2 vertices[MAX_VERTEX_COUNT] = {}; // The vertex data of a loaded file
	int endOfVertices = 0; // 1 + index of the last element in vertices

	void parseStringToFloatArray(char str[MAX_FILE_LENGTH], float dest[MAX_VERTEX_COUNT], int& floatCount);
	void parseCoordsToVector2(float coords[MAX_VERTEX_COUNT], int numCoords);

	/**
	* Loads Vec2 objects from the file stored in the path file.
	* 
	* @param file absolute path to the file to be loaded
	* @returns 0 upon success, -1 otherwise	*/
	int loadData(const char* file);
};