#include "stdafx.h"
#include "RAMCleaner.h"
#include "RAMCleanerDlg.h"
#include "afxdialogex.h"
#include "MemoryUtil.h"
#include "aerosubc.h"
#include <commctrl.h>
#pragma comment(lib,"AeroGlassGDI.lib")

using namespace std;

class CAboutDlg : public CDialogEx {
public:
    CAboutDlg();
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD) {
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    CDialogEx::DoDataExchange(pDX);
}

CRAMCleanerDlg::CRAMCleanerDlg(CWnd *pParent )
    : CDialogEx(CRAMCleanerDlg::IDD, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRAMCleanerDlg::DoDataExchange(CDataExchange *pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRAMCleanerDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_MESSAGE(WM_SHOWTASK, onShowTask)
    ON_BN_CLICKED(IDC_CHECK1, &CRAMCleanerDlg::OnBnClickedCheck1)
    ON_BN_CLICKED(IDC_OPTIMIZE, &CRAMCleanerDlg::OnClickedOptimize)
    ON_BN_CLICKED(IDC_COMMENCE, &CRAMCleanerDlg::OnClickedCommence)
    ON_BN_CLICKED(IDC_STOPTIMER, &CRAMCleanerDlg::OnBnClickedStoptimer)
    ON_BN_CLICKED(IDC_RUNBG, &CRAMCleanerDlg::OnBnClickedRunbg)
    ON_MESSAGE(WM_USER_UPDATEDATA, &CRAMCleanerDlg::OnUpdateData)
END_MESSAGE_MAP()

BOOL CRAMCleanerDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();

    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    if (CMenu* pSysMenu = GetSystemMenu(FALSE)) {
        CString strAboutMenu;
        if (strAboutMenu.LoadString(IDS_ABOUTBOX) && !strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);
    AllAero(m_hWnd);

    return TRUE;
}

void CRAMCleanerDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    } else
        CDialogEx::OnSysCommand(nID, lParam);
}

void CRAMCleanerDlg::OnPaint() {
    if (IsIconic()) {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    } else
        CDialogEx::OnPaint();
}

UINT ThreadFunc(LPVOID pParam) {
    CRAMCleanerDlg* pThis = (CRAMCleanerDlg*)pParam;
    AdjustTokenPrivilegesForNT();
    Info ii = EmptyAllSet();
    pThis->PostMessage(WM_USER_UPDATEDATA, (WPARAM)new Info(ii), 0);
    return 0;
}

NOTIFYICONDATA nid;
bool minize = false;
bool backNotify = false;

