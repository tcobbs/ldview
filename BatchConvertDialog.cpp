#include "BatchConvertDialog.h"
#include "Resource.h" // For dialog and control IDs
#include "BatchStlConverter.h" // For BatchStlConverter
#include <shlobj.h>  // For SHBrowseForFolder
#include <string>    // For std::wstring, std::string
#include <vector>    // For path buffer
#include <codecvt>   // For std::wstring_convert (potentially, or use WinAPI directly)

// Constructor
BatchConvertDialog::BatchConvertDialog(HWND hParent)
    : m_hParent(hParent), m_hDlg(NULL) {
}

// Method to show the dialog
INT_PTR BatchConvertDialog::Show() {
    // HINSTANCE hInstance = GetModuleHandle(NULL); // This is also valid
    return DialogBoxParam(
        GetModuleHandle(NULL), // Use current module's instance handle
        MAKEINTRESOURCE(IDD_BATCH_CONVERT_STL),
        m_hParent,
        DialogProc,
        reinterpret_cast<LPARAM>(this)
    );
}

// Static DialogProc
INT_PTR CALLBACK BatchConvertDialog::DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    BatchConvertDialog* pDlg = nullptr;

    if (message == WM_INITDIALOG) {
        SetWindowLongPtr(hDlg, DWLP_USER, reinterpret_cast<LONG_PTR>(lParam));
        pDlg = reinterpret_cast<BatchConvertDialog*>(lParam);
        if (pDlg) {
            pDlg->m_hDlg = hDlg; // Store HWND
            return pDlg->OnInitDialog(hDlg);
        }
        return (INT_PTR)FALSE; // Should not happen if DialogBoxParam is called correctly
    }

    // Retrieve the dialog class instance for other messages
    pDlg = reinterpret_cast<BatchConvertDialog*>(GetWindowLongPtr(hDlg, DWLP_USER));
    if (!pDlg) {
        // This could happen if DWLP_USER is not set yet or got corrupted.
        // For WM_COMMAND and WM_CLOSE, we need pDlg.
        if (message == WM_COMMAND || message == WM_CLOSE) {
             // OutputDebugString(L"BatchConvertDialog::DialogProc - pDlg is null for critical message.\n");
        }
        return (INT_PTR)FALSE; // Or DefWindowProc for some messages, but FALSE is typical for unhandled dialog messages
    }

    switch (message) {
        case WM_COMMAND:
            return pDlg->OnCommand(wParam, lParam);
        
        case WM_CLOSE:
            return pDlg->OnClose();
    }
    return (INT_PTR)FALSE; // Default handling for unhandled messages
}

