#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#define SERVICE_NAME _T("SystemPayloadService")

int _tmain(int argc, _TCHAR* argv[]) {
    SC_HANDLE hSCManager = NULL;
    SC_HANDLE hService = NULL;
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD dwBytesNeeded;

    // Parse command-line arguments
    if (argc < 2) {
        _tprintf(_T("Usage: %s <payload path> [computer]\n"), argv[0]);
        return 1;
    }

    const _TCHAR* payloadPath = argv[1];
    const _TCHAR* computer = (argc >= 3) ? argv[2] : NULL;

    // Connect to SCM
    hSCManager = OpenSCManager(
        computer,
        NULL,
        SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE
    );

    if (!hSCManager) {
        _tprintf(_T("OpenSCManager failed. Error: %d\n"), GetLastError());
        return 1;
    }

    // Check if service exists (with proper access rights)
    hService = OpenService(
        hSCManager,                     // SCM handle
        SERVICE_NAME,                   // Service name
        SERVICE_QUERY_STATUS | SERVICE_START  // Required access
    );

    // Create service if it doesn't exist
    if (!hService) {
        DWORD dwError = GetLastError();
        if (dwError == ERROR_SERVICE_DOES_NOT_EXIST) {
            // Create the service
            hService = CreateService(
                hSCManager,
                SERVICE_NAME,
                _T("System Payload Service"),
                SERVICE_ALL_ACCESS,
                SERVICE_WIN32_OWN_PROCESS,
                SERVICE_AUTO_START,
                SERVICE_ERROR_NORMAL,
                payloadPath,
                NULL,
                NULL,
                NULL,
                _T("LocalSystem"),
                NULL
            );

            if (!hService) {
                _tprintf(_T("CreateService failed. Error: %d\n"), GetLastError());
                CloseServiceHandle(hSCManager);
                return 1;
            }
            _tprintf(_T("Service created successfully.\n"));
        }
        else {
            _tprintf(_T("OpenService failed. Error: %d\n"), dwError);
            CloseServiceHandle(hSCManager);
            return 1;
        }
    }

    // Start the service if not running
    if (!QueryServiceStatusEx(
        hService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ssStatus,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded
    )) {
        _tprintf(_T("QueryServiceStatusEx failed. Error: %d\n"), GetLastError());
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return 1;
    }

    if (ssStatus.dwCurrentState != SERVICE_RUNNING) {
        if (!StartService(hService, 0, NULL)) {
            DWORD dwError = GetLastError();
            if (dwError != ERROR_SERVICE_ALREADY_RUNNING) {
                _tprintf(_T("StartService failed. Error: %d\n"), dwError);
                CloseServiceHandle(hService);
                CloseServiceHandle(hSCManager);
                return 1;
            }
        }
        _tprintf(_T("Service started successfully.\n"));
    }
    else {
        _tprintf(_T("Service is already running.\n"));
    }

    // Cleanup
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);

    return 0;
}