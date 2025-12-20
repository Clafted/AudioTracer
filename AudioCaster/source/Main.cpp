#include "raylib.h"
#include "../include/SoundListener.hpp"
#include "../include/VertexBuffer.hpp"
#include "../include/LineBuffer.hpp"
#include "../include/TracerEngine.hpp"
#include "../include/TracerVisualizer.hpp"
#include "../include/BoundingVolumeHierarchy.h"
#include <thread>
#define VERTEX_FILE "resources/plan.csv"

#include <iostream>

TracerEngine tEng(8);
TracerVisualizer tVis(tEng);

void runBVH();
void runTracer();
/*


RECORD THIS LOOOOOOOOOOOL

*/
int main(){
	runTracer();
	return 0;
}

VertexBuffer vB;
LineBuffer lB;
BoundingVolumeHierarchy bvh;
BVHNode* root = nullptr;

void runBVH() {

	vB.loadData(VERTEX_FILE);
	lB.loadData(vB.vertices, vB.endOfVertices);
	
	std::cout << "\Before sort:" << std::endl;
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
	int sWidth = (int)(GetScreenWidth() * 0.7f);
	int sHeight = (int)(GetScreenHeight() * 0.7f);
	SetWindowSize(sWidth, sHeight);
	SetWindowPosition((int)(GetScreenHeight() * 0.15f), (int)(GetScreenWidth() * 0.15f));
	InitAudioDevice();

	tEng.loadMap(VERTEX_FILE);

	// Create sound-sources
	LineObject& snd = tEng.lB.lines[tEng.addObject(LineObject(Vec2{ 400, 400 }, 8, "resources/snap.mp3"))];
	LineObject& snd2 = tEng.lB.lines[tEng.addObject(LineObject(Vec2{ 400, 400 }, 40, "resources/bottle.mp3"))];
	LineObject& head = tEng.lB.lines[tEng.addObject(LineObject(Vec2(player.getPosition().x - 12.0f, player.getPosition().y + 30.0f), Vec2(player.getPosition().x + 12.0f, player.getPosition().y + 30.0f)))];

	while (!WindowShouldClose())
	{
		head.move((Vec2)GetMousePosition() - player.getPosition());
		player.setPosition(GetMousePosition());
		snd.start = player.getPosition();

		// Process inputs
		if (IsKeyPressed(KEY_SPACE)) snd.playSound();
		if (IsKeyPressed(KEY_B)) snd2.playSound();
		if (IsKeyPressed(KEY_UP)) player.incrementSampleSize(20);
		if (IsKeyPressed(KEY_DOWN)) player.incrementSampleSize(-20);
		if (IsKeyPressed(KEY_LEFT)) player.incrementMaxBounces(-1);
		if (IsKeyPressed(KEY_RIGHT)) player.incrementMaxBounces(1);
		if (IsKeyPressed(KEY_Q)) player.incrementResolution(-20);
		if (IsKeyPressed(KEY_E)) player.incrementResolution(20);
		if (IsKeyDown(KEY_W)) tVis.moveCamera(0, -1);
		if (IsKeyDown(KEY_S)) tVis.moveCamera(0, 1);
		if (IsKeyDown(KEY_A)) tVis.moveCamera(-1, 0);
		if (IsKeyDown(KEY_D)) tVis.moveCamera(1, 0);

		// Render sound and visuals
		tEng.listen();
		tVis.drawFrame();
		tEng.clearDetected();
	}

	CloseAudioDevice();
	CloseWindow();
}