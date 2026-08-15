#include "Plugin.h"

#include <sampapi/CGame.h>
#include <sampapi/CNetGame.h>

char logsPath[300];
FILE* g_fLog = nullptr;

namespace fs = std::filesystem; 

std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::string GetDocumentsFolderPath() {
    PWSTR pszPath = nullptr;
    std::wstring documentsPath = L"";
    if (SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &pszPath) == S_OK)
        documentsPath = pszPath;

    if (pszPath != nullptr)
        CoTaskMemFree(pszPath);
    return WStringToString(documentsPath);
}

bool CreateLogsFolderIfMissing() {
	std::string dirPathStr = GetDocumentsFolderPath() + "\\GTA San Andreas User Files\\SAMP\\Chatlogs";
	fs::path dir_path(dirPathStr);

    if (!fs::exists(dir_path)) {
        if (!fs::create_directory(dir_path))
            return false;
    }
	return true;
}

std::mutex logMutex;
void WriteToLogFile(const char *path, const char *fmt, ...) {
    std::lock_guard<std::mutex> lock(logMutex);

	SYSTEMTIME time;
	va_list ap;

	if (g_fLog == NULL)
	{
		g_fLog = fopen(path, "a");
		if (g_fLog == NULL)
			return;
	}

	GetLocalTime(&time);
	fprintf(g_fLog, "[%02d:%02d:%02d.%03d] ", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);
	va_start(ap, fmt);
	vfprintf(g_fLog, fmt, ap);
	va_end(ap);
	fprintf(g_fLog, "\n");
	fflush(g_fLog);
}

template <typename T>
std::string read_with_size(RakNet::BitStream *bs) {
    T size;
    if (!bs->Read(size))
        return {};

    const size_t availableBytes = bs->GetNumberOfUnreadBits() / 8;
    if (static_cast<size_t>(size) > availableBytes)
        return {};

    std::string str(size, '\0');
    if (!bs->Read(str.data(), static_cast<unsigned int>(size)))
        return {};
    return str;
}

const char* GetLocalPlayerName()
{
    rakhook::samp_ver version = rakhook::samp_version();
	const char* plrName = nullptr;
	switch (version) {
		case rakhook::samp_ver::v037r1:
		{
			SAMPAPI_EXPORT sampapi::v037r1::CNetGame* pNetGame = sampapi::v037r1::RefNetGame();
			if (pNetGame && pNetGame->m_pPools)
			    plrName = pNetGame->m_pPools->m_pPlayer->m_localInfo.m_pObject->m_szName;
			break;
		}
		case rakhook::samp_ver::v037r31:
		{
			SAMPAPI_EXPORT sampapi::v037r3::CNetGame* pNetGame = sampapi::v037r3::RefNetGame();
			if (pNetGame && pNetGame->m_pPools)
			    plrName = pNetGame->m_pPools->m_pPlayer->m_localInfo.m_pObject->m_szName;
			break;
		}
		case rakhook::samp_ver::v037r5:
		{
			SAMPAPI_EXPORT sampapi::v037r5::CNetGame* pNetGame = sampapi::v037r5::RefNetGame();
			if (pNetGame && pNetGame->m_pPools)
			    plrName = pNetGame->m_pPools->m_pPlayer->m_localInfo.m_pObject->m_szName;
			break;
		}
		case rakhook::samp_ver::v03dlr1:
		{
			SAMPAPI_EXPORT sampapi::v03dl::CNetGame* pNetGame = sampapi::v03dl::RefNetGame();
			if (pNetGame && pNetGame->m_pPools)
			    plrName = pNetGame->m_pPools->m_pPlayer->pLocalPlayer->m_szName;
			break;
		}
		default:{
			plrName = nullptr;
			break;
		}
	}
	return plrName;
}

std::string GetServerAddressPort()
{
    rakhook::samp_ver version = rakhook::samp_version();
	std::string srvAddrPrt;
	switch (version) {
		case rakhook::samp_ver::v037r1:
		{
			SAMPAPI_EXPORT sampapi::v037r1::CNetGame* pNetGame = sampapi::v037r1::RefNetGame();
			if (pNetGame)
			    srvAddrPrt = std::string(pNetGame->m_szHostAddress) + ':' + std::to_string(pNetGame->m_nPort);
			break;
		}
		case rakhook::samp_ver::v037r31:
		{
			SAMPAPI_EXPORT sampapi::v037r3::CNetGame* pNetGame = sampapi::v037r3::RefNetGame();
			if (pNetGame)
			    srvAddrPrt = std::string(pNetGame->m_szHostAddress) + ':' + std::to_string(pNetGame->m_nPort);
			break;
		}
		case rakhook::samp_ver::v037r5:
		{
			SAMPAPI_EXPORT sampapi::v037r5::CNetGame* pNetGame = sampapi::v037r5::RefNetGame();
			if (pNetGame)
			    srvAddrPrt = std::string(pNetGame->m_szHostAddress) + ':' + std::to_string(pNetGame->m_nPort);
			break;
		}
		case rakhook::samp_ver::v03dlr1:
		{
			SAMPAPI_EXPORT sampapi::v03dl::CNetGame* pNetGame = sampapi::v03dl::RefNetGame();
			if (pNetGame)
			    srvAddrPrt = std::string(pNetGame->m_szHostAddress) + ':' + std::to_string(pNetGame->m_nPort);
			break;
		}
	}
	return srvAddrPrt;
}

