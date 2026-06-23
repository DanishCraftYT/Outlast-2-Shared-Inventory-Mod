#include <iostream>
#include <chrono>
#include <thread>
#include <format>
#include <cmath>

#include "Window/Window.hpp"
#include "Memory/Memory.hpp"

int main() {
    Memory mem("Outlast2.exe", "Outlast2.exe");
    Window window({4, 6, GLFW_OPENGL_CORE_PROFILE}, {300, 300}, WindowMode::WINDOWED_FALLBACK, "Outlast 2 Shared Inventory Mod!");

    int batteries = 0;
    int lastBatteriesValue = 0;

    int bandages = 0;
    int lastBandagesValue = 0;
    int bandageSpaceUsed = 0;
    const int bandageSpace = 2; // bandages takes up 2 batteries of space.

    int totalSpaceUsed = 0;
    int maxInventorySpace = 8;

    int guiBatteries = 0;
    int guiMaxBatteries = 8;

    int guiBandages = 0;
    int guiMaxBandages = 4;

    const int guiMaxInventorySpace = 8; // maximum possible inventory space.

    std::string currentDfficultyString = "Normal";

    while (!window.shouldWindowClose()) {
        // sets the viewport color.
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.newFrameImGUI();

        ImGui::SetNextWindowSize(ImVec2(283, 231));
        ImGui::Begin("Outlast 2 Shared Inventory Mod!", __null, ImGuiWindowFlags_NoResize);
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
            if (totalSpaceUsed > maxInventorySpace) {
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
                else {
                    while (totalSpaceUsed > maxInventorySpace) {
                        if (batteries != 0) {
                            batteries--;
                        }
                        else if (bandages != 0) {
                            bandages--;
                            bandageSpaceUsed = bandages * bandageSpace;
                        }
                        else {
                            break;
                        }
                        totalSpaceUsed = batteries + bandageSpaceUsed;
                    }

                    // batteries.
                    if (!mem.writeMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &batteries, sizeof(batteries))) {
                        std::cout << "failed to write batteries value." << std::endl;
                        break;
                    }

                    // bandages.
                    if (!mem.writeMemory(mem.readPointer(0x0219FDB8, { 0x8, 0x154, 0xC88 }), &bandages, sizeof(bandages))) {
                        std::cout << "failed to write bandages value." << std::endl;
                        break;
                    }
                }
            }

            if (ImGui::BeginTabBar("OL2SharedInvModTabs")) {
                if (ImGui::BeginTabItem("Infomation")) {
                    ImGui::Text(std::format("Inventory Used: {} / {}", totalSpaceUsed, maxInventorySpace).c_str());
                    ImGui::Text(std::format("Batteries: {}", batteries).c_str());
                    ImGui::Text(std::format("Bandages: {} (inventory space used: {})", bandages, bandageSpaceUsed).c_str());
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Difficulty")) {
                    ImGui::Text("Current Diffculty:");
                    if (ImGui::BeginCombo("##Diffculty", currentDfficultyString.c_str())) {
                        if (ImGui::Selectable("Normal")) {
                            maxInventorySpace = 8;
                            currentDfficultyString = "Normal";

                            guiMaxBatteries = 8;
                            guiMaxBandages = 4;
                        }
                        if (ImGui::Selectable("Hard")) {
                            maxInventorySpace = 6;
                            currentDfficultyString = "Hard";

                            guiMaxBatteries = 6;
                            guiMaxBandages = 3;
                        }
                        ImGui::EndCombo();
                    }

                    ImGui::Text("");

                    ImGui::Text("Difficulty Infomation:");
                    ImGui::Text("Normal: you have 8 Inventory Spaces\nBandages take up 2 Inventory Spaces.");
                    ImGui::Text("Hard: you have 6 Inventory Spaces\nBandages take up 2 Inventory Spaces.");
                    ImGui::Text("Custom: automatically sets the\ndifficulty to Custom if you modify the\nmax inventory value in the Debug tab.");
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Debug")) {
                    guiBatteries = batteries;
                    // batteries.
                    ImGui::Text("Batteries In Inventory:");
                    if (ImGui::SliderInt("##DebugBatteries", &guiBatteries, 0, guiMaxBatteries)) {
                        if (!mem.writeMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &guiBatteries, sizeof(guiBatteries))) {
                            std::cout << "failed to write bandages value." << std::endl;
                            break;
                        }
                    }

                    ImGui::Text("");

                    guiBandages = bandages;
                    // bandages.
                    ImGui::Text("Bandages In Inventory:");
                    if (ImGui::SliderInt("##DebugBandages", &guiBandages, 0, guiMaxBandages)) {
                        if (!mem.writeMemory(mem.readPointer(0x0219FDB8, { 0x8, 0x154, 0xC88 }), &guiBandages, sizeof(guiBandages))) {
                            std::cout << "failed to write bandages value." << std::endl;
                            break;
                        }
                    }
                    ImGui::EndTabItem();

                    ImGui::Text("");

                    ImGui::Text("Max Inventory Space:");
                    if (ImGui::SliderInt("##DebugMaxInvSpace", &maxInventorySpace, 0, guiMaxInventorySpace)) {
                        currentDfficultyString = "Custom";
                        guiMaxBatteries = maxInventorySpace;
                        guiMaxBandages = std::round(maxInventorySpace / 2);
                    }
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
        window.renderImGUI();

        // swaps GLFW window buffers and handle poll events.
        window.swapBuffers();
        window.pollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // terminates the window.
    window.terminate();
    return 0;
}
