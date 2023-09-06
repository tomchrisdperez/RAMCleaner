#pragma once

#define WM_SHOWTASK (WM_USER + 10)
#define WM_USER_UPDATEDATA (WM_USER + 5)

class CRAMCleanerDlg : public CDialogEx {

public:
    CRAMCleanerDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_RAMCLEANER_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:
    HICON m_hIcon;
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
private:
    bool isStarted = false;
public:
    afx_msg void OnClickedCommence();
    afx_msg void OnClickedOptimize();
    afx_msg void OnBnClickedStoptimer();
    afx_msg void OnBnClickedRunbg();
    afx_msg  LRESULT  onShowTask(WPARAM   wParam, LPARAM   lParam);
    afx_msg void OnBnClickedCheck1();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    LRESULT OnUpdateData(WPARAM wParam, LPARAM lParam);
};