void NotifyWithMessage(HWND hwnd, LPCWSTR title, LPCWSTR info) {
    memset(&nid, 0, sizeof(nid));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.uID = IDR_MAINFRAME;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    nid.hWnd = hwnd;
    nid.uCallbackMessage = WM_SHOWTASK;
    nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
    wsprintf(nid.szTip, L"Click here to show the mainframe");
    wsprintf(nid.szInfo, info);
    wsprintf(nid.szInfoTitle, title);
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void ShowTaskDialog(HWND hwnd, PCWSTR mainInstruction, PCWSTR content, TASKDIALOG_COMMON_BUTTON_FLAGS buttons, PCWSTR icon) {
    TaskDialog(hwnd, NULL, mainInstruction, content, NULL, buttons, icon, NULL);
}

void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime) {
    AdjustTokenPrivilegesForNT();
    Info ii = EmptyAllSet();

    CTime time = CTime::GetCurrentTime();
    CString date = time.Format(_T("%Y%m%d-%H:%M:%S"));
    CString message;

    if (ii.isOk) {
        message.Format(_T("Time:%s\nTotal number of processes:%d\nTotal RAM Size:%dMB\nAvailable RAM size after cleaning:%dMB\n"),
            date.GetString(), ii.processess, ii.TotalMem, ii.AvaiMem);

        if (!minize) {
            TaskDialog(NULL, NULL, _T("Tips"), _T("Success"), message.GetString(),
                TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
        }
        else if (backNotify) {
            Shell_NotifyIcon(NIM_DELETE, &nid);
            NotifyWithMessage(hwnd, _T("Autocleaning completed!"), message.GetString());
        }
    }
    else {
        const CString errorTitle = _T("Error");
        const CString errorMsg = ii.isOk ? message.GetString() : _T("Error occurred while cleaning");
        const CString notifyMsg = _T("Please restart the program");

        if (!minize) {
            TaskDialog(NULL, NULL, errorTitle, errorMsg,
                NULL, TDCBF_OK_BUTTON, TD_ERROR_ICON, NULL);
        }
        else {
            Shell_NotifyIcon(NIM_DELETE, &nid);
            NotifyWithMessage(hwnd, errorMsg, notifyMsg);
        }
    }
}

LRESULT CRAMCleanerDlg::OnUpdateData(WPARAM wParam, LPARAM lParam) {
    Info* ii = (Info*)wParam;
    CString message;
    if (ii->isOk) {
        CTime time = CTime::GetCurrentTime();
        message.Format(_T("Time:%s\nTotal number of processes:%d\nTotal RAM Size:%dMB\nAvailable RAM size after cleaning:%dMB\n"),
            time.Format(_T("%Y%m%d-%H:%M:%S")), ii->processess, ii->TotalMem, ii->AvaiMem);
        TaskDialog(NULL, NULL, _T("Tips"), _T("Success"), message, TDCBF_OK_BUTTON, TD_INFORMATION_ICON, NULL);
    }
    else {
        TaskDialog(NULL, NULL, _T("Error"), _T("Error occurred while cleaning"), NULL, TDCBF_OK_BUTTON, TD_ERROR_ICON, NULL);
    }
    delete ii;
    return 0;
}


void CRAMCleanerDlg::OnTimer(UINT_PTR nIDEvent) {
    CDialogEx::OnTimer(nIDEvent);
}

LRESULT CRAMCleanerDlg::onShowTask(WPARAM wParam, LPARAM lParam) {
    if (wParam != IDR_MAINFRAME) return 1;

    switch (lParam) {
    case WM_RBUTTONUP: {
        POINT point;
        CMenu menu;
        ::GetCursorPos(&point);
        menu.CreatePopupMenu();
        menu.AppendMenu(MF_STRING, WM_DESTROY, L"Close");
        menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
        menu.DestroyMenu();
        break;
    } 

    case WM_LBUTTONDOWN:
        this->ShowWindow(SW_SHOW);
        Shell_NotifyIcon(NIM_DELETE, &nid);
        minize = false;
        break;
    }
    return 0;
}

void CRAMCleanerDlg::OnClickedOptimize() {
    AfxBeginThread(ThreadFunc, this);
}

void CRAMCleanerDlg::OnClickedCommence() {
    if (isStarted) {
        ShowTaskDialog(NULL, _T("Tips"), _T("There's already a timing autoclean schedule"), TDCBF_OK_BUTTON, TD_WARNING_ICON);
        return;
    }
    ShowTaskDialog(NULL, _T("Tips"), _T("Timing autoclean schedule has been started, and it will autoclean per 30 seconds."), TDCBF_OK_BUTTON, TD_INFORMATION_ICON);
    SetTimer(1, 30 * 1000, (TIMERPROC)TimerProc);
    isStarted = true;
}

void CRAMCleanerDlg::OnBnClickedStoptimer() {
    if (!isStarted) {
        ShowTaskDialog(NULL, _T("Tips"), _T("No autoclean schedule started."), TDCBF_OK_BUTTON, TD_WARNING_ICON);
        return;
    }
    ShowTaskDialog(NULL, _T("Tips"), _T("Autoclean schedule stopped."), TDCBF_OK_BUTTON, TD_INFORMATION_ICON);
    KillTimer(1);
    isStarted = false;
}

void CRAMCleanerDlg::OnBnClickedRunbg() {
    NotifyWithMessage(this->m_hWnd, L"Click this tray icon to display the main interface of the program."
        , L"The program is running in minimized mode.");
    ShowWindow(SW_HIDE);
    minize = true;
}

void CRAMCleanerDlg::OnBnClickedCheck1() {
    CButton* pBtn = (CButton*)GetDlgItem(IDC_CHECK1);
    backNotify = (pBtn->GetCheck() == 1);
}