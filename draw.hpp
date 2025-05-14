#ifndef DRAW
#define DRAW

#include <functional>
#include "runner.hpp"
#include <vector>

// Type alias for a scene rendering function
using Scene = std::function<void()>;

struct SceneSettings {
    float scale = 10;
    //int spacing = 8;
    ImVec4 gridColorF32 = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

    ImU32 getGridColorU32() const {
        return ImGui::ColorConvertFloat4ToU32(gridColorF32);
    }
};

struct cell {
    ImVec4 color;
	bool isWhite = false; // default is a black square
};

class grid {
public:

    grid(int rows, int cols, float cellSize, SceneSettings& settings) :
        rows(rows), cols(cols), cellSize(cellSize), cells(rows * cols), settings(settings) {
    }

    ImVec2 cameraOffset = ImVec2(0.0f, 0.0f); // pixels

    struct ant {
        int row = 0;
        int col = 0;
        float angle = 0.0f;
        ImVec4 color = ImVec4(1, 0, 0, 1);
        grid* parent = nullptr;
        float speed = 10.0f; // steps/second

        cell& getCurCell() {
            return parent->cells[row * parent->cols + col];
        }
        
        void think() {
            cell& c = getCurCell();

            // Turn and flip color
            if (c.isWhite) {
                angle += 90.0f;
                if (angle >= 360.0f) angle -= 360.0f;
                c.isWhite = false;
            }
            else {
                angle -= 90.0f;
                if (angle < 0.0f) angle += 360.0f;
                c.isWhite = true;
            }

            // Update the cell color after flip
            c.color = c.isWhite ? ImVec4(1, 1, 1, 1) : ImVec4(0, 0, 0, 1);

            // Move
        
            // Move ant
            if (angle == 0.0f)
                row--;
            else if (angle == 90.0f)
                col++;
            else if (angle == 180.0f)
                row++;
            else if (angle == 270.0f)
                col--;
        }
    };

    void setCellColor(int row, int col, const ImVec4& color) {
        if (isValid(row, col))
            cells[row * cols + col].color = color;
    }

	void setAntCell(int row, int col, ant &ant) {
        if (isValid(row, col)) {
			ant.col = col;
			ant.row = row;
        }
		
        setCellColor(row, col, ImColor(ant.color));
	}

    void draw(const ant* a = nullptr) const {
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        ImVec2 origin = cameraOffset;
        float s = settings.scale * cellSize;

        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < cols; ++x) {
				ImVec2 topLeft = ImVec2(origin.x + x * (settings.scale * cellSize), origin.y + y * (settings.scale * cellSize));
				ImVec2 bottomRight = ImVec2(topLeft.x + (settings.scale * cellSize), topLeft.y + (settings.scale * cellSize));

                const ImVec4& color = cells[y * cols + x].color;
                drawList->AddRectFilled(topLeft, bottomRight, ImGui::ColorConvertFloat4ToU32(color));
                drawList->AddRect(topLeft, bottomRight, ImColor(settings.gridColorF32)); // outline
            }
        }

        // Draw ant on top
        if (a && isValid(a->row, a->col)) {
            // get top most drawlist
			drawList = ImGui::GetForegroundDrawList();
            ImVec2 topLeft = ImVec2(origin.x + a->col * s, origin.y + a->row * s);
            ImVec2 bottomRight = ImVec2(topLeft.x + s, topLeft.y + s);
            drawList->AddRectFilled(topLeft, bottomRight, ImGui::ColorConvertFloat4ToU32(a->color), 2.0f); // ant square with rounding
        }
    }

    void settingsWindow(ant* a = nullptr) {
        ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);
        ImGui::SliderFloat("Scale", &settings.scale, 0.01f, 5);
        //ImGui::SliderInt("Spacing", &settings.spacing, 1, 100);
        ImGui::ColorEdit4("Grid Color", (float*)&settings.gridColorF32);
        ImGui::SliderFloat("Ant Speed: %.1f steps/sec", &a->speed, 1.0f, 1000.0f, "%.1f");
        ImGui::End();
    }

private:
    int rows, cols;
    float cellSize;
    std::vector<cell> cells;
	SceneSettings &settings;

    bool isValid(int row, int col) const {
        return row >= 0 && row < rows && col >= 0 && col < cols;
    }
};

// Call the actual render function
void startRender(const Scene& scene) {
    render(scene);
}

#endif // !DRAW
