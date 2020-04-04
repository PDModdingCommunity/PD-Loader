#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#include <detours.h>
#include <thread>
#include <string>
#include "framework.h"
#include "PluginConfigApi.h"
#include <iostream>

bool(__cdecl* DSC_EXECUTE_COMMAND)(__int64 dsc_address, float delta_time, __int64 start_time, void* wait_timer, int* a5, int a6, int a7)
= (bool(__cdecl*)(__int64 dsc_address, float delta_time, __int64 start_time, void* wait_timer, int* a5, int a6, int a7))
0x14011CBA0;
bool hookedDSC_EXECUTE_COMMAND(__int64 dsc_address, float delta_time, __int64 start_time, void* wait_timer, int* a5, int a6, int a7);
void serve();

bool debug = false;
unsigned short port = 8139;

void loadConfig()
{
	port = GetPrivateProfileIntW(L"general", L"port", 8139, CONFIG_FILE);
	debug = GetPrivateProfileIntW(L"general", L"debug", 0, CONFIG_FILE) > 0;

	return;
}

float last_delta_time = 0.0;
__int64 last_start_time = 0i64;
void* last_wait_timer = nullptr;
int* last_a5 = nullptr;
int last_a6 = 0;
int last_a7 = 0;

bool hookedDSC_EXECUTE_COMMAND(__int64 dsc_address, float delta_time, __int64 start_time, void* wait_timer, int* a5, int a6, int a7)
{
	last_delta_time = delta_time;
	last_start_time = start_time;
	last_wait_timer = wait_timer;
	last_a5 = a5;
	last_a6 = a6;
	last_a7 = a7;

	return DSC_EXECUTE_COMMAND(dsc_address, delta_time, start_time, wait_timer, a5, a6, a7);
}

void serve()
{
	using namespace std;

	cout << "[DSCRemote] Staring server..." << endl;

	WSADATA wsadata;
	int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (result) // error
	{
		cout << "[DSCRemote] E: WSAStartup error " << result << '.' << endl;
		return;
	}

	SOCKET sock;
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (result == INVALID_SOCKET)
	{
		cout << "[DSCRemote] E: Invalid socket." << endl;
		WSACleanup();
		return;
	}

	sockaddr_in saddrin;
	saddrin.sin_family = AF_INET;
	saddrin.sin_addr.s_addr = 0;
	saddrin.sin_port = htons(port);

	result = ::bind(sock, (sockaddr*)&saddrin, sizeof(sockaddr_in));
	if (result) // error
	{
		cout << "[DSCRemote] E: Couldn't bind port " << port << '.' << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	result = listen(sock, 1);
	if (result) // error
	{
		cout << "[DSCRemote] E: Couldn't listen." << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	while (true)
	{
		cout << "[DSCRemote] Waiting for client..." << endl;
		SOCKET client_sock;
		client_sock = accept(sock, NULL, NULL);
		cout << "[DSCRemote] Client accepted." << endl;

		char buff_recv[64];
		vector<int> cmdStack;
		do
		{
			result = recv(client_sock, buff_recv, sizeof(buff_recv), 0);
			if (result > 0)
			{
				if (debug) cout << "[DSCRemote] D: Received: " << buff_recv << endl;
				if (buff_recv[0] != 'i')
				{					
					long stack = stol(buff_recv);
					for (int i = 0; i < sizeof(buff_recv); i++) buff_recv[i] = '\0';
					cmdStack.push_back(stack);
					const char buff_out[] = "Stacking command/parameter. Send 'i' to inject.";
					send(client_sock, buff_out, sizeof(buff_out), 0);
					continue;
				}
				for (int i = 0; i < sizeof(buff_recv); i++) buff_recv[i] = '\0';

				const char buff_out[] = "Injecting stack.";
				send(client_sock, buff_out, sizeof(buff_out), 0);

				const int64_t dscbaseaddr = 0x140CDD978i64;
				int* currentpos = (int*)(dscbaseaddr + 0x2bf2c);
				int* command = (int*)(dscbaseaddr + 0xc);

				int posbk = *currentpos;
				vector<int> cmdbk;
				for (int i=0; i<cmdStack.size(); i++)
					cmdbk.push_back(command[i]);

				*currentpos = 0;

				for (int i = 0; i < cmdStack.size(); i++)
				{
					if (debug) cout << "[DSCRemote] D: Injecting " << cmdStack.at(i) << endl;
					command[i] = cmdStack.at(i);
				}

				cmdStack.clear();

				DSC_EXECUTE_COMMAND(dscbaseaddr, last_delta_time, last_start_time, last_wait_timer, last_a5, last_a6, last_a7);

				for (int i = 0; i < cmdbk.size(); i++)
				{
					if (debug) cout << "[DSCRemote] D: Restoring " << cmdbk.at(i) << endl;
					command[i] = cmdbk.at(i);
				}
				
				cmdbk.clear();
				*currentpos = posbk;
			}
		} while (result > 0);
		closesocket(client_sock);
		cout << "[DSCRemote] Restarting..." << endl;
	}

	cout << "[DSCRemote] Bye!" << endl;
	closesocket(sock);
	WSACleanup();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{

	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		using namespace std;

		loadConfig();
		DisableThreadLibraryCalls(hModule);
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		cout << "[DSCRemote] Hooking functions..." << endl;
		DetourAttach(&(PVOID&)DSC_EXECUTE_COMMAND, (PVOID)hookedDSC_EXECUTE_COMMAND);
		cout << "[DSCRemote] Functions hooked." << endl;
		DetourTransactionCommit();

		thread* server_thread = new thread(serve);
	}

	return TRUE;
}

PluginConfig::PluginConfigOption config[] = {
	{ PluginConfig::CONFIG_NUMERIC, new PluginConfig::PluginConfigNumericData{ L"port", L"general", CONFIG_FILE, L"Port", L"TCP/IP port.", 8139, 1, USHRT_MAX } },
	{ PluginConfig::CONFIG_BOOLEAN, new PluginConfig::PluginConfigBooleanData{ L"debug", L"general", CONFIG_FILE, L"Debug", L"Print extra information.", false } },
};

extern "C" __declspec(dllexport) LPCWSTR GetPluginName(void)
{
	return L"DSCRemote";
}

extern "C" __declspec(dllexport) LPCWSTR GetPluginDescription(void)
{
	return L"Allows external applications to send DSC commands via TCP/IP.";
}

extern "C" __declspec(dllexport) PluginConfig::PluginConfigArray GetPluginOptions(void)
{
	return PluginConfig::PluginConfigArray{ _countof(config), config };
}
