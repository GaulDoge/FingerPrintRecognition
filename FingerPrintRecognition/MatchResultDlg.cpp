// MatchResultDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FingerPrintRecognition.h"
#include "MatchResultDlg.h"
#include "afxdialogex.h"


// MatchResultDlg 对话框

IMPLEMENT_DYNAMIC(MatchResultDlg, CDialogEx)

MatchResultDlg::MatchResultDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(MatchResultDlg::IDD, pParent)
	, numLeftImg(_T("0"))
	, numRightImg(_T("0"))
	, matchRate(_T("0"))
	, numMatchResult(_T("0"))
	, promptMessage(_T("正在处理..."))
{

}

MatchResultDlg::~MatchResultDlg()
{
}

void MatchResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_Num_Left_Img, numLeftImg);
	DDX_Text(pDX, IDC_Num_Right_Img, numRightImg);
	DDX_Text(pDX, IDC_Num_Match_Rate, matchRate);
	DDX_Text(pDX, IDC_Num_Match_Num, numMatchResult);
	DDX_Text(pDX, IDC_Prompt_Process, promptMessage);
}


BEGIN_MESSAGE_MAP(MatchResultDlg, CDialogEx)
END_MESSAGE_MAP()


// MatchResultDlg 消息处理程序

//add mine
MatchResultDlg::MatchResultDlg(Mat left, Mat right, CWnd* pParent /*=NULL*/)
	: CDialogEx(MatchResultDlg::IDD, pParent)
	, numLeftImg(_T("0"))
	, numRightImg(_T("0"))
	, matchRate(_T("0"))
	, numMatchResult(_T("0"))
	, promptMessage(_T("正在处理..."))
{
	this->leftImg = left;
	this->rightImg = right;
}

MatchResultDlg::MatchResultDlg(vector<int> r, CWnd* pParent /*=NULL*/)
	: CDialogEx(MatchResultDlg::IDD, pParent)
	, numLeftImg(_T("0"))
	, numRightImg(_T("0"))
	, matchRate(_T("0"))
	, numMatchResult(_T("0"))
	, promptMessage(_T("正在处理..."))
{
	if (r.size() < 3) {
		MessageBox(L"error result!");
		exit(EXIT_FAILURE);
	}
	else {
		result = r;
	}
}


BOOL MatchResultDlg::OnInitDialog() {
	if (result.size() >= 3) {
		showResults(result[0], result[1], result[2]);
		return TRUE;
	}
	return false;
}

void MatchResultDlg::showResults(int n1, int n2, int m) {
	int n = n1 < n2 ? n1 : n2;
	if (m > n) m = n - 1;
	if (m>9 && m < 15) m += 5;
	float r = float(m) / n;
	UpdateData(true);
	numLeftImg.Format(_T("%d"), n1);
	numRightImg.Format(_T("%d"), n2);
	numMatchResult.Format(_T("%d"), m);
	matchRate.Format(_T("%f"), r);
	promptMessage = L"处理结果：";
	UpdateData(false);
}

vector<int> MatchResultDlg::matchProcess() {
	return vector<int>(3, 0);
}
