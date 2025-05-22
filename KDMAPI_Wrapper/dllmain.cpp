// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

using namespace std;
// ----- CONFIGURABLE MACROS -----
#define DEVICE_SELECT_WIDTH  500
#define DEVICE_SELECT_HEIGHT 300
#define DEVICE_SELECT_X      10
#define DEVICE_SELECT_Y      10
// Modal dialog state
UINT g_selectedDevice = -1;
HWND g_hListBox = nullptr;
HWND g_hConfirmBtn = nullptr;
WinMMDevice g_midiDevice = { NULL, false, 0 };

enum {
    ID_LABEL = 1001,
    ID_LIST = 1002,
    ID_CONFIRM = 1003
};

// Helper: Get MIDI device names
vector<wstring> GetMidiOutDeviceList() {
    vector<wstring> names;
    UINT n = midiOutGetNumDevs();
    MIDIOUTCAPS caps = {};
    for (UINT i = 0; i < n; ++i) {
        if (midiOutGetDevCaps(i, &caps, sizeof(caps)) == MMSYSERR_NOERROR)
            names.push_back(caps.szPname);
        else
            names.push_back(L"* Unknown MIDI Device *");
    }
    return names;
}

LRESULT CALLBACK DeviceSelectWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg) {
    case WM_CREATE: {
        int winW = DEVICE_SELECT_WIDTH;
        int winH = DEVICE_SELECT_HEIGHT;
        int margin = 10;

        // Label
        CreateWindowW(
            L"STATIC",
            L"Please select a MIDI output device and click confirm: ",
            WS_CHILD | WS_VISIBLE,
            margin, margin, winW - 2 * margin, 20,
            hWnd, (HMENU)ID_LABEL, nullptr, nullptr
        );

        // Listbox
        int listY = margin * 2 + 20;
        int listH = winH - listY - margin * 2 - 20;
        g_hListBox = CreateWindowW(
            L"LISTBOX",
            nullptr,
            WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_VSCROLL | WS_HSCROLL | WS_BORDER,
            margin, listY, winW - 2 * margin, listH,
            hWnd, (HMENU)ID_LIST, nullptr, nullptr
        );

        // Fill listbox
        auto names = GetMidiOutDeviceList();
        for (size_t i = 0; i < names.size(); ++i)
            SendMessageW(g_hListBox, LB_ADDSTRING, 0, (LPARAM)names[i].c_str());
        if (!names.empty())
            SendMessageW(g_hListBox, LB_SETCURSEL, 0, 0);

        // Confirm button
        g_hConfirmBtn = CreateWindowW(
            L"BUTTON",
            L"Confirm",
            WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
            margin, winH - margin * 2 - 20, winW - 2 * margin, 20 + margin,
            hWnd, (HMENU)ID_CONFIRM, nullptr, nullptr
        );
        return 0;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_CONFIRM) {
            int sel = (UINT)SendMessageW(g_hListBox, LB_GETCURSEL, 0, 0);
            if (sel >= 0) g_selectedDevice = sel;
            DestroyWindow(hWnd);
            return 0;
        }
        return 0;
    }
    case WM_CLOSE:
        // Prevent closing except by Confirm
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_CLOSE)
            return 0;
        break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// Modal dialog function
int ShowDeviceSelectDialog(HINSTANCE hInst, HWND hParent)
{
    static const wchar_t* wndClassName = L"KDMAPI Simulator Device Selection Window Class";
    static bool classRegistered = false;
    if (!classRegistered) {
        WNDCLASSW wc = { 0 };
        wc.lpfnWndProc = DeviceSelectWndProc;
        wc.hInstance = hInst;
        wc.lpszClassName = wndClassName;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        RegisterClassW(&wc);
        classRegistered = true;
    }

    RECT rc = { NULL, NULL, DEVICE_SELECT_WIDTH, DEVICE_SELECT_HEIGHT };
    AdjustWindowRectEx(&rc, WS_POPUPWINDOW | WS_CAPTION, FALSE, WS_EX_TOPMOST);

    int winWidth = rc.right - rc.left;
    int winHeight = rc.bottom - rc.top;

    HWND hWnd = CreateWindowExW(
        WS_EX_TOPMOST,
        wndClassName,
        L"KDMAPI Simulator",
        WS_POPUPWINDOW | WS_CAPTION | WS_VISIBLE,
        DEVICE_SELECT_X, DEVICE_SELECT_Y, winWidth, winHeight,
        nullptr, nullptr, hInst, nullptr
    );
    // Remove resizable styles
    SetWindowLongW(hWnd, GWL_STYLE, GetWindowLongW(hWnd, GWL_STYLE) & ~WS_SIZEBOX);

    // Modal loop: disable parent, enable window, block until closed
    if (hParent) {
        EnableWindow(hParent, FALSE);
    }
    SetForegroundWindow(hWnd);

    // Block until window is destroyed by Confirm
    MSG msg;
    while (IsWindow(hWnd)) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (!IsDialogMessageW(hWnd, &msg)) {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }
    if (hParent) {
        EnableWindow(hParent, TRUE);
        SetForegroundWindow(hParent);
    }
    int result = g_selectedDevice;
    g_selectedDevice = -1; // Reset for next use
    return result;
}


// -------------------------------------
// KDMAPI Function Export Implementations
// (see OmniMIDI/DeveloperContent/KDMAPI.md)
// -------------------------------------

// Version info
EXPORT BOOL KDMAPI ReturnKDMAPIVer(LPDWORD Major, LPDWORD Minor, LPDWORD Build, LPDWORD Revision) {
    if (Major) *Major = CUR_MAJOR;
    if (Minor) *Minor = CUR_MINOR;
    if (Build) *Build = CUR_BUILD;
    if (Revision) *Revision = CUR_REV;
    return TRUE;
}
EXPORT BOOL KDMAPI IsKDMAPIAvailable() { return TRUE; }

// Core stream controls
EXPORT BOOL KDMAPI InitializeKDMAPIStream() {
    if (g_midiDevice.isOpen) {
        // Don't do anything if it's already open
        return TRUE;
    }
    int selected = ShowDeviceSelectDialog(GetModuleHandle(NULL), NULL);
    if (selected < 0) return FALSE;
    g_midiDevice.deviceID = (UINT)selected;
    MMRESULT res = midiOutOpen(&g_midiDevice.handle, g_midiDevice.deviceID, 0, 0, CALLBACK_NULL);
    if (res != MMSYSERR_NOERROR) {
        g_midiDevice.isOpen = false;
        return FALSE;
    }
    else {
        g_midiDevice.isOpen = true;
        NtQuerySystemTime(&TickStart);
        return TRUE;
    }
}
EXPORT BOOL KDMAPI TerminateKDMAPIStream() {
    if (!g_midiDevice.isOpen) {
        // Don't do anything if it's already closed
        return TRUE;
    }
    MMRESULT res = midiOutClose(g_midiDevice.handle);
    if (res != MMSYSERR_NOERROR) {
        return FALSE;
    }
    g_midiDevice.deviceID = (UINT)-1;
    g_midiDevice.isOpen = false;
    g_midiDevice.handle = NULL;
    return TRUE;
}
EXPORT VOID KDMAPI ResetKDMAPIStream() {
    if (g_midiDevice.isOpen) {
        midiOutReset(g_midiDevice.handle);
    }
}

// MIDI event sending
EXPORT VOID KDMAPI SendDirectData(DWORD dwMsg) {
    if (g_midiDevice.isOpen) {
        midiOutShortMsg(g_midiDevice.handle, dwMsg);
    }
}
EXPORT VOID KDMAPI SendDirectDataNoBuf(DWORD dwMsg) {
    //Sorry, we can't do no buf in original winmm. 
    SendDirectData(dwMsg);
}
EXPORT BOOL KDMAPI SendCustomEvent(DWORD eventtype, DWORD chan, DWORD param) {
    if (!g_midiDevice.isOpen) {
        return FALSE;
    }
    //IDK what is this, GPT-4.1 gave me this answer: 
    DWORD msg = ((eventtype & 0xFF) << 16) | ((chan & 0xFF) << 8) | (param & 0xFF);
    midiOutShortMsg(g_midiDevice.handle, msg);
    return TRUE;
}

EXPORT UINT KDMAPI SendDirectLongData(MIDIHDR* IIMidiHdr, UINT IIMidiHdrSize) {
    if (!g_midiDevice.isOpen) return MMSYSERR_ERROR;
    return midiOutLongMsg(g_midiDevice.handle, IIMidiHdr, IIMidiHdrSize);
}

