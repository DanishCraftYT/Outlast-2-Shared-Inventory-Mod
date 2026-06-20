#include <iostream>
#include <chrono>
#include <thread>

#include "Memory/Memory.hpp"

int main() {
    Memory mem("Outlast2.exe", "Outlast2.exe");
    // waits for Outlast 2 process to open if it's not already open.
    while (true) {
        // breaks out of the loop once the Outlast 2 process has been found.
        if (mem.getProcess()) {
            break;
        }

        std::cout << "Outlast 2 not running. retrying in 1 second." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        system("cls");
    }

    std::cout << "Outlast 2 process found." << std::endl;

    int batteries = 0;
    int bandages = 0;
    int lastBatteriesValue = 0;
    int lastBandagesValue = 0;
    int bandageSpaceUsed = 0;
    int totalSpaceUsed = 0;

    const int maxInventory = 8;
    const int bandageSpace = 2; // bandages takes up 2 batteries of space.

    /* TEST:
    int defaultBatteries = 6;
    mem.writeMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &defaultBatteries, sizeof(defaultBatteries));
    int defaultBandages = 0;
    mem.writeMemory(mem.readPointer(0x020F5220, { 0x10, 0x10, 0x154, 0xC88 }), &defaultBandages, sizeof(defaultBandages));
    */

    while (true) {
        lastBatteriesValue = batteries;
        // reads batteries value.
        if (!mem.readMemory(mem.readPointer(0x0219FF58, { 0x250, 0xD0, 0x10, 0x8, 0xC80 }), &batteries, sizeof(batteries))) {
            std::cout << "failed to read batteries value." << std::endl;
            break;
        }

        lastBandagesValue = bandages;
        // reads bandages value.
        if (!mem.readMemory(mem.readPointer(0x020F5220, { 0x10, 0x10, 0x154, 0xC88 }), &bandages, sizeof(bandages))) {
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
                if (!mem.writeMemory(mem.readPointer(0x020F5220, { 0x10, 0x10, 0x154, 0xC88 }), &lastBandagesValue, sizeof(lastBandagesValue))) {
                    std::cout << "failed to write bandages value." << std::endl;
                    break;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    /* TEST:
    while (true) {
        lastBatteriesValue = batteries;
        std::cin >> batteries;
        lastBandagesValue = bandages;
        std::cin >> bandages;

        bandageSpaceUsed = bandages * bandageSpace;
        totalSpaceUsed = batteries + bandageSpaceUsed;

        // determines if the total space used by the batteries and bandages exceeds the available space.
        if (totalSpaceUsed > maxInventory) {
            if (lastBatteriesValue != batteries) {
                batteries = lastBatteriesValue;
            }
            else if (lastBandagesValue != bandages) {
                bandages = lastBandagesValue;
            }
        }
        std::cout << "batteries: " << batteries << " bandages: " << bandages << std::endl;
    }*/
    return 0;
}
