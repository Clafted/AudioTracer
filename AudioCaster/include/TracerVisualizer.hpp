#pragma once

#include <raylib.h>
#include "TracerEngine.hpp"

#define FONT_SIZE 30
#define LINE_HEIGHT 40
#define RADIAN_BAR_Y_OFFSET -50
#define BACKGROUND_COLOR Color{5, 10, 15}
#define AXIS_COLOR Color{ 255, 255, 255, 70}
#define GRID_COLOR Color{ 220, 220, 220, 20}
#define SHOW_UNITS true

class TracerVisualizer {

	bool showStats = true;
	bool showDetected = true;
	bool showObjects = true;

public:

	TracerEngine& engine;
	Camera2D camera = { 0,0,0,0 };

	TracerVisualizer(TracerEngine& engine) : engine(engine) {
		camera.zoom = 1.0f;
	}

	void updateConfiguration(bool showStats, bool showDetected, bool showObjects);

	void moveCamera(int x, int y);

	void drawStats();
	void drawDetected();
	void drawObjects();
	void drawFrame();
};