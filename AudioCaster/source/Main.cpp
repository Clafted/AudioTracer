#include "raylib.h"
#include "../include/SoundListener.hpp"
#include "../include/VertexBuffer.hpp"
#include "../include/LineBuffer.hpp"
#include "../include/TracerEngine.hpp"
#include "../include/TracerVisualizer.hpp"
#include "../include/BoundingVolumeHierarchy.h"

#include <iostream>
#include <chrono>

#define VERTEX_FILE "resources/plan.csv"
#define NUM_THREADS 4
#define PLAYER_SPEED 0.2f

BoundingVolumeHierarchy bvh;
TracerEngine tEng(NUM_THREADS);
TracerVisualizer tVis(tEng);
VertexBuffer vB;
LineBuffer lB;
BVHNode* root = nullptr;

void runBVH();
void runTracer();

int main() {
	runTracer();
	return 0;
}

void runBVH() {

	vB.loadData(VERTEX_FILE);
	lB.loadData(vB.vertices, vB.endOfVertices);
	
	std::cout << "\nBefore sort:" << std::endl;
	for (int i = 0; i < lB.lineCount; i++) {
		std::cout << (lB.lines[i].start.y + lB.lines[i].end.y) / 2 << ", ";
	}
	bvh.createBVH(lB.lines, lB.lineCount, root, Y);
	std::cout << "\nAfter sort:" << std::endl;
	for (int i = 0; i < lB.lineCount; i++) {
		std::cout << (lB.lines[i].start.y + lB.lines[i].end.y) / 2 << ", ";
	}
}

void runTracer() {
	SoundListener& player = tEng.listener;

	// Initialize Raylib
	InitWindow(0, 0, "RayCaster");
	InitAudioDevice();
	// Configure widow
	int sWidth = (int)(GetScreenWidth() * 0.7f);
	int sHeight = (int)(GetScreenHeight() * 0.7f);
	SetWindowSize(sWidth, sHeight);
	SetWindowPosition((int)(GetScreenHeight() * 0.15f), (int)(GetScreenWidth() * 0.15f));
	SetConfigFlags(FLAG_MSAA_4X_HINT);

	// Set up engine
	tEng.loadMap(VERTEX_FILE);
	// Create sound-sources
	LineObject& snd = tEng.lB.lines[tEng.addObject(LineObject(player.getPosition(), 8, "resources/snap.mp3"))];
	LineObject& snd2 = tEng.lB.lines[tEng.addObject(LineObject(Vec2{ 400, 400 }, 40, "resources/bottle.mp3"))];
	LineObject& head = tEng.lB.lines[tEng.addObject(LineObject(Vec2(player.getPosition().x - 12.0f, player.getPosition().y + 30.0f), Vec2(player.getPosition().x + 12.0f, player.getPosition().y + 30.0f)))];
	auto start = std::chrono::high_resolution_clock::now();

	Vec2 movement, cameraMovement;
	while (!WindowShouldClose()) {

		// Reset values
		tEng.clearDetected();
		movement = Vec2(0.0f);
		cameraMovement = Vec2(0.0f);

		// Process inputs
		if (IsKeyPressed(KEY_SPACE)) snd.playSound();
		if (IsKeyPressed(KEY_B)) snd2.playSound();
		if (IsKeyPressed(KEY_UP)) player.incrementSampleSize(20);
		if (IsKeyPressed(KEY_DOWN)) player.incrementSampleSize(-20);
		if (IsKeyPressed(KEY_LEFT)) player.incrementMaxBounces(-1);
		if (IsKeyPressed(KEY_RIGHT)) player.incrementMaxBounces(1);
		if (IsKeyPressed(KEY_Q)) player.incrementResolution(-20);
		if (IsKeyPressed(KEY_E)) player.incrementResolution(20);
		if (IsKeyDown(KEY_W)) movement.y -= PLAYER_SPEED;
		if (IsKeyDown(KEY_S)) movement.y += PLAYER_SPEED;
		if (IsKeyDown(KEY_A)) movement.x -= PLAYER_SPEED;
		if (IsKeyDown(KEY_D)) movement.x += PLAYER_SPEED;
		if (IsKeyDown(KEY_I)) cameraMovement.y -= PLAYER_SPEED;
		if (IsKeyDown(KEY_K)) cameraMovement.y += PLAYER_SPEED;
		if (IsKeyDown(KEY_J)) cameraMovement.x -= PLAYER_SPEED;
		if (IsKeyDown(KEY_L)) cameraMovement.x += PLAYER_SPEED;
		if (IsKeyDown(KEY_U)) tVis.cameraChangeZoom(1.001f);
		if (IsKeyDown(KEY_O)) tVis.cameraChangeZoom(1/1.001f);
		
		// Update positions
		player.setPosition(player.getPosition() + movement);
		tVis.moveCamera(cameraMovement.x, cameraMovement.y);
		head.move(movement);
		snd.move(movement);

		// Render sound and visuals
		tEng.listen();
		tVis.draw();
	}
	CloseAudioDevice();
	CloseWindow();
}