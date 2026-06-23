#include <iostream>
#include <chrono>
#include <thread>
#include <format>

#include "Window/Window.hpp"
#include "Memory/Memory.hpp"

int main() {
    Memory mem("Outlast2.exe", "Outlast2.exe");
    Window window({4, 6, GLFW_OPENGL_CORE_PROFILE}, {500, 500}, WindowMode::WINDOWED_FALLBACK, "Outlast 2 Shared Inventory Mod!");

    int batteries = 0;
    int bandages = 0;
    int lastBatteriesValue = 0;
    int lastBandagesValue = 0;
    int bandageSpaceUsed = 0;
    int totalSpaceUsed = 0;

    const int maxInventory = 8;
    const int bandageSpace = 2; // bandages takes up 2 batteries of space.

    int guiBatteries = 0;
    int guiBandages = 0;

    while (!window.shouldWindowClose()) {
        // sets the viewport color.
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.newFrameImGUI();

        ImGui::Begin("Outlast 2 Shared Inventory Mod!");
        // checks if the Outlast 2 process is running.
        if (!mem.getProcess()) {
            ImGui::Text("Outlast 2 is not running. please open the game.");
        }
        else {
            lastBatteriesValue = batteries;
            // reads batteries value.
            if (!mem.readMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &batteries, sizeof(batteries))) {
                std::cout << "failed to read batteries value." << std::endl;
                break;
            }

            lastBandagesValue = bandages;
            // reads bandages value.
            if (!mem.readMemory(mem.readPointer(0x0219FDB8, { 0x8, 0x154, 0xC88 }), &bandages, sizeof(bandages))) {
                std::cout << "failed to read bandages value." << std::endl;
                break;
            }

            bandageSpaceUsed = bandages * bandageSpace;
            totalSpaceUsed = batteries + bandageSpaceUsed;

            // determines if the total space used by the batteries and bandages exceeds the available space.
            if (totalSpaceUsed > maxInventory) {
                if (lastBatteriesValue != batteries) {
                    if (!mem.writeMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &lastBatteriesValue, sizeof(lastBatteriesValue))) {
                        std::cout << "failed to write batteries value." << std::endl;
                        break;
                    }
                }
                else if (lastBandagesValue != bandages) {
                    if (!mem.writeMemory(mem.readPointer(0x0219FDB8, { 0x8, 0x154, 0xC88 }), &lastBandagesValue, sizeof(lastBandagesValue))) {
                        std::cout << "failed to write bandages value." << std::endl;
                        break;
                    }
                }
            }

            ImGui::Text(std::format("Inventory Used: {} / 8", totalSpaceUsed).c_str());
            ImGui::Text(std::format("Batteries: {}", batteries).c_str());
            ImGui::Text(std::format("Bandages: {} (space used: {})", bandages, bandageSpaceUsed).c_str());
            // debug options.
            if (ImGui::CollapsingHeader("DEBUG")) {
                guiBatteries = batteries;
                // batteries.
                if (ImGui::SliderInt("Batteries In Inventory", &guiBatteries, 0, 8)) {
                    if (!mem.writeMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &guiBatteries, sizeof(guiBatteries))) {
                        std::cout << "failed to write bandages value." << std::endl;
                        break;
                    }
                }

                guiBandages = bandages;
                // bandages.
                if (ImGui::SliderInt("Bandages In Inventory", &guiBandages, 0, 4)) {
                    if (!mem.writeMemory(mem.readPointer(0x0219FDB8, { 0x8, 0x154, 0xC88 }), &guiBandages, sizeof(guiBandages))) {
                        std::cout << "failed to write bandages value." << std::endl;
                        break;
                    }
                }
            }
        }
        ImGui::End();
        window.renderImGUI();

        // swaps GLFW window buffers and handle poll events.
        window.swapBuffers();
        window.pollEvents();
    }

    // terminates the window.
    window.terminate();
    return 0;
}
