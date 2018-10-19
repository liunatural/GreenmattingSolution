
// HXVideoStreamPlayerDlg.h : header file
//

#pragma once


#include <atlimage.h>
#include "HXStreamNetClient.h"
#include "afxwin.h"


// CHXVideoStreamPlayerDlg dialog
class CHXVideoStreamPlayerDlg : public CDialogEx
{
// Construction
public:
	CHXVideoStreamPlayerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HXVIDEOSTREAMPLAYER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


private:
	CButton m_btnStart;
	CButton m_btnClose;

	CImage	m_image;
	HXStreamNetClient *pHXStreamNetClientSvc;
	bool bConnected;


public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	afx_msg void OnClose();
};
