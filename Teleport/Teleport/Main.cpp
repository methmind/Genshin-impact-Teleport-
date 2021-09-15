#include <Windows.h>
#include <stdio.h>
#include <tlhelp32.h>
#include "Strings.h"

#define LOCS_PATH L"C:\\Locs\\"
#define LOCS_PATH2 L"C:\\Locs\\*"

struct GameInfo
{
	HANDLE hProc;
	DWORD dwPid;
	DWORD64 dwUnityPlayer;
	DWORD64 dwUserAssembly;
};

struct GameVector
{
	float x, z, y;

	void operator =(GameVector vSome)
	{
		this->x = vSome.x; 
		this->y = vSome.y; 
		this->z = vSome.z;
	}

	GameVector operator +(GameVector &vSome)
	{
		return { this->x + vSome.x, vSome.z, this->y + vSome.y };
	}

	GameVector operator -(GameVector& vSome)
	{
		return { this->x - vSome.x, this->z, this->y - vSome.y };
	}
};

enum Offset
{
	m_vecSpawn = 0x1CF9D70,

	//

	m_dwRelPointer = 0x0B1AAB68,
	m_dwRelPointer2 = 0xA0,
	m_dwRelPointer3 = 0xC88,
	m_dwRelPointer4 = 0x2D0,
	m_dwRelPointer5 = 0x10,
	m_dwRelPointer6 = 0x68,
	//
	m_vecRelative = 0xA0,
	m_vecRelative2 = 0x140,
	//
	m_vecVelocity = 0xD0,
	m_vecVelocity2 = 0x14C,

	//

	m_dwLsPointer = 0x0B137C38,
	m_dwLsPointer2 = 0xA0,
	m_bLsStatus = 0x488,
};

void _cls()
{
	COORD topLeft = { 0, 0 };
	CONSOLE_SCREEN_BUFFER_INFO screen = { 0 };
	HANDLE hConsole = 0;
	DWORD dwWritten = 0;

	do
	{
		if ((hConsole = GetStdHandle(STD_OUTPUT_HANDLE)) == NULL)
			break;

		if (!GetConsoleScreenBufferInfo(hConsole, &screen))
			break;

		if (!FillConsoleOutputCharacterA(hConsole, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &dwWritten))
			break;

		if (!FillConsoleOutputAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE, screen.dwSize.X * screen.dwSize.Y, topLeft, &dwWritten))
			break;

		if (!SetConsoleCursorPosition(hConsole, topLeft))
			break;

	} while (false);
}

DWORD64 GetModuleBaseAddr(DWORD dwPid, LPCWSTR lpDllName) 
{
	HANDLE hSnap = 0;
	DWORD64 dwBaseAddr = 0;
	MODULEENTRY32 me32 = { sizeof(MODULEENTRY32) };

	do
	{
		if ((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid)) == NULL)
			break;

		if (!Module32First(hSnap, &me32))
			break;

		do 
		{
			if (lstrcmpW(me32.szModule, lpDllName) == 0) 
			{
				dwBaseAddr = (DWORD64)me32.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnap, &me32));

	} while (false);

	if (hSnap)
		CloseHandle(hSnap);

	return dwBaseAddr;
}

void GetGameInfo(GameInfo &Info)
{
	while (true)
	{
		HWND hWnd = 0;
		do
		{
			if ((hWnd = FindWindowW(0, L"Genshin Impact")) == NULL)
			{
				wprintf(L"Window didnt found!\n");
				break;
			}

			if (!GetWindowThreadProcessId(hWnd, &Info.dwPid))
			{
				wprintf(L"Couldnt get PID!\n");
				break;
			}

			if ((Info.hProc = OpenProcess(PROCESS_ALL_ACCESS, true, Info.dwPid)) == NULL)
			{
				wprintf(L"Couldnt get process handle!\n");
				break;
			}
			
			if ((Info.dwUnityPlayer = GetModuleBaseAddr(Info.dwPid, L"UnityPlayer.dll")) == NULL)
			{
				wprintf(L"Couldnt get UnityPlayer.dll base addr!\n");
				break;
			}

			if ((Info.dwUserAssembly = GetModuleBaseAddr(Info.dwPid, L"UserAssembly.dll")) == NULL)
			{
				wprintf(L"Couldnt get UserAssembly.dll base addr!\n");
				break;
			}
			return;
		} while (false);
		Sleep(1500);
	}
}

