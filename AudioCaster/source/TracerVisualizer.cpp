#pragma once

#include <raylib.h>

#include "../include/TracerVisualizer.hpp"

void TracerVisualizer::moveCamera(float x, float y) {
	camera.offset.x += x;
	camera.offset.y += y;
}

void TracerVisualizer::drawBuffer() {
	while (engine.drawBuffer.hasNextCall()) {
		DrawCall call = engine.drawBuffer.getNextCall();
		DrawLine(call.getStart().x, call.getStart().y,
				call.getEnd().x, call.getEnd().y,
				call.getColor());
	}
}

void TracerVisualizer::drawStats() {
	std::string t;
	SoundListener& l = engine.listener;
	t = "FPS: " + std::to_string((int)engine.getFPS());
	DrawText(t.c_str(), 10, 10, FONT_SIZE, WHITE);
	DrawText(("Resolution [Q/E]: " + std::to_string(l.getResolution())).c_str(),
		10, 10 + 1 * LINE_HEIGHT, FONT_SIZE, WHITE);
	DrawText(("Sample size [UP/DOWN]: " + std::to_string((int)l.getSampleSize())).c_str(),
		10, 10 + 2 * LINE_HEIGHT, FONT_SIZE, WHITE);
	DrawText(("SPP (res/ss): " + std::to_string((int)(l.getSampleSize() / l.getResolution()))).c_str(),
		10, 10 + 3 * LINE_HEIGHT, FONT_SIZE, WHITE);
	DrawText(("Num bounces [LEFT/RIGHT]: " + std::to_string((int)l.getMaxBounces())).c_str(),
		10, 10 + 4 * LINE_HEIGHT, FONT_SIZE, WHITE);
	DrawText(("Num rays: " + std::to_string(l.getNumRays())).c_str(),
		10, 10 + 5 * LINE_HEIGHT, FONT_SIZE, WHITE);
	
}

void TracerVisualizer::drawDetected()
{
	float volume;
	float renderHeight;
	int resolution = engine.listener.getResolution();
	int barWidth = GetScreenWidth() / resolution;
	int screenHeight = GetScreenHeight();
	int yTemp;
	std::string text;
	
	// VOLUME AXIS
	for (int i = 1; i <= 10; i++) {
		yTemp = screenHeight + RADIAN_BAR_Y_OFFSET - screenHeight * 0.9 * i / 10;
		text = std::to_string(i * 10) + "%";
		DrawLine(0, yTemp, GetScreenWidth(), yTemp, AXIS_COLOR);
		DrawText(text.c_str(), 20, yTemp - LINE_HEIGHT + 10, FONT_SIZE, AXIS_COLOR);
	}
	yTemp = screenHeight + RADIAN_BAR_Y_OFFSET;
	
	// HISTOGRAM
	for (int i = 0; i < resolution; i++) {
		volume = engine.listener.getDetectedPairs()[i].second.volume;
		if (volume <= 0.0) continue;
		renderHeight = screenHeight * volume * 0.9;
		DrawRectangle(i * barWidth,
			yTemp - renderHeight,
			barWidth,
			renderHeight,
			PINK);
	}
	
	// RADIAN AXIS
	DrawLine(0, yTemp, GetScreenWidth(), yTemp, AXIS_COLOR);
	for (int i = 0; i < 10; i++) {
		text = std::to_string(i * 0.6283).substr(0, 4) + "rad";
		DrawText(text.c_str(),
			30 + i * GetScreenWidth() / 10,
			yTemp - LINE_HEIGHT,
			FONT_SIZE, AXIS_COLOR);
	}
}

void TracerVisualizer::drawObjects() {
	int sWidth = GetScreenWidth();
	int sHeight = GetScreenHeight();
	for (LineObject l : engine.lB.lines) {
		if (l.type == WALL) {
			DrawLine(l.start.x,l.start.y,
				l.end.x,l.end.y,
				Color{ (unsigned char)(abs(50*l.start.x/sWidth) + 205), 
						(unsigned char)(abs(50*l.start.y/sHeight) + 205), 
						200,
						(unsigned char)(255 * (1.0f - l.absorption))
				});
		}
		else {
			DrawCircle(l.start.x, l.start.y, l.radius, YELLOW);
			for (int j = 0; j < l.numActive; j++) {
				if (l.activeSounds[j].first.volume <= 0.0f) continue;
				DrawCircleLines(l.start.x,
					l.start.y,
					SOUND_SPEED * ((float)GetTime() - l.activeSounds[j].second),
					YELLOW);
			}
		}
	}
	DrawCircle((int)engine.listener.getPosition().x, (int)engine.listener.getPosition().y, 10, GREEN);
}

void TracerVisualizer::draw() {
	BeginDrawing();
		ClearBackground(BACKGROUND_COLOR);
		BeginMode2D(camera);
			if (showBuffer) drawBuffer();
			if (showObjects) drawObjects();
		EndMode2D();
		if (showStats) drawStats();
		if (showDetected) drawDetected();
	EndDrawing();
}