bool IsGameInitialized()
{
    rakhook::samp_ver version = rakhook::samp_version();
	bool _initialized = false;
	switch (version) {
		case rakhook::samp_ver::v037r1:
		{
			_initialized = (sampapi::v037r1::RefGame() != nullptr);
			break;
		}
		case rakhook::samp_ver::v037r31:
		{
			_initialized = (sampapi::v037r3::RefGame() != nullptr);
			break;
		}
		case rakhook::samp_ver::v037r5:
		{
			_initialized = (sampapi::v037r5::RefGame() != nullptr);
			break;
		}
		case rakhook::samp_ver::v03dlr1:
		{
			_initialized = (sampapi::v03dl::RefGame() != nullptr);
			break;
		}
		default:{
			_initialized = false;
			break;
		}
	}
	return _initialized;
}

bool ASIinitialized = false;
bool pluginReady = false;
std::atomic<bool> shuttingDown{false};
bool localPlayerJoined = false;

void InitializeHooks() {
	while (GetModuleHandleA("samp.dll") == nullptr) {
	  if (shuttingDown.load(std::memory_order_relaxed))
		  return;
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	while (!ASIinitialized) {
		if (shuttingDown.load(std::memory_order_relaxed))
			return;

		if (rakhook::samp_addr() && rakhook::samp_version() != rakhook::samp_ver::unknown) {
			if (IsGameInitialized()) {
				if (rakhook::initialize()) {
					ASIinitialized = true;
					break;
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	rakhook::on_send_rpc += [](int &id, RakNet::BitStream *bs, PacketPriority &priority, PacketReliability &reliability, char &ord_channel, bool &sh_timestamp) -> bool {
	  if (id != 101)
		  return true;

	  std::string msg = read_with_size<unsigned char>(bs);

      const char* localName = GetLocalPlayerName();
	  WriteToLogFile(logsPath, "[OUT:%s]-> %s", localName ? localName : "??", msg.c_str());

	  bs->ResetReadPointer();
	  return true;
	};
	rakhook::on_receive_rpc += [](unsigned char &id, RakNet::BitStream *bs) -> bool {
	  if (id != 93)
		  return true;
	  unsigned long color;

	  bs->Read(color);
	  std::string msg = read_with_size<unsigned int>(bs);

	  WriteToLogFile(logsPath, "[IN][color=0x%08X]-> %s", color, msg.c_str());

	  bs->ResetReadPointer();
	  return true;
	};
	rakhook::on_receive_rpc += [](unsigned char &id, RakNet::BitStream *bs) -> bool {
		if (id != 139)
			return true;

        localPlayerJoined = true;
		WriteToLogFile(logsPath, "[JOIN] Connected to %s", GetServerAddressPort().c_str());

		bs->ResetReadPointer();
		return true;
	};
	rakhook::on_receive_packet += [](auto* packet) -> bool {
		unsigned char pid = packet->data[0];

		if (pid == ID_DISCONNECTION_NOTIFICATION || pid == ID_CONNECTION_LOST) {
			if (localPlayerJoined) {
				const char* reason = (pid == ID_DISCONNECTION_NOTIFICATION) ? "graceful" : "timeout";
				WriteToLogFile(logsPath, "[QUIT] Disconnected (%s)", reason);
				localPlayerJoined = false;
			}
		}
		else if (pid == ID_CONNECTION_BANNED || pid == ID_INVALID_PASSWORD || pid == ID_NO_FREE_INCOMING_CONNECTIONS || pid == ID_CONNECTION_ATTEMPT_FAILED) {
			if (!localPlayerJoined)
				WriteToLogFile(logsPath, "[CONNECT_FAILED] reason id=%d", (int)pid);
		}
		return true;
	};
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason) {
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);
			
			if (CreateLogsFolderIfMissing()) {
				SYSTEMTIME current_time;
				GetLocalTime(&current_time);

				snprintf(logsPath, 300, "%s\\GTA San Andreas User Files\\SAMP\\Chatlogs\\%d-%.2d-%.2d %.2d.%.2d.%.2d.log", GetDocumentsFolderPath().c_str(), current_time.wYear, current_time.wMonth, current_time.wDay, current_time.wHour, current_time.wMinute, current_time.wSecond);
				WriteToLogFile(logsPath, "Initialized successfully...");
				pluginReady = true;
			}
			else
				pluginReady = false;

			static bool threadSpawned = false;
			if (pluginReady && !threadSpawned) {
                threadSpawned = true;
			    std::thread(InitializeHooks).detach();
            }
			break;
		}
        case DLL_PROCESS_DETACH: {
			shuttingDown.store(true, std::memory_order_relaxed);

			rakhook::on_receive_rpc.clear();
			rakhook::on_send_rpc.clear();
			rakhook::on_receive_packet.clear();
			rakhook::on_send_packet.clear();

			if (GetModuleHandleA("samp.dll") != nullptr) {
				rakhook::destroy();
			}

            std::lock_guard<std::mutex> lock(logMutex);
            if (g_fLog) {
                fclose(g_fLog);
                g_fLog = nullptr;
            }
            break;
        }
	}
    return true;
}