void GetSpawnPosition(GameInfo gInfo, GameVector &vecInfo)
{
	DWORD64 dwPointer = 0;
	do
	{
		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(gInfo.dwUnityPlayer + Offset::m_vecSpawn), &vecInfo, sizeof(GameVector), 0))
		{
			wprintf(L"Couldnt read spawn cords!\n");
			break;
		}
	} while (false);
}

void SetRelativePosition(GameInfo gInfo, GameVector& vecInfo)
{
	DWORD64 dwPointer = 0;
	DWORD64 dwPointer2 = 0;
	DWORD64 dwPointer3 = 0;
	DWORD64 dwPointer4 = 0;
	DWORD64 dwPointer5 = 0;
	DWORD64 dwPointer6 = 0;
	SIZE_T szWritten = 0;
	do
	{
		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(gInfo.dwUserAssembly + Offset::m_dwRelPointer), &dwPointer, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer + Offset::m_dwRelPointer2), &dwPointer2, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 2 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer2 + Offset::m_dwRelPointer3), &dwPointer3, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 3 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer3 + Offset::m_dwRelPointer4), &dwPointer4, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 4 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer4 + Offset::m_dwRelPointer5), &dwPointer5, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 5 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer5 + Offset::m_dwRelPointer6), &dwPointer6, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 6 to relative cords!\n");
			break;
		}

		if (!WriteProcessMemory(gInfo.hProc, (LPVOID)(dwPointer6 + Offset::m_vecRelative), &vecInfo, sizeof(GameVector), &szWritten))
		{
			wprintf(L"Couldnt write relative cords!\n");
			break;
		}

		szWritten = 0;
		if (!WriteProcessMemory(gInfo.hProc, (LPVOID)(dwPointer6 + Offset::m_vecRelative2), &vecInfo, sizeof(GameVector), &szWritten))
		{
			wprintf(L"Couldnt write 2 relative cords!\n");
			break;
		}

	} while (false);
}

void GetRelativePosition(GameInfo gInfo, GameVector& vecInfo)
{
	DWORD64 dwPointer = 0;
	DWORD64 dwPointer2 = 0;
	DWORD64 dwPointer3 = 0;
	DWORD64 dwPointer4 = 0;
	DWORD64 dwPointer5 = 0;
	DWORD64 dwPointer6 = 0;
	do
	{
		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(gInfo.dwUserAssembly + Offset::m_dwRelPointer), &dwPointer, sizeof(DWORD64), 0))
		{
			wprintf(L"  [Error] Couldnt read pointer to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer + Offset::m_dwRelPointer2), &dwPointer2, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 2 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer2 + Offset::m_dwRelPointer3), &dwPointer3, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 3 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer3 + Offset::m_dwRelPointer4), &dwPointer4, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 4 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer4 + Offset::m_dwRelPointer5), &dwPointer5, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 5 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer5 + Offset::m_dwRelPointer6), &dwPointer6, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 6 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer6 + Offset::m_vecRelative), &vecInfo, sizeof(GameVector), 0))
		{
			wprintf(L"Couldnt read relative cords!\n");
			break;
		}

	} while (false);
}

void SetRelativeVelocity(GameInfo gInfo, GameVector& vecInfo)
{
	DWORD64 dwPointer = 0;
	DWORD64 dwPointer2 = 0;
	DWORD64 dwPointer3 = 0;
	DWORD64 dwPointer4 = 0;
	DWORD64 dwPointer5 = 0;
	DWORD64 dwPointer6 = 0;
	SIZE_T szWritten = 0;
	do
	{
		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(gInfo.dwUserAssembly + Offset::m_dwRelPointer), &dwPointer, sizeof(DWORD64), 0))
		{
			wprintf(L"  [Error] Couldnt read pointer to velocity!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer + Offset::m_dwRelPointer2), &dwPointer2, sizeof(DWORD64), 0))
		{
			wprintf(L"  [Error] Couldnt read pointer 2 to velocity!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer2 + Offset::m_dwRelPointer3), &dwPointer3, sizeof(DWORD64), 0))
		{
			wprintf(L"  [Error] Couldnt read pointer 3 to velocity!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer3 + Offset::m_dwRelPointer4), &dwPointer4, sizeof(DWORD64), 0))
		{
			wprintf(L"  [Error] Couldnt read pointer 4 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer4 + Offset::m_dwRelPointer5), &dwPointer5, sizeof(DWORD64), 0))
		{
			wprintf(L"  [Error] Couldnt read pointer 5 to relative cords!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer5 + Offset::m_dwRelPointer6), &dwPointer6, sizeof(DWORD64), 0))
		{
			wprintf(L"  [Error] Couldnt read pointer 6 to relative cords!\n");
			break;
		}

		if (!WriteProcessMemory(gInfo.hProc, (LPVOID)(dwPointer6 + Offset::m_vecVelocity), &vecInfo, sizeof(GameVector), &szWritten))
		{
			wprintf(L"  [Error] Couldnt write velocity!\n");
			break;
		}

		szWritten = 0;
		if (!WriteProcessMemory(gInfo.hProc, (LPVOID)(dwPointer6 + Offset::m_vecVelocity2), &vecInfo, sizeof(GameVector), &szWritten))
		{
			wprintf(L"  [Error] Couldnt write 2 velocity!\n");
			break;
		}

	} while (false);
}

bool IsLoadScreenActive(GameInfo gInfo)
{
	DWORD64 dwPointer = 0;
	DWORD64 dwPointer2 = 0;
	bool bRes = false;
	do
	{
		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(gInfo.dwUserAssembly + Offset::m_dwLsPointer), &dwPointer, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer to load screen!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer + Offset::m_dwLsPointer2), &dwPointer2, sizeof(DWORD64), 0))
		{
			wprintf(L"Couldnt read pointer 2 to load screen!\n");
			break;
		}

		if (!ReadProcessMemory(gInfo.hProc, (LPVOID)(dwPointer2 + Offset::m_bLsStatus), &bRes, sizeof(bool), 0))
		{
			wprintf(L"Couldnt read load screen status!\n");
			break;
		}
	} while (false);

	return bRes;
}

void Teleport(GameInfo &gInfo, GameVector &vecTeleport)
{
	GameVector vecSpawn = { 0 };
	GameVector vecFreezVel = { 0 };
	GameVector tempVec = { 0 };
	while (true)
	{
		if (IsLoadScreenActive(gInfo)) //need to revers this -> UserAssembly.dll+2209130 and UserAssembly.dll+220B9A0, if u wanna make it better :)
		{
			wprintf(L"Spoofing cords...\n");
			while (IsLoadScreenActive(gInfo) || (GetAsyncKeyState(VK_F7) & 1) == false)
			{
				GetSpawnPosition(gInfo, vecSpawn);
				tempVec = vecTeleport - vecSpawn;
				SetRelativePosition(gInfo, tempVec);
				SetRelativeVelocity(gInfo, vecFreezVel);
				Sleep(1);
			}
			wprintf(L"Cords was spoofed :)\n");
			break;
		}
		Sleep(15);
	}
}

bool IsDirectoryExists(LPCWSTR lpPath)
{
	DWORD dwFileAttr = GetFileAttributesW(lpPath);
	bool bRes = true;

	if (dwFileAttr == INVALID_FILE_ATTRIBUTES || !(dwFileAttr & FILE_ATTRIBUTE_DIRECTORY))
		bRes = false;

	return bRes;
}

bool IsFileExists(LPCWSTR lpPath)
{
	DWORD dwFileAttr = GetFileAttributesW(lpPath);
	bool bRes = true;

	if (dwFileAttr == INVALID_FILE_ATTRIBUTES || (dwFileAttr & FILE_ATTRIBUTE_DIRECTORY))
		bRes = false;

	return bRes;
}

void EnumAllFiles(int iSelected, String& strSelected)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = 0;
	int iCount = 0;
	_cls();

	do
	{
		if ((hFind = FindFirstFileW(LOCS_PATH2, &FindFileData)) == INVALID_HANDLE_VALUE)
			break;

		do
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;

			if (iCount == iSelected)
			{
				strSelected = LOCS_PATH;
				strSelected += FindFileData.cFileName;
				wprintf(L"%s <--\n", FindFileData.cFileName);
			}
			else
			{
				wprintf(L"%s\n", FindFileData.cFileName);
			}

			iCount++;
		} while (FindNextFileW(hFind, &FindFileData));

	} while (false);

	if (!iCount)
		wprintf(L"U dont have a locations :(\n");

	if(hFind)
		FindClose(hFind);
}

int main()
{
	GameInfo gInfo = { 0 };
	GameVector vecTeleport = { 0 };
	GameVector vecTemp = { 0 };
	GameVector vecTemp2 = { 0 };
	GameVector vecTemp3 = { 0 };
	//
	String strSelected;
	wchar_t flData[12] = { 0 };
	//
	DWORD dwExitCode = 0;
	int iSelected = 0;
	//
	bool bShowed = false;
	GetGameInfo(gInfo);

	do
	{
		if (!IsDirectoryExists(LOCS_PATH))
		{
			if (!CreateDirectoryW(LOCS_PATH, 0))
			{
				wprintf(L"Couldnt create locations directory!\n");
				break;
			}
		}

		while (true)
		{
			if (!GetExitCodeProcess(gInfo.hProc, &dwExitCode) && dwExitCode != STILL_ACTIVE)
			{
				wprintf(L"Game process was closed...\n");
				break;
			}

			if (GetAsyncKeyState(VK_RETURN) & 1)
			{
				_cls();
				do
				{
					if (!strSelected.data())
					{
						wprintf(L"U havnt selected a file!\n");
						break;
					}

					if (!GetPrivateProfileStringW(L"vector", L"x", 0, flData, 12, strSelected.data()))
						break;
					vecTeleport.x = (float)wcstod(flData, 0);

					if (!GetPrivateProfileStringW(L"vector", L"y", 0, flData, 12, strSelected.data()))
						break;
					vecTeleport.y = (float)wcstod(flData, 0);

					if (!GetPrivateProfileStringW(L"vector", L"z", 0, flData, 12, strSelected.data()))
						break;
					vecTeleport.z = (float)wcstod(flData, 0);

					wprintf(L"Ready for spoofing cords!\n");
					Teleport(gInfo, vecTeleport);
					EnumAllFiles(iSelected, strSelected);
				} while (false);
			}
			else if (GetAsyncKeyState(VK_UP) & 1 || !bShowed)
			{
				if (iSelected - 1 > -1)
					iSelected--;

				EnumAllFiles(iSelected, strSelected);
				bShowed = true;
			}
			else if (GetAsyncKeyState(VK_DOWN) & 1)
			{
				iSelected++;
				EnumAllFiles(iSelected, strSelected);
			}
			else if (GetAsyncKeyState(VK_F6) & 1)
			{
				_cls();
				GetSpawnPosition(gInfo, vecTemp);
				GetRelativePosition(gInfo, vecTemp2);
				//
				vecTemp3 = vecTemp + vecTemp2;
				vecTemp3.z = vecTemp2.z;
				wprintf(L"[vector]\nx=%f\ny=%f\nz=%f", vecTemp3.x, vecTemp3.y, vecTemp3.z);
			}
			Sleep(5);
		}
	} while (false);
	Sleep(5000);
}