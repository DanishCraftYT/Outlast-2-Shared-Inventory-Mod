#pragma once

#include <iostream>
#include <string>

#include <windows.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <tchar.h>

class Process {
public:
    Process(const std::string processName, const std::string moduleName);
    Process() = default;
    void getProcess();
    bool validProcess();
    void getBaseAddress();
    bool validBaseAddress();
    int processID;
    HANDLE process;
    DWORD_PTR baseAddress;
private:
    std::string processName;
    std::string moduleName;
};
