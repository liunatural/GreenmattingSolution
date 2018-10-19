
// HXVideoStreamPlayerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HXVideoStreamPlayer.h"
#include "HXVideoStreamPlayerDlg.h"
#include "afxdialogex.h"
#include "stdint.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CHXVideoStreamPlayerDlg dialog



CHXVideoStreamPlayerDlg::CHXVideoStreamPlayerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_HXVIDEOSTREAMPLAYER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHXVideoStreamPlayerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnStart);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
}

BEGIN_MESSAGE_MAP(CHXVideoStreamPlayerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CHXVideoStreamPlayerDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CHXVideoStreamPlayerDlg::OnBnClickedCancel)
	ON_WM_TIMER()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CHXVideoStreamPlayerDlg message handlers

BOOL CHXVideoStreamPlayerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	bConnected = false;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CHXVideoStreamPlayerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CHXVideoStreamPlayerDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect(&rc);

	if (IsIconic())
	{

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();
		if (!m_image.IsNull())
		{
				HDC dcImage = m_image.GetDC();
				SetStretchBltMode(dc.m_hDC, COLORONCOLOR);
				SetBrushOrgEx(dc.m_hDC, 0, 0, NULL);
				SetStretchBltMode(dcImage, COLORONCOLOR);
				//SetBrushOrgEx(dcImage, 0, 0, NULL);
				SetBkMode(dcImage, TRANSPARENT);
				


				int sw = rc.Width();
				int sh = rc.Height();
				int ds = m_image.GetWidth();
				int dh = m_image.GetHeight();

				StretchBlt(dc.m_hDC, 0, 0, rc.Width(), rc.Height(), dcImage, 0, 0, m_image.GetWidth(), m_image.GetHeight(), SRCCOPY);
				//BitBlt(dc.m_hDC, 0, 0, rc.Width(), rc.Height(), dcImage,m_image.GetWidth(), m_image.GetHeight(), SRCCOPY);
				m_image.ReleaseDC();

		}
	}

	//CDialogEx::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CHXVideoStreamPlayerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHXVideoStreamPlayerDlg::OnBnClickedOk()
{
	pHXStreamNetClientSvc = CreateVideoStreamNetClientService();
	bool ret = pHXStreamNetClientSvc->Connect("127.0.0.1", 10002);
	if (ret)
	{	
		bConnected = true;
		SetTimer(0, 30, 0);
		m_btnStart.ShowWindow(SW_HIDE);
		m_btnClose.ShowWindow(SW_SHOW);
	}
	else
	{
		delete pHXStreamNetClientSvc;
		pHXStreamNetClientSvc = NULL;
		MessageBox(L"Connect Server failed.");
	}

}


void CHXVideoStreamPlayerDlg::OnBnClickedCancel()
{
	if (pHXStreamNetClientSvc)
	{
		pHXStreamNetClientSvc->Close();
		delete pHXStreamNetClientSvc;
		pHXStreamNetClientSvc = NULL;
	}

	m_btnStart.ShowWindow(SW_SHOW);
	m_btnClose.ShowWindow(SW_HIDE);
	bConnected = false;
}


void CHXVideoStreamPlayerDlg::OnTimer(UINT_PTR nIDEvent)
{
	static bool bIn = false;
	if (!bIn)
	{
		bIn = true;
		if (bConnected)
		{
			uint8_t* data = NULL;
			int width = 0;
			int height = 0;
			int size = 0;

			bool ret = pHXStreamNetClientSvc->GetStreamData(&data, width, height, size);
			if (ret)
			{
				if (m_image.IsNull())
				{
					m_image.Create(width, height, 24);
					CRect rc;
					GetWindowRect(&rc);
					rc.right = rc.left + width;
					rc.bottom = rc.top + height;
					MoveWindow(&rc);
				}


				uint8_t c;
				uint8_t* tmp = data;

				for (int i = 0; i < height; i++)
				{
					for (int j = 0; j < width; j++)
					{
						c = *tmp;
						*tmp = *(tmp + 2);
						*(tmp + 2) = c;
						tmp += 3;
					}
				}

				int nPitch = m_image.GetPitch();
				for (int y = 0; y < m_image.GetHeight(); y++)
				{
					char* dst = (char*)m_image.GetBits() + y*nPitch;
					memcpy(dst, data + y* width * 3, width * 3);
				}



			}
		}
		bIn = false;
	}

	if (nIDEvent == 0)
	{
		Invalidate(false);
	}

	CDialogEx::OnTimer(nIDEvent);

}


void CHXVideoStreamPlayerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	EndDialog(IDCANCEL);
	//CDialogEx::OnClose();
}
