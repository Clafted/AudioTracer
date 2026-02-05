#pragma once

#include <raylib.h>
#include "TracerEngine.hpp"

#define FONT_SIZE 30
#define LINE_HEIGHT 40
#define RADIAN_BAR_Y_OFFSET -50
#define BACKGROUND_COLOR Color{5, 10, 15}
#define AXIS_COLOR Color{ 255, 255, 255, 70}
#define GRID_COLOR Color{ 220, 220, 220, 20}
#define OBJECTS 0x1
#define DETECTED 0x2
#define BUFFER 0x4
#define STATS 0x8


// Class to visualize the TracerEngine state
class TracerVisualizer {

	TracerEngine& engine;
	Camera2D camera = { 0,0,0,0 };

	bool showDetected = true;
	bool showBuffer = true;
	bool showObjects = true;
	bool showStats = true;

	void drawStats();
	void drawBuffer();
	void drawDetected();
	void drawObjects();

public:

	

	TracerVisualizer(TracerEngine& engine) : engine(engine) {
		camera.zoom = 1.0f;
	}

	/**
	* Modify rendering flags given bitmap configFlags
	* 
	* @param configFlags bitmap of flags to configure rendering	*/
	void updateConfiguration(unsigned int configFlags) {
		showDetected = (configFlags & DETECTED) != 0;
		showBuffer = (configFlags & BUFFER) != 0;
		showObjects = (configFlags & OBJECTS) != 0;
		showStats = (configFlags & STATS) != 0;
	}

	void draw();
	void moveCamera(float x, float y);
	
	inline void cameraChangeZoom(float zoomFactor) {
		camera.zoom *= zoomFactor;
	}
};