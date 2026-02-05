#include "raylib.h"
#include "../include/TracerEngine.hpp"
#include "../include/TracerVisualizer.hpp"
#include "../include/BoundingVolumeHierarchy.h"

#include <iostream>

#define VERTEX_FILE "resources/plan.csv"
#define NUM_THREADS 4
#define PLAYER_SPEED 0.2f

BoundingVolumeHierarchy bvh;
TracerEngine tracerEngine(NUM_THREADS);
TracerVisualizer tracerVisualizer(tracerEngine);
VertexBuffer vB;
LineBuffer lB;
BVHNode* root = nullptr;
SoundListener& player = tracerEngine.listener;
Vec2 movement, cameraMovement;
LineObject* head, * snd, * snd2;

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

void initRaylib() {
	InitWindow(0, 0, "RayCaster");
	InitAudioDevice();
	SetWindowSize((int)(GetScreenWidth() * 0.7f), (int)(GetScreenHeight() * 0.7f));
	SetWindowPosition((int)(GetScreenHeight() * 0.15f), (int)(GetScreenWidth() * 0.15f));
	SetConfigFlags(FLAG_MSAA_4X_HINT);
}

void uninitRaylib() {
	CloseAudioDevice();
	CloseWindow();
}

void processInputs() {
	if (IsKeyPressed(KEY_SPACE)) snd->playSound();
	if (IsKeyPressed(KEY_B)) snd2->playSound();
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
	if (IsKeyDown(KEY_U)) tracerVisualizer.cameraChangeZoom(1.001f);
	if (IsKeyDown(KEY_O)) tracerVisualizer.cameraChangeZoom(1 / 1.001f);
}

void runTracer() {
	initRaylib();

	// Set up engine and visualizer
	tracerEngine.loadMap(VERTEX_FILE);
	tracerVisualizer.updateConfiguration(OBJECTS | DETECTED | BUFFER | STATS);

	// Create sound-sources
	snd = &tracerEngine.lB.lines[tracerEngine.addObject(LineObject(player.getPosition(), 8, "resources/snap.mp3"))];
	snd2 = &tracerEngine.lB.lines[tracerEngine.addObject(LineObject(Vec2{ 400, 400 }, 40, "resources/bottle.mp3"))];
	head = &tracerEngine.lB.lines[tracerEngine.addObject(LineObject(Vec2(player.getPosition().x - 12.0f, player.getPosition().y + 30.0f), Vec2(player.getPosition().x + 12.0f, player.getPosition().y + 30.0f)))];
	while (!WindowShouldClose()) {

		// Reset values
		tracerEngine.clearDetected();
		movement = Vec2(0.0f);
		cameraMovement = Vec2(0.0f);

		processInputs();
		
		// Update positions
		player.setPosition(player.getPosition() + movement);
		head->move(movement);
		snd->move(movement);

		// Use engine and visualizer classes
		tracerEngine.listen();
		tracerVisualizer.moveCamera(cameraMovement.x, cameraMovement.y);
		tracerVisualizer.draw();
	}

	uninitRaylib();
}