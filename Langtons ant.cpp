#include "draw.hpp"
#include <chrono>
#include <algorithm>

template <typename T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
	return (v < lo) ? lo : (hi < v) ? hi : v;
}

int main(int, char**) {
	SceneSettings settings;
	settings.scale = 2;
	settings.gridColorF32 = ImVec4(1.0f, 1.0f, 1.0f, 0.125f);

	// Initialize grid
	grid grid(512, 512, 32.0f, settings);

	// Initialize THE ant
	grid::ant ant;
	ant.parent = &grid;
	grid.setAntCell(25, 25, ant);

	// Lambda to render all windows/scenes and pass a single func to startRender
	Scene combinedScene = [&]() {
		//settingsWindow(settings);
		grid.settingsWindow(&ant);

		// Handle mouse scroll zoom
		float& scale = settings.scale;
		float scroll = ImGui::GetIO().MouseWheel;
		if (scroll != 0.0f) {
			scale += scroll * .25; // zoom speed
			scale = clamp(scale, 0.01f, 100.0f);
		}

		// Mouse drag camera
		static bool isDragging = false;
		static ImVec2 lastMousePos;
		ImVec2 mousePos = ImGui::GetIO().MousePos;

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
			if (!isDragging) {
				lastMousePos = mousePos;
				isDragging = true;
			}

			ImVec2 delta = ImVec2(mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y);
			grid.cameraOffset.x += delta.x;
			grid.cameraOffset.y += delta.y;
			lastMousePos = mousePos;
		}
		else {
			isDragging = false;
		}

		static std::chrono::time_point<std::chrono::high_resolution_clock> lastStepTime = std::chrono::high_resolution_clock::now();
		float stepInterval = 1.0f / ant.speed; // seconds per step

		std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> elapsed = now - lastStepTime;

		if (elapsed.count() >= stepInterval) {
			ant.think();
			lastStepTime = now;
		}

		grid.draw(&ant);
		};

	startRender(combinedScene);
	return 0;
}