// OnInitDialog handler
BOOL BatchConvertDialog::OnInitDialog(HWND hDlg) {
    // Optional: Center the dialog
    if (m_hParent) { // Check if parent exists
        RECT rcOwner, rcDlg;
        GetWindowRect(m_hParent, &rcOwner);
        GetWindowRect(hDlg, &rcDlg);
        
        int x = rcOwner.left + (rcOwner.right - rcOwner.left) / 2 - (rcDlg.right - rcDlg.left) / 2;
        int y = rcOwner.top + (rcOwner.bottom - rcOwner.top) / 2 - (rcDlg.bottom - rcDlg.top) / 2;
        
        SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
    } else { // Center on desktop if no parent
        RECT rcDesktop, rcDlg;
        GetWindowRect(GetDesktopWindow(), &rcDesktop);
        GetWindowRect(hDlg, &rcDlg);

        int x = rcDesktop.left + (rcDesktop.right - rcDesktop.left) / 2 - (rcDlg.right - rcDlg.left) / 2;
        int y = rcDesktop.top + (rcDesktop.bottom - rcDesktop.top) / 2 - (rcDlg.bottom - rcDlg.top) / 2;

        SetWindowPos(hDlg, HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
    }
    return TRUE; // Return TRUE to set focus to the default control
}

// OnCommand handler
BOOL BatchConvertDialog::OnCommand(WPARAM wParam, LPARAM lParam) {
    switch (LOWORD(wParam)) {
        case IDC_BUTTON_BROWSEINPUT: {
            BROWSEINFOW bi = { 0 };
            bi.hwndOwner = m_hDlg;
            bi.lpszTitle = L"Select Input Directory";
            bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
            LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);

            if (pidl != nullptr) {
                wchar_t path[MAX_PATH];
                if (SHGetPathFromIDListW(pidl, path)) {
                    m_inputDir = path;
                    SetDlgItemTextW(m_hDlg, IDC_EDIT_INPUTDIR, m_inputDir.c_str());
                }
                CoTaskMemFree(pidl);
            }
            return TRUE;
        }

        case IDC_BUTTON_BROWSEOUTPUT: {
            BROWSEINFOW bi = { 0 };
            bi.hwndOwner = m_hDlg;
            bi.lpszTitle = L"Select Output Directory";
            bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
            LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);

            if (pidl != nullptr) {
                wchar_t path[MAX_PATH];
                if (SHGetPathFromIDListW(pidl, path)) {
                    m_outputDir = path;
                    SetDlgItemTextW(m_hDlg, IDC_EDIT_OUTPUTDIR, m_outputDir.c_str());
                }
                CoTaskMemFree(pidl);
            }
            return TRUE;
        }

        case IDC_BUTTON_STARTBATCH: {
            // Retrieve text from edit controls to ensure m_inputDir and m_outputDir are current
            // (though SHBrowseForFolder already updated them)
            std::vector<wchar_t> inputBuf(MAX_PATH);
            std::vector<wchar_t> inputBuf(MAX_PATH);
            std::vector<wchar_t> outputBuf(MAX_PATH);

            GetDlgItemTextW(m_hDlg, IDC_EDIT_INPUTDIR, inputBuf.data(), MAX_PATH);
            GetDlgItemTextW(m_hDlg, IDC_EDIT_OUTPUTDIR, outputBuf.data(), MAX_PATH);
            
            m_inputDir = inputBuf.data();
            m_outputDir = outputBuf.data();

            if (m_inputDir.empty() || m_outputDir.empty()) {
                MessageBoxW(m_hDlg, L"Input and Output directories must be specified.", L"Validation Error", MB_OK | MB_ICONERROR);
                return TRUE;
            }

            // Disable UI Elements
            EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_STARTBATCH), FALSE);
            EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_BROWSEINPUT), FALSE);
            EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_BROWSEOUTPUT), FALSE);
            EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_INPUTDIR), FALSE);
            EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_OUTPUTDIR), FALSE);

            // Clear Log
            SetDlgItemTextW(m_hDlg, IDC_EDIT_LOG, L"");

            // Helper function for wstring to string (UTF-8)
            auto wstringToString = [](const std::wstring& wstr) -> std::string {
                if (wstr.empty()) return std::string();
                int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
                std::string strTo(size_needed, 0);
                WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
                return strTo;
            };
            std::string inputDirStdStr = wstringToString(m_inputDir);
            std::string outputDirStdStr = wstringToString(m_outputDir);

            BatchStlConverter converter(inputDirStdStr, outputDirStdStr);

            // Set Logger
            converter.setLogger([this](const std::string& type, const std::string& message) {
                auto stringToWstring = [](const std::string& str) -> std::wstring {
                    if (str.empty()) return std::wstring();
                    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
                    std::wstring wstrTo(size_needed, 0);
                    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
                    return wstrTo;
                };

                std::wstring wtype = stringToWstring(type);
                std::wstring wmessage = stringToWstring(message);
                std::wstring logLine = L"[" + wtype + L"] " + wmessage + L"\r\n"; // Use \r\n for Windows multiline edit
                
                HWND hLogEdit = GetDlgItem(m_hDlg, IDC_EDIT_LOG);
                int currentTextLen = GetWindowTextLengthW(hLogEdit);
                SendMessageW(hLogEdit, EM_SETSEL, (WPARAM)currentTextLen, (LPARAM)currentTextLen);
                SendMessageW(hLogEdit, EM_REPLACESEL, FALSE, (LPARAM)logLine.c_str());
            });

            // Run Conversion (Synchronously)
            bool conversionSuccess = converter.runConversion();

            // Show Summary Message
            if (conversionSuccess) {
                MessageBoxW(m_hDlg, L"Batch conversion completed successfully (see log for details).", L"Conversion Finished", MB_OK | MB_ICONINFORMATION);
            } else {
                MessageBoxW(m_hDlg, L"Batch conversion completed with errors (see log for details).", L"Conversion Finished", MB_OK | MB_ICONWARNING);
            }

            // Re-enable UI Elements
            EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_STARTBATCH), TRUE);
            EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_BROWSEINPUT), TRUE);
            EnableWindow(GetDlgItem(m_hDlg, IDC_BUTTON_BROWSEOUTPUT), TRUE);
            EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_INPUTDIR), TRUE);
            EnableWindow(GetDlgItem(m_hDlg, IDC_EDIT_OUTPUTDIR), TRUE);
            
            return TRUE;
        }

        case IDCANCEL:
            EndDialog(m_hDlg, LOWORD(wParam));
            return TRUE;
    }
    return FALSE; // Command not handled
}

// OnClose handler
BOOL BatchConvertDialog::OnClose() {
    EndDialog(m_hDlg, 0); // Or appropriate code, 0 for general close
    return TRUE;
}
