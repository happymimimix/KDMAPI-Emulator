// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"
#include <windows.h>
#include <mmsystem.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <wtypes.h>
#include <winternl.h>

#define KDMAPI __stdcall
#define EXPORT extern "C" __declspec(dllexport)

// KDMAPI version
#define CUR_MAJOR	4
#define CUR_MINOR	1
#define CUR_BUILD	0
#define CUR_REV		0

#define MODM_INIT		DRVM_INIT
#define MODM_GETNUMDEVS     1
#define MODM_GETDEVCAPS     2
#define MODM_OPEN           3
#define MODM_CLOSE          4
#define MODM_PREPARE        5
#define MODM_UNPREPARE      6
#define MODM_DATA           7
#define MODM_LONGDATA       8
#define MODM_RESET          9
#define MODM_GETVOLUME      10
#define MODM_SETVOLUME      11
#define MODM_CACHEPATCHES       12
#define MODM_CACHEDRUMPATCHES   13
#define MODM_STRMDATA               14
#define MODM_GETPOS                 17
#define MODM_PAUSE                  18
#define MODM_RESTART                19
#define MODM_STOP                   20
#define MODM_PROPERTIES             21
#define MODM_PREFERRED              22
#define MODM_RECONFIGURE            (0x4000+0x0768)

// The debug info struct, you can set the default values by assigning DEFAULT_DEBUG
struct DriverDebugInfo
{
	FLOAT RenderingTime = 0.0f;				// Current BASS rendering time
	DWORD ActiveVoices[16] = { 0 };			// Active voices for each channel

	// ASIO debug info (DO NOT USE)
	DOUBLE ASIOInputLatency = 0.0f;
	DOUBLE ASIOOutputLatency = 0.0f;

	// Threads info
	DOUBLE HealthThreadTime = 0.0;
	DOUBLE ATThreadTime = 0.0;
	DOUBLE EPThreadTime = 0.0;
	DOUBLE CookedThreadTime = 0.0;

	// SoundFonts list
	DWORD CurrentSFList;

	// Audio latency
	DOUBLE AudioLatency = 0.0f;
	DWORD AudioBufferSize = 0;

	// Add more down here
	// ------------------
};

struct WinMMDevice {
	HMIDIOUT handle;
	bool isOpen;
	UINT deviceID;
};

extern WinMMDevice g_midiDevice;

static LARGE_INTEGER TickStart = { 0 };

#endif //PCH_H
