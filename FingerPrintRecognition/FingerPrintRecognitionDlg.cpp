
// FingerPrintRecognitionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FingerPrintRecognition.h"
#include "FingerPrintRecognitionDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
//protected:
//	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}



// CFingerPrintRecognitionDlg 对话框

CFingerPrintRecognitionDlg::CFingerPrintRecognitionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFingerPrintRecognitionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFingerPrintRecognitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

//事件响应函数
BEGIN_MESSAGE_MAP(CFingerPrintRecognitionDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ImportImg1, &CFingerPrintRecognitionDlg::OnBnClickedOpenImg1)
	ON_BN_CLICKED(IDC_ImportImg2, &CFingerPrintRecognitionDlg::OnBnClickedOpenImg2)
	ON_BN_CLICKED(IDC_Recognize, &CFingerPrintRecognitionDlg::OnBnClickedRecognize)
	ON_BN_CLICKED(ID_MenuAdd, &CFingerPrintRecognitionDlg::OnBnMenuAdd)
	ON_BN_CLICKED(ID_MenuNew, &CFingerPrintRecognitionDlg::OnBnMenuNew)
	ON_BN_CLICKED(ID_MenuRecognize, &CFingerPrintRecognitionDlg::OnBnMenuRecognize)
END_MESSAGE_MAP()


// CFingerPrintRecognitionDlg 消息处理程序

BOOL CFingerPrintRecognitionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	//add mine
	menu.LoadMenu(IDR_MENU1);
	SetMenu(&menu);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CFingerPrintRecognitionDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFingerPrintRecognitionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFingerPrintRecognitionDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//add mine
void CFingerPrintRecognitionDlg::drawPicToHDC(UINT ID) {
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	if (ID == IDC_PicLeft) m_leftImage.DrawToHDC(hDC, &rect);
	if (ID == IDC_PicRight) m_rightImage.DrawToHDC(hDC, &rect);
	ReleaseDC(pDC);

}

void CFingerPrintRecognitionDlg::changeDlgMode(DlgMode m) {
	m_mode = m;
	INT_PTR md1 = SW_SHOW, md2 = SW_HIDE;
	if (m_mode == MODE_RECG) {
		md1 = SW_HIDE;
		md2 = SW_SHOW;
	}

	GetDlgItem(IDC_ExtractTrait)->ShowWindow(md1);
	GetDlgItem(IDC_SAVE)->ShowWindow(md1);

	GetDlgItem(IDC_Recognize)->ShowWindow(md2);
	GetDlgItem(IDC_ImportImg2)->ShowWindow(md2);
}

void CFingerPrintRecognitionDlg::OnBnClickedOpenImg1()
{
	// TODO:  在此添加控件通知处理程序代码
	IplImage *img = NULL;
	CFileDialog fdlg(TRUE, L".BMP");
	INT_PTR rs = fdlg.DoModal();
	if (rs == IDOK) {
		CString path = fdlg.GetPathName();
		CT2A cvt(path);
		img = cvLoadImage(cvt.m_psz);
		if (img == NULL) {
			MessageBox(L"load image eror");
			return;
		}
		m_leftImage.CopyOf(img);
		cvReleaseImage(&img);
		drawPicToHDC(IDC_PicLeft);
	}
}

void CFingerPrintRecognitionDlg::OnBnClickedOpenImg2() {
	// TODO:  在此添加控件通知处理程序代码
	IplImage *img = NULL;
	CFileDialog fdlg(TRUE, L".BMP");
	INT_PTR rs = fdlg.DoModal();
	if (rs == IDOK) {
		CString path = fdlg.GetPathName();
		CT2A cvt(path);
		img = cvLoadImage(cvt.m_psz);
		if (img == NULL) {
			MessageBox(L"load image eror");
			return;
		}
		m_rightImage.CopyOf(img);
		cvReleaseImage(&img);
		drawPicToHDC(IDC_PicRight);
	}
}

void CFingerPrintRecognitionDlg::OnBnMenuAdd() {
	//MessageBox(L"菜单项：录入->添加");

	InputDlg inputDlg = InputDlg(IDD_DIALOG_Input, GetDlgItem(IDD_FINGERPRINTRECOGNITION_DIALOG));
	INT_PTR nResponse = inputDlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  在此放置处理何时用
		//  “确定”来关闭对话框的代码
		MessageBox(L"get text:" + inputDlg.m_text);
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	//change to add mode
	changeDlgMode(MODE_ADD);
}

void CFingerPrintRecognitionDlg::OnBnMenuNew() {
	//MessageBox(L"菜单项：录入->新建");

	InputDlg inputDlg = InputDlg(IDD_DIALOG_Input, GetDlgItem(IDD_FINGERPRINTRECOGNITION_DIALOG));
	INT_PTR nResponse = inputDlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  在此放置处理何时用
		//  “确定”来关闭对话框的代码
		MessageBox(L"get text:" + inputDlg.m_text);
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	//change to add mode
	changeDlgMode(MODE_NEW);
}

void CFingerPrintRecognitionDlg::OnBnMenuRecognize() {
	MessageBox(L"菜单项：识别");

	//change to add mode
	changeDlgMode(MODE_RECG);
}

void CFingerPrintRecognitionDlg::OnBnClickedRecognize() {

}