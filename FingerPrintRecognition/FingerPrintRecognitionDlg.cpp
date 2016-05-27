
// FingerPrintRecognitionDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FingerPrintRecognition.h"
#include "FingerPrintRecognitionDlg.h"
#include "afxdialogex.h"

// add mine
#include "core.h"
#include "MatchResultDlg.h"
#include <opencv2/opencv.hpp>
#include <fstream>
using cv::Mat;

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
	ON_BN_CLICKED(IDC_Match, &CFingerPrintRecognitionDlg::OnBnClickedMatch)
	ON_BN_CLICKED(IDC_Recognize, &CFingerPrintRecognitionDlg::OnBnClickedRecognize)
	ON_BN_CLICKED(IDC_ExtractTrait, &CFingerPrintRecognitionDlg::OnBnClickedExtractTrait)
	ON_BN_CLICKED(IDC_SAVE, &CFingerPrintRecognitionDlg::OnBnClickedSave)
	ON_BN_CLICKED(ID_MenuAdd, &CFingerPrintRecognitionDlg::OnBnMenuAdd)
	ON_BN_CLICKED(ID_MenuRecognize, &CFingerPrintRecognitionDlg::OnBnMenuRecognize)
	ON_BN_CLICKED(ID_MenuMatch, &CFingerPrintRecognitionDlg::OnBnMenuMatch)
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
static Mat leftImage(480, 480, CV_8UC1);
static Mat rightImage(480, 480, CV_8UC1);
static bool leftState = false;
static bool rightState = false;
extern int extraitState;

void CFingerPrintRecognitionDlg::showImgInHDC(UINT ID, const Mat img) {
	CvvImage image;
	image.CopyOf(&IplImage(img));

	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);

	image.DrawToHDC(hDC, &rect);

	ReleaseDC(pDC);
}

/*void CFingerPrintRecognitionDlg::drawPicToHDC(UINT ID) {
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	if (ID == IDC_PicLeft) m_leftImage.DrawToHDC(hDC, &rect);
	if (ID == IDC_PicRight) m_rightImage.DrawToHDC(hDC, &rect);
	ReleaseDC(pDC);

}*/

void CFingerPrintRecognitionDlg::changeDlgMode(DlgMode m) {
	this->m_mode = m;
	leftImage = 100;
	rightImage = 100;
	leftState = false;
	rightState = false;
	extraitState = 0;//将提取阶段值为0
	showImgInHDC(IDC_PicLeft, leftImage);
	showImgInHDC(IDC_PicRight, rightImage);

	if (m == MODE_ADD) {
		GetDlgItem(IDC_ImportImg1)->ShowWindow(TRUE);
		GetDlgItem(IDC_Recognize)->ShowWindow(FALSE);
		GetDlgItem(IDC_ExtractTrait)->ShowWindow(TRUE);
		GetDlgItem(IDC_Match)->ShowWindow(FALSE);
		GetDlgItem(IDC_SAVE)->ShowWindow(TRUE);
		GetDlgItem(IDC_ImportImg2)->ShowWindow(FALSE);
	}
	else if (m == MODE_MATCH) {
		GetDlgItem(IDC_ImportImg1)->ShowWindow(TRUE);
		GetDlgItem(IDC_Recognize)->ShowWindow(FALSE);
		GetDlgItem(IDC_ExtractTrait)->ShowWindow(FALSE);
		GetDlgItem(IDC_Match)->ShowWindow(TRUE);
		GetDlgItem(IDC_SAVE)->ShowWindow(FALSE);
		GetDlgItem(IDC_ImportImg2)->ShowWindow(TRUE);
	}
	else if (m == MODE_RECG) {
		GetDlgItem(IDC_ImportImg1)->ShowWindow(TRUE);
		GetDlgItem(IDC_Recognize)->ShowWindow(TRUE);
		GetDlgItem(IDC_ExtractTrait)->ShowWindow(FALSE);
		GetDlgItem(IDC_Match)->ShowWindow(FALSE);
		GetDlgItem(IDC_SAVE)->ShowWindow(FALSE);
		GetDlgItem(IDC_ImportImg2)->ShowWindow(FALSE);
	}
	else {
		MessageBox(_T("Unknown Error!"));
		exit(EXIT_FAILURE);
	}
	
	GetDlgItem(IDC_PicLeft)->ShowWindow(TRUE);
	GetDlgItem(IDC_PicRight)->ShowWindow(TRUE);
}

void CFingerPrintRecognitionDlg::OnBnClickedOpenImg1()
{
	// TODO:  在此添加控件通知处理程序代码
	//IplImage *img = NULL;
	CFileDialog fdlg(TRUE, L".BMP");
	INT_PTR rs = fdlg.DoModal();
	if (rs == IDOK) {
		//CString path = fdlg.GetPathName();
		CT2A cvt(fdlg.GetPathName());
		/*img = cvLoadImage(cvt.m_psz);
		if (img == NULL) {
			MessageBox(L"load image eror");
			return;
		}
		Mat temp = cv::cvarrToMat(img);
		temp = normalize(temp);
		*img = IplImage(temp);
		m_leftImage.CopyOf(img);
		//cvReleaseImage(&img);
		drawPicToHDC(IDC_PicLeft);*/
		Mat img = cv::imread(cvt.m_psz);
		if (img.empty()) {
			MessageBox(L"load image eror");
			return;
		}
		else {
			img = normalize(img);
			leftImage = img;
			leftState = true;
			showImgInHDC(IDC_PicLeft, img);
		}
	}
}

void CFingerPrintRecognitionDlg::OnBnClickedOpenImg2() {
	// TODO:  在此添加控件通知处理程序代码
	//IplImage *img = NULL;
	CFileDialog fdlg(TRUE, L".BMP");
	INT_PTR rs = fdlg.DoModal();
	if (rs == IDOK) {
		CString path = fdlg.GetPathName();
		CT2A cvt(path);
		/*img = cvLoadImage(cvt.m_psz);
		if (img == NULL) {
			MessageBox(L"load image eror");
			return;
		}
		Mat temp = cv::cvarrToMat(img);
		temp = normalize(temp);
		*img = IplImage(temp);
		m_rightImage.CopyOf(img);
		//cvReleaseImage(&img);
		//drawPicToHDC(IDC_PicRight);*/
		Mat img = cv::imread(cvt.m_psz);
		if (img.empty()) {
			MessageBox(L"load image eror");
			return;
		}
		else {
			img = normalize(img);
			rightImage = img;
			rightState = true;
			showImgInHDC(IDC_PicRight, img);
		}
	}
}

void CFingerPrintRecognitionDlg::OnBnMenuAdd() {
	//MessageBox(L"菜单项：录入->添加");

	/*InputDlg inputDlg = InputDlg(IDD_DIALOG_Input, GetDlgItem(IDD_FINGERPRINTRECOGNITION_DIALOG));
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
	}*/

	//change to add mode
	changeDlgMode(MODE_ADD);
}

void CFingerPrintRecognitionDlg::OnBnMenuRecognize() {
	//MessageBox(L"菜单项：识别");

	//change to recognize mode
	changeDlgMode(MODE_RECG);
}

void CFingerPrintRecognitionDlg::OnBnMenuMatch() {
	//MessageBox(L"on menu match");
	changeDlgMode(MODE_MATCH);
}

void CFingerPrintRecognitionDlg::OnBnClickedMatch() {
	if (!(leftState&&rightState)) {
		MessageBox(_T("请先选择指纹图像！"));
	}
	else {
		MessageBox(L"begin");
		Mat leftOri, rightOri;
		Mat leftImg, rightImg;
		leftImg = leftImage.clone();
		rightImg = rightImage.clone();
		preprocess(leftImg, leftOri);
		preprocess(rightImg, rightOri);
		Mat leftShow = showTraitPoints(leftImg, leftOri);
		Mat rightShow = showTraitPoints(rightImg, rightOri);
		showImgInHDC(IDC_PicLeft, leftShow);
		showImgInHDC(IDC_PicRight, rightShow);
		MessageBox(L"done!");
		MatchResultDlg result(
			match(leftImg, leftOri, rightImg, rightOri),
			GetDlgItem(IDD_FINGERPRINTRECOGNITION_DIALOG));
		result.DoModal();
		leftState = rightState = false;
	}
}

