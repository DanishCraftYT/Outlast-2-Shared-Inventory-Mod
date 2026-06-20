#pragma once

#include <iostream>
#include <format>
#include <vector>
#include <map>
#include <chrono>
#include <thread>

#include <process.h>

#include "Process/Process.hpp"

class Memory {
public:
    Memory(std::string processName, std::string moduleName);
    Memory() = default;
    int getProcess();
    int readMemory(const DWORD_PTR& address, LPVOID buffer, SIZE_T bufferSize);
    int writeMemory(const DWORD_PTR& address, LPVOID buffer, SIZE_T bufferSize);
    int readString(const DWORD_PTR& address, char (&buffer)[], SIZE_T bufferSize);
    int writeString(const DWORD_PTR& address, char (&buffer)[], SIZE_T bufferSize);
    int freezeMemory(const std::string threadName, const DWORD_PTR& address, std::string buffer, std::string bufferType);
    int unfreezeMemory(const std::string threadName);
    int isFrozen(const std::string threadName);
    DWORD_PTR readPointer(const DWORD_PTR& address, const std::vector<DWORD_PTR>& offsets);
    Process process;
private:
    struct ThreadMemArgs {
        std::string threadName;
	    DWORD_PTR address;
	    std::string buffer;
	    std::string bufferType;
	    int exitFlag;
	    Memory* mem;
    };
    std::map<HANDLE, ThreadMemArgs*> threads = std::map<HANDLE, ThreadMemArgs*>();
    static unsigned int __stdcall threadMemory(void* data);
};
