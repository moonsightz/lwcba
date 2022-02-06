#include <iostream>
#include <string>

#include <Windows.h>


/* Constants */

// NOTE: This value is OK in most cases.
// Not to drop any path, size must be get by API and WCHAR handling will be required.
// (And CreateProcess() has a limit of lpCommandLine length)
constexpr static size_t cPathLengthMax = 32768;

constexpr static size_t cCommandLengthMax = 32767;

/* Variables */
static char *sClipboardText = nullptr;

static UINT sPathNum = 0;
static char *sPathBuf = nullptr;


/* Prototypes */
static int GetClipboardText(void);


int main(int argc, char **argv)
{
    if (argc < 2) {
        std::cerr << "Application file must be specified." << std::endl;
        return -1;
    }

    // CreateProcessA() does not set argv[0] when lpApplicationName is set???
    // So use lpCommandLine to pass application path and args, despite the restriction of MAX_PATH.
    // https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createprocessa

    std::string argString;
    argString.reserve(cCommandLengthMax);
    argString += argv[1];
    for (int i = 2; i < argc; ++i) {
        argString += ' ';
        argString += argv[i];
    }
    int r = GetClipboardText();
    if (r != 0) {
        return r;
    }

    if (sClipboardText != nullptr) {
        argString += ' ';
        argString += sClipboardText;
    }
    if (sPathNum > 0) {
        for (size_t i = 0; i < sPathNum; ++i) {
            argString += ' ';
            argString += &sPathBuf[cPathLengthMax * i];
        }
    }

    STARTUPINFOA startupInfo;
    PROCESS_INFORMATION processInfo;
    ::ZeroMemory(&startupInfo, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);
    ::ZeroMemory(&processInfo, sizeof(processInfo));

    // CreateProcessA() does not modify lpCommandLine.
    BOOL b = ::CreateProcessA(nullptr, argString.data(), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);
    if (b == FALSE) {
        std::cerr << "Failed to CreateProcess() " << ::GetLastError() << std::endl;
        return -1;
    }

    (void)::CloseHandle(startupInfo.hStdInput);
    (void)::CloseHandle(startupInfo.hStdOutput);
    (void)::CloseHandle(startupInfo.hStdError);

    (void)::CloseHandle(processInfo.hProcess);
    (void)::CloseHandle(processInfo.hThread);

    return 0;
}


static int GetClipboardText(void)
{
    BOOL b = ::OpenClipboard(NULL);
    if (b == FALSE) {
        std::cerr << "Failed to OpenClipboard()" << std::endl;
        return -1;
    }
    
    HGLOBAL handle;
    
    handle = ::GetClipboardData(CF_TEXT);
    if (handle != NULL) {
        sClipboardText = (char *)::GlobalLock(handle);
        ::GlobalUnlock(handle);
    }
    handle = ::GetClipboardData(CF_HDROP);
    if (handle != NULL) {
        sPathNum = ::DragQueryFileA(static_cast<HDROP>(handle), 0xFFFFFFFF, nullptr, 0);
        sPathBuf = new(std::nothrow) char[cPathLengthMax * sPathNum];
        if (sPathBuf == nullptr) {
            std::cerr << "Failed to allocate path buffer." << std::endl;
            (void)::CloseClipboard();
            return -1;
        }

        for (UINT i = 0; i < sPathNum; ++i) {
            UINT r = ::DragQueryFileA(static_cast<HDROP>(handle), i, &sPathBuf[cPathLengthMax * i], cPathLengthMax);
            if (r == 0) {
                std::cerr << "Failed to DragQueryFile() " << i << std::endl;
                (void)::CloseClipboard();
                return -1;
            }
        }
    }

    (void)::CloseClipboard();

    return 0;
}
