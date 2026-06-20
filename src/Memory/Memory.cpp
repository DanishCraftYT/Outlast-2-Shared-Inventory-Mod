#include "Memory.hpp"

Memory::Memory(std::string processName, std::string moduleName) : process(processName, moduleName) {}

int Memory::getProcess() {
    this->process.getProcess();
    if (!this->process.validProcess()) {
        return 0;
    }

    this->process.getBaseAddress();
    if (!this->process.validBaseAddress()) {
        return 0;
    }

    return 1;
}

int Memory::readMemory(const DWORD_PTR& address, LPVOID buffer, SIZE_T bufferSize) {
    if (!ReadProcessMemory(this->process.process, (LPCVOID)address, buffer, bufferSize, NULL)) {
        std::cout << "failed to read address: " << std::hex << address << std::dec << " please verify that the address exists | System Error: " << GetLastError() << std::endl;
        return 0;
    }
    return 1;
}

int Memory::writeMemory(const DWORD_PTR& address, LPVOID buffer, SIZE_T bufferSize) {
    if (!WriteProcessMemory(this->process.process, (LPVOID)address, buffer, bufferSize, NULL)) {
        std::cout << "failed to write to address: " << std::hex << address << std::dec << " please verify that the address exists | System Error: " << GetLastError() << std::endl;
        return 0;
    }
    return 1;
}

int Memory::readString(const DWORD_PTR& address, char (&buffer)[], SIZE_T bufferSize) {
    if (!ReadProcessMemory(this->process.process, (LPVOID)address, &buffer, bufferSize, NULL)) {
        std::cout << "failed to read address: " << std::hex << address << std::dec << " please verify that the address exists | System Error: " << GetLastError() << std::endl;
        return 0;
    }
    return 1;
}

int Memory::writeString(const DWORD_PTR& address, char (&buffer)[], SIZE_T bufferSize) {
    if (!WriteProcessMemory(this->process.process, (LPVOID)address, &buffer, bufferSize, NULL)) {
        std::cout << "failed to write to address: " << std::hex << address << std::dec << " please verify that the address exists | System Error: " << GetLastError() << std::endl;
        return 0;
    }
    return 1;
}

int Memory::freezeMemory(const std::string threadName, const DWORD_PTR& address, std::string buffer, std::string bufferType) {
    for (const auto& [key, value] : this->threads) {
        if (value->threadName == threadName) {
            std::cout << "thread: \"" << threadName << "\" already exists, " << "address: \"" << std::hex << address << std::dec << "\" is already frozen." << std::endl;
            return 0;
        }
    }
    ThreadMemArgs* tma = new ThreadMemArgs{ threadName, address, buffer, bufferType, 0, this };
    // creates thread. SPENT FUCKING HOURS ON THIS SHIT! used "&tma" instead of "tma" which meant that i would need to use some pointer shit to access the struct which i couldn't figure out how to do.
    HANDLE thread = (HANDLE)_beginthreadex(NULL, NULL, &Memory::threadMemory, tma, NULL, NULL);

    if (!thread || thread == INVALID_HANDLE_VALUE) {
        std::cout << "failed to create thread | System Error: " << GetLastError() << std::endl;
        return 0;
    }

    // appends thread to threads map.
    this->threads.insert(std::make_pair(thread, tma));
    return 1;
}

int Memory::unfreezeMemory(const std::string threadName) {
    HANDLE thread = NULL;
    for (const auto& [key, value] : this->threads) {
        if (value->threadName == threadName) {
            this->threads.at(key)->exitFlag = 1;
            this->threads.erase(key);
            CloseHandle(key);
            return 1;
        }
    }
    return 0;
}

int Memory::isFrozen(const std::string threadName) {
    for (const auto& [key, value] : this->threads) {
        if (value->threadName == threadName) {
            return 1;
        }
    }
    return 0;
}

unsigned int __stdcall Memory::threadMemory(void* data) {
    ThreadMemArgs* tma = (ThreadMemArgs*)data;
    int writeResult = -1;
    // i hate that it took me 7+ hours to figure out this was the solution to my problems... i know it isn't a good solution but i have tried literally everything else so i guess this will do for now.
    int ibuffer;
    float fbuffer;
    double dbuffer;
    while (!tma->exitFlag) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (tma->bufferType == "int") {
            ibuffer = std::stoi(tma->buffer);
            writeResult = tma->mem->writeMemory(tma->address, &ibuffer, sizeof(ibuffer));
        }
        else if (tma->bufferType == "float") {
            fbuffer = std::stof(tma->buffer);
            writeResult = tma->mem->writeMemory(tma->address, &fbuffer, sizeof(fbuffer));
        }
        else if (tma->bufferType == "double") {
            dbuffer = std::stod(tma->buffer);
            writeResult = tma->mem->writeMemory(tma->address, &dbuffer, sizeof(dbuffer));
        }
        else {
            writeResult = tma->mem->writeMemory(tma->address, &tma->buffer, sizeof(tma->buffer));
        }
        if (!writeResult) {
            std::cout << "unable to freeze address: " << std::hex << tma->address << std::dec << " | System Error: " << GetLastError() << std::endl;
            return 0;
        }
        continue;
    }
    //delete tma->mem; - throws heap corruption error.
    delete tma;
    return 1;
}

DWORD_PTR Memory::readPointer(const DWORD_PTR& address, const std::vector<DWORD_PTR>& offsets) {
    DWORD_PTR ptrAddress = NULL;

    // adds base address & base address offset together.
    if (!this->readMemory(this->process.baseAddress + address, &ptrAddress, sizeof(ptrAddress))) {
        std::cout << "failed to add base address: " << std::hex << this->process.baseAddress << std::dec << " and address: " << std::hex << address << std::dec << " together | System Error: " << GetLastError() << std::endl;
        return 0;
    }

    // adds offsets to the pointer address.
    for (size_t i = 0; i < offsets.size() - 1; i++) {
        if (!this->readMemory(ptrAddress + offsets.at(i), &ptrAddress, sizeof(ptrAddress))) {
            std::cout << "failed to add offset: " << std::hex << offsets.at(i) << std::dec << " to: " << std::hex << ptrAddress << std::dec << " | System Error: " << GetLastError() << std::endl;
            return 0;
        }
    }

    // get's the address the pointer is pointing to.
    ptrAddress += offsets.at(offsets.size() - 1);

    return ptrAddress;
}