vector<TraitPoint> tpVec;
vector<LocalTrait> ltVec;
bool isSaveable = false;
int extraitState = 0;

void CFingerPrintRecognitionDlg::OnBnClickedExtractTrait() {
	//MessageBox(L"clicked extract trait");
	if (!leftState) {
		MessageBox(_T("请选择指纹图像！"));
	}
	else {
		static Mat extraitImage;
		static Mat extraitImgOri;
		switch (extraitState) {
		case 0://滤波
			isSaveable = false;
			extraitImage = leftImage.clone();
			extraitImgOri = directionalOrientation(extraitImage);
			toBlockOrientation(extraitImgOri);
			extraitImage = directionalFilter(extraitImage, extraitImgOri);
			showImgInHDC(IDC_PicRight, extraitImage);
			//showImgInHDC(IDC_PicRight, extraitImgOri*30);
			extraitState += 1;
			break;
		case 1://二值化
			thresholdImage(extraitImage);
			showImgInHDC(IDC_PicRight, extraitImage);
			extraitState += 1;
			break;
		case 2://细化
			thinningImage(extraitImage);
			optimizeThinnedImage(extraitImage);
			showImgInHDC(IDC_PicRight, extraitImage);
			extraitState += 1;
			break;
		case 3://删除噪声
			deletePointAndShortLine(extraitImage);
			showImgInHDC(IDC_PicRight, extraitImage);
			extraitState += 1;
			break;
		case 4://显示特征点 ;
			showImgInHDC(IDC_PicRight, showTraitPoints(extraitImage));
			extraitState += 1;
			break;
		case 5://消除伪特征点
			tpVec = getTraitPoints(extraitImage, extraitImgOri);
			ltVec = getLocalTrait(extraitImage, tpVec);
			extraitImage = showTraitPoints(extraitImage, extraitImgOri);
			showImgInHDC(IDC_PicRight, extraitImage);
			extraitState += 1;
			break;
		default:
			extraitState = 0;
			leftState = false;
			isSaveable = true;
			MessageBox(_T("特征提取完成！"));
		}
	}
}

void CFingerPrintRecognitionDlg::OnBnClickedSave() {
	//MessageBox(L"clicked save");
	//CString str;
	//str.Format(_T("%d, %d"), tpVec.size(), ltVec.size());
	//MessageBox(str);
	if (isSaveable) {
		std::ofstream out;
		out.open("fingerprint.db", std::ofstream::out | std::ofstream::app | std::ofstream::binary);
		bool rst = writeTraits(out, tpVec, ltVec);
		out.close();
		if (rst) MessageBox(_T("保存成功！"));
		else MessageBox(_T("保存失败！"));
	}
}

void CFingerPrintRecognitionDlg::OnBnClickedRecognize() {
	//MessageBox(L"clicked recognize");
	if (!leftState) {
		MessageBox(_T("请选择指纹图像！"));
	}
	else {
		Mat ori;
		Mat nor = leftImage;
		preprocess(nor, ori);
		Mat showImg = showTraitPoints(nor, ori);
		showImgInHDC(IDC_PicRight, showImg);
		vector<TraitPoint> va = getTraitPoints(nor, ori);
		vector<LocalTrait> avec = getLocalTrait(nor, va);
		//CString str;
		//str.Format(_T("%d, %d"), va.size(), avec.size());
		//MessageBox(str);

		std::ifstream in;
		in.open("fingerprint.db", std::ifstream::in | std::ifstream::binary);

		vector<int> rstVec(3, 0);
		while (in) {
			vector<int> tmp = recognize(in, va, avec);
			if (tmp.size() >= 3) {
				if (rstVec[2] < tmp[2])
					rstVec = tmp;
			}
		}

		MatchResultDlg result(
			rstVec,
			GetDlgItem(IDD_FINGERPRINTRECOGNITION_DIALOG));
		result.DoModal();
		in.close();
		leftState = false;
	}
}
