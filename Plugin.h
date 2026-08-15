#ifndef PLUGIN_H
#define PLUGIN_H

#include <windows.h>
#include <iostream>
#include <filesystem>

#include <shlobj.h>
#include <combaseapi.h>

#include <sampapi/sampapi.h>

#include <RakNet/PacketEnumerations.h>
#include <RakNet/StringCompressor.h>
#include <RakNet/BitStream.h>
#include <RakHook/rakhook.hpp>
#include <RakHook/samp.hpp>

#include <map>
#include <memory>
#include <thread>
#include <chrono>
#include <cstring>
#include <string>
#include <filesystem>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <vector>

extern bool ASIinitialized;

std::string WStringToString(const std::wstring& wstr);
std::string GetDocumentsFolderPath();
bool CreateLogsFolderIfMissing();
void WriteToLogFile(const char *path, const char *fmt, ...);
bool SleepWithInterrupt(std::chrono::milliseconds ms);

const char* GetLocalPlayerName();
std::string GetServerAddressPort();
bool IsGameInitialized();

void InitializeHooks();

#endif