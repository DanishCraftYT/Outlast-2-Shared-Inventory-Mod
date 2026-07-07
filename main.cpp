#include <cstddef>
#include <iostream>
#include <chrono>
#include <memory>
#include <thread>
#include <format>
#include <cmath>

#include "ImGUI/imgui.h"
#include "Window/Window.hpp"
#include "Memory/Memory.hpp"

int main() {
    WindowSize windowSize = {400, 350};

    Memory mem("Outlast2.exe", "Outlast2.exe");
    Window window({4, 6, GLFW_OPENGL_CORE_PROFILE}, windowSize, WindowMode::WINDOWED_FALLBACK, "Outlast 2 Shared Inventory Mod!", DISABLED, glfwGetPrimaryMonitor());

    int monitorCount = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
    std::string guiPreviewMonitor = glfwGetMonitorName(glfwGetPrimaryMonitor());

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

    bool instantUseEnabled = false; // WIP.

    const std::string modVersion = "1.0.0"; // contains the current mod version.

    std::string currentDfficultyString = "Normal";

    std::string errMsg = "None.";
    bool recentErr = false;

    while (!window.shouldWindowClose()) {
        // sets the viewport color.
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        recentErr = false;

        window.newFrameImGUI();

        ImGui::SetNextWindowSize(ImVec2(375, 232));
        ImGui::Begin("Outlast 2 Shared Inventory Mod!", __null, ImGuiWindowFlags_NoResize);
        // checks if the Outlast 2 process is running.
        if (!mem.getProcess()) {
            ImGui::Text("Outlast 2 is not running. please open the game.");
        }
        else {
            lastBatteriesValue = batteries;
            // reads batteries value.
            if (!mem.readMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &batteries, sizeof(batteries))) {
                errMsg = "failed to read batteries value.";
                recentErr = true;
            }

            lastBandagesValue = bandages;
            // reads bandages value.
            if (!mem.readMemory(mem.readPointer(0x0219FDB8, { 0x8, 0x154, 0xC88 }), &bandages, sizeof(bandages))) {
                errMsg = "failed to read bandages value.";
                recentErr = true;
            }

            bandageSpaceUsed = bandages * bandageSpace;
            totalSpaceUsed = batteries + bandageSpaceUsed;

            // determines if the total space used by the batteries and bandages exceeds the available space.
            if (totalSpaceUsed > maxInventorySpace) {
                if (lastBatteriesValue != batteries) {
                    if (instantUseEnabled) {
                        // write current battery duration value to 1.
                    }
                    if (!mem.writeMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &lastBatteriesValue, sizeof(lastBatteriesValue))) {
                        errMsg = "failed to write batteries value.";
                        recentErr = true;
                    }
                }
                else if (lastBandagesValue != bandages) {
                    if (instantUseEnabled) {
                        // write current health value to 100.
                    }
                    if (!mem.writeMemory(mem.readPointer(0x0219FDB8, { 0x8, 0x154, 0xC88 }), &lastBandagesValue, sizeof(lastBandagesValue))) {
                        errMsg = "failed to write bandages value.";
                        recentErr = true;
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
                        errMsg = "failed to write batteries value.";
                        recentErr = true;
                    }

                    // bandages.
                    if (!mem.writeMemory(mem.readPointer(0x0219FDB8, { 0x8, 0x154, 0xC88 }), &bandages, sizeof(bandages))) {
                        errMsg = "failed to write bandages value.";
                        recentErr = true;
                    }
                }
            }

            if (ImGui::BeginTabBar("OL2SharedInvModTabs")) {
                // displays inventory infomation.
                if (ImGui::BeginTabItem("Info")) {
                    ImGui::Text(std::format("Inventory Used: {} / {}", totalSpaceUsed, maxInventorySpace).c_str());
                    ImGui::Text(std::format("Batteries: {}", batteries).c_str());
                    ImGui::Text(std::format("Bandages: {} (inventory space used: {})", bandages, bandageSpaceUsed).c_str());
                    ImGui::Checkbox("Instant Use (WIP)", &instantUseEnabled);
                    ImGui::Text("");
                    ImGui::Text(std::format("Error: {}", errMsg).c_str());
                    ImGui::Text("");
                    ImGui::Text("About Mod:");
                    ImGui::Text("Creator: Danish Craft");
                    ImGui::Text(std::format("Version: {}", modVersion).c_str());
                    ImGui::EndTabItem();
                }

                // contains difficulty settings.
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
                    ImGui::Text("Normal: you have 8 Inventory Spaces.\nBandages take up 2 Inventory Spaces.");
                    ImGui::Text("Hard: you have 6 Inventory Spaces.\nBandages take up 2 Inventory Spaces.");
                    ImGui::Text("Custom: automatically sets the difficulty to Custom\nif you modify the max inventory value in the Debug\ntab.");
                    ImGui::EndTabItem();
                }

                // contains debug sliders.
                if (ImGui::BeginTabItem("Debug")) {
                    guiBatteries = batteries;
                    // allows you to adjust the amount of batteries in your inventory.
                    ImGui::Text("Batteries In Inventory:");
                    if (ImGui::SliderInt("##DebugBatteries", &guiBatteries, 0, guiMaxBatteries)) {
                        if (!mem.writeMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &guiBatteries, sizeof(guiBatteries))) {
                            errMsg = "failed to write bandages value.";
                            recentErr = true;
                        }
                    }

                    ImGui::Text("");

                    guiBandages = bandages;
                    // allows you to adjust the amount of bandages in your inventory.
                    ImGui::Text("Bandages In Inventory:");
                    if (ImGui::SliderInt("##DebugBandages", &guiBandages, 0, guiMaxBandages)) {
                        if (!mem.writeMemory(mem.readPointer(0x0219FDB8, { 0x8, 0x154, 0xC88 }), &guiBandages, sizeof(guiBandages))) {
                            errMsg = "failed to write bandages value.";
                            recentErr = true;
                        }
                    }
                    ImGui::EndTabItem();

                    ImGui::Text("");

                    // allows you to adjust the total inventory space.
                    ImGui::Text("Max Inventory Space:");
                    if (ImGui::SliderInt("##DebugMaxInvSpace", &maxInventorySpace, 0, guiMaxInventorySpace)) {
                        currentDfficultyString = "Custom";
                        guiMaxBatteries = maxInventorySpace;
                        guiMaxBandages = std::round(maxInventorySpace / 2);
                    }
                }

                // settings for the mod.
                if (ImGui::BeginTabItem("Settings")) {
                    // determines what monitor the GLFW Window is rendered on.
                    ImGui::Text("change the monitor the mod overlay will render on:");
                    if (ImGui::BeginCombo("List of Monitors", guiPreviewMonitor.c_str())) {
                        int monitorWidth = 0;
                        // loops through all the monitors.
                        for (int i = 0; i < monitorCount; i++) {
                            if (ImGui::Selectable(std::format("{}##{}", glfwGetMonitorName(monitors[i]), i).c_str())) {
                                glfwSetWindowMonitor(window.getWindow(), NULL, monitorWidth, 0, windowSize.width, windowSize.height, glfwGetVideoMode(monitors[i])->refreshRate);
                            }
                            monitorWidth += glfwGetVideoMode(monitors[i])->width;
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        ImGui::End();
        window.renderImGUI();

        // if no error occured during this iteration of the loop. it sets the error message to none.
        if (!recentErr) {
            errMsg = "None.";
        }

        // swaps GLFW window buffers and handle poll events.
        window.swapBuffers();
        window.pollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    // terminates the window.
    window.terminate();
    return 0;
}
