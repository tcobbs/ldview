#ifndef BATCHCONVERTDIALOG_H
#define BATCHCONVERTDIALOG_H

#include <windows.h>
#include <string>

class BatchConvertDialog {
public:
    BatchConvertDialog(HWND hParent);
    INT_PTR Show();

private:
    static INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

    // Handlers for DialogProc messages
    BOOL OnInitDialog(HWND hDlg);
    BOOL OnCommand(WPARAM wParam, LPARAM lParam); // lParam is not used for WM_COMMAND in DialogProc directly but passed for completeness
    BOOL OnClose();

    HWND m_hDlg;
    HWND m_hParent;
    std::wstring m_inputDir;
    std::wstring m_outputDir;
};

#endif // BATCHCONVERTDIALOG_H
