
// FingerPrintRecognitionDlg.cpp : ʵ���ļ�
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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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



// CFingerPrintRecognitionDlg �Ի���

CFingerPrintRecognitionDlg::CFingerPrintRecognitionDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFingerPrintRecognitionDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFingerPrintRecognitionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

//�¼���Ӧ����
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


// CFingerPrintRecognitionDlg ��Ϣ�������

BOOL CFingerPrintRecognitionDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	//add mine
	menu.LoadMenu(IDR_MENU1);
	SetMenu(&menu);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFingerPrintRecognitionDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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
	extraitState = 0;//����ȡ�׶�ֵΪ0
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
	//MessageBox(L"�˵��¼��->���");

	/*InputDlg inputDlg = InputDlg(IDD_DIALOG_Input, GetDlgItem(IDD_FINGERPRINTRECOGNITION_DIALOG));
	INT_PTR nResponse = inputDlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
		MessageBox(L"get text:" + inputDlg.m_text);
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO:  �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}*/

	//change to add mode
	changeDlgMode(MODE_ADD);
}

void CFingerPrintRecognitionDlg::OnBnMenuRecognize() {
	//MessageBox(L"�˵��ʶ��");

	//change to recognize mode
	changeDlgMode(MODE_RECG);
}

void CFingerPrintRecognitionDlg::OnBnMenuMatch() {
	//MessageBox(L"on menu match");
	changeDlgMode(MODE_MATCH);
}

void CFingerPrintRecognitionDlg::OnBnClickedMatch() {
	if (!(leftState&&rightState)) {
		MessageBox(_T("����ѡ��ָ��ͼ��"));
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
		MessageBox(_T("��ѡ��ָ��ͼ��"));
	}
	else {
		static Mat extraitImage;
		static Mat extraitImgOri;
		switch (extraitState) {
		case 0://�˲�
			isSaveable = false;
			extraitImage = leftImage.clone();
			extraitImgOri = directionalOrientation(extraitImage);
			toBlockOrientation(extraitImgOri);
			extraitImage = directionalFilter(extraitImage, extraitImgOri);
			showImgInHDC(IDC_PicRight, extraitImage);
			//showImgInHDC(IDC_PicRight, extraitImgOri*30);
			extraitState += 1;
			break;
		case 1://��ֵ��
			thresholdImage(extraitImage);
			showImgInHDC(IDC_PicRight, extraitImage);
			extraitState += 1;
			break;
		case 2://ϸ��
			thinningImage(extraitImage);
			optimizeThinnedImage(extraitImage);
			showImgInHDC(IDC_PicRight, extraitImage);
			extraitState += 1;
			break;
		case 3://ɾ������
			deletePointAndShortLine(extraitImage);
			showImgInHDC(IDC_PicRight, extraitImage);
			extraitState += 1;
			break;
		case 4://��ʾ������ ;
			showImgInHDC(IDC_PicRight, showTraitPoints(extraitImage));
			extraitState += 1;
			break;
		case 5://����α������
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
			MessageBox(_T("������ȡ��ɣ�"));
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
		if (rst) MessageBox(_T("����ɹ���"));
		else MessageBox(_T("����ʧ�ܣ�"));
	}
}

void CFingerPrintRecognitionDlg::OnBnClickedRecognize() {
	//MessageBox(L"clicked recognize");
	if (!leftState) {
		MessageBox(_T("��ѡ��ָ��ͼ��"));
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
