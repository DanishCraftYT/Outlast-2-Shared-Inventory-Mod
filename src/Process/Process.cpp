#include "Process.hpp"

Process::Process(const std::string processName, const std::string moduleName) : processID(0), process(NULL), baseAddress(0), processName(processName), moduleName(moduleName) {}

void Process::getProcess() {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    // creates a snapshot of all the running processes.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    // checks if the snapshot is valid.
    if (!snapshot || snapshot == INVALID_HANDLE_VALUE) {
        std::cout << "snapshot has a invalid handle value | System Error: " << GetLastError() << std::endl;
        return;
    }

    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (_stricmp(entry.szExeFile, this->processName.c_str()) == 0) {
                // get's process.
                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);

                // checks if the process is valid.
                if (!hProcess || hProcess == INVALID_HANDLE_VALUE) {
                    std::cout << "process has a invalid handle value | System Error: " << GetLastError() << std::endl;
                    CloseHandle(snapshot);
                    return;
                }

                CloseHandle(snapshot);
                this->process = hProcess;
                // get's process id.
                this->processID = GetProcessId(this->process);
                return;
            }
        }
    }

    CloseHandle(snapshot);
    std::cout << "failed to find process: " << processName << std::endl;
}

bool Process::validProcess() {
    return this->process;
}

void Process::getBaseAddress() {
    _TCHAR szProcessName[MAX_PATH] = _TEXT("<unknown>");

    HMODULE hMod;
    DWORD cbNeeded;

    if (EnumProcessModulesEx(this->process, &hMod, sizeof(hMod), &cbNeeded, LIST_MODULES_32BIT | LIST_MODULES_64BIT)) {
        GetModuleBaseName(this->process, hMod, szProcessName, sizeof(szProcessName) / sizeof(_TCHAR));
        if (!_tcsicmp(this->moduleName.c_str(), szProcessName)) {
            // converts base address to DWORD_PTR.
            this->baseAddress = (DWORD_PTR)hMod;
            return;
        }
    }
    std::cout << "failed to get the base address of: " << this->moduleName << std::endl;
}

bool Process::validBaseAddress() {
    return this->baseAddress;
}
