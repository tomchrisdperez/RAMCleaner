#include "stdafx.h"
#include "RAMCleaner.h"
#include "RAMCleanerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CRAMCleanerApp, CWinApp)
    ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CRAMCleanerApp::CRAMCleanerApp() {
}

CRAMCleanerApp theApp;

BOOL CRAMCleanerApp::InitInstance() {
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);
    CWinApp::InitInstance();
    AfxEnableControlContainer();
    CShellManager *pShellManager = new CShellManager;
    SetRegistryKey(_T("LocalAPP"));
    CRAMCleanerDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
    } else if (nResponse == IDCANCEL) {
    }
    if (pShellManager != NULL)
        delete pShellManager;
    return FALSE;
}