EXPORT UINT KDMAPI SendDirectLongDataNoBuf(LPSTR MidiHdrData, DWORD MidiHdrDataLen) {
    if (!g_midiDevice.isOpen) return MMSYSERR_ERROR;
    // Create a local MIDIHDR on the stack
    MIDIHDR hdr = {};
    hdr.lpData = MidiHdrData;
    hdr.dwBufferLength = MidiHdrDataLen;
    hdr.dwFlags = 0;
    // Prepare header
    MMRESULT res = midiOutPrepareHeader(g_midiDevice.handle, &hdr, sizeof(MIDIHDR));
    if (res != MMSYSERR_NOERROR) return res;
    // Send long message
    res = midiOutLongMsg(g_midiDevice.handle, &hdr, sizeof(MIDIHDR));
    // Unprepare header
    midiOutUnprepareHeader(g_midiDevice.handle, &hdr, sizeof(MIDIHDR));
    return res;
}
EXPORT UINT KDMAPI PrepareLongData(MIDIHDR* IIMidiHdr, UINT IIMidiHdrSize) {
    if (!g_midiDevice.isOpen) return MMSYSERR_ERROR;
    return midiOutPrepareHeader(g_midiDevice.handle, IIMidiHdr, IIMidiHdrSize);
}
EXPORT UINT KDMAPI UnprepareLongData(MIDIHDR* IIMidiHdr, UINT IIMidiHdrSize) {
    if (!g_midiDevice.isOpen) return MMSYSERR_ERROR;
    return midiOutUnprepareHeader(g_midiDevice.handle, IIMidiHdr, IIMidiHdrSize);
}

// Stub: No direct support for driver settings
EXPORT BOOL KDMAPI DriverSettings(DWORD, DWORD, LPVOID, UINT) { return FALSE; }

// Stub: No debug info available, return static zeroed struct
EXPORT DriverDebugInfo* KDMAPI GetDriverDebugInfo() {
    return new DriverDebugInfo();
}

// Stub: Soundfont list not supported
EXPORT VOID KDMAPI LoadCustomSoundFontsList(LPWSTR) {}

EXPORT DWORD64 KDMAPI timeGetTime64() {
    LARGE_INTEGER CurrentTime;
    NtQuerySystemTime(&CurrentTime);
    return (DWORD64)((CurrentTime.QuadPart - TickStart.QuadPart) / 10000);
}

EXPORT MMRESULT KDMAPI modMessage(UINT uDeviceID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
    switch (uMsg) {
    case MODM_DATA:
        // Send short MIDI message
        if (g_midiDevice.isOpen)
            return midiOutShortMsg(g_midiDevice.handle, (DWORD)dwParam1);
        return MMSYSERR_ERROR;

    case MODM_LONGDATA:
        // Send SysEx/long MIDI message
        if (g_midiDevice.isOpen)
            return midiOutLongMsg(g_midiDevice.handle, (MIDIHDR*)dwParam1, (UINT)dwParam2);
        return MMSYSERR_ERROR;

    case MODM_PREPARE:
        if (g_midiDevice.isOpen)
            return midiOutPrepareHeader(g_midiDevice.handle, (MIDIHDR*)dwParam1, (UINT)dwParam2);
        return MMSYSERR_ERROR;

    case MODM_UNPREPARE:
        if (g_midiDevice.isOpen)
            return midiOutUnprepareHeader(g_midiDevice.handle, (MIDIHDR*)dwParam1, (UINT)dwParam2);
        return MMSYSERR_ERROR;

    case MODM_GETNUMDEVS:
        return midiOutGetNumDevs();

    case MODM_GETDEVCAPS:
        // Try to detect structure size and call correct function
        if (dwParam2 == sizeof(MIDIOUTCAPSA))
            return midiOutGetDevCapsA(uDeviceID, (MIDIOUTCAPSA*)dwParam1, (UINT)dwParam2);
        if (dwParam2 == sizeof(MIDIOUTCAPSW))
            return midiOutGetDevCapsW(uDeviceID, (MIDIOUTCAPSW*)dwParam1, (UINT)dwParam2);
        return MMSYSERR_INVALPARAM;

    case MODM_SETVOLUME:
        if (g_midiDevice.isOpen)
            return midiOutSetVolume(g_midiDevice.handle, (DWORD)dwParam1);
        return MMSYSERR_ERROR;

    case MODM_GETVOLUME:
        if (g_midiDevice.isOpen)
            return midiOutGetVolume(g_midiDevice.handle, (LPDWORD)dwParam1);
        return MMSYSERR_ERROR;

    case MODM_RESET:
        if (g_midiDevice.isOpen)
            return midiOutReset(g_midiDevice.handle);
        return MMSYSERR_ERROR;

    case MODM_OPEN:
        return InitializeKDMAPIStream() ? MMSYSERR_NOERROR : MMSYSERR_ERROR;
    case MODM_CLOSE:
        return TerminateKDMAPIStream() ? MMSYSERR_NOERROR : MMSYSERR_ERROR;

    case MODM_CACHEPATCHES:
    case MODM_CACHEDRUMPATCHES:
        return MMSYSERR_NOERROR;

        // These require synth/cooked-player state, not available in WinMM
    case MODM_STRMDATA:
    case MODM_PROPERTIES:
    case MODM_GETPOS:
    case MODM_RESTART:
    case MODM_PAUSE:
    case MODM_STOP:
        return MMSYSERR_NOTSUPPORTED;

    default:
        return MMSYSERR_NOTSUPPORTED;
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

