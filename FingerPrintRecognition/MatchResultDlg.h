#pragma once
//add mine
#include "core.h"
#include <opencv2/opencv.hpp>
#include <vector>

using cv::Mat;
using std::vector;

// MatchResultDlg �Ի���

class MatchResultDlg : public CDialogEx
{
	DECLARE_DYNAMIC(MatchResultDlg)

public:
	MatchResultDlg(CWnd* pParent = NULL);   // ��׼���캯��
	MatchResultDlg(Mat left, Mat right, CWnd* pParent = NULL);
	MatchResultDlg::MatchResultDlg(vector<int>, CWnd* pParent = NULL);
	virtual ~MatchResultDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_Result };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

//add mine
public:
	void showResults(int n1, int n2, int m);
	vector<int> matchProcess();
	Mat leftImg;
	Mat rightImg;
	vector<int> result;
	BOOL OnInitDialog();
	CString numLeftImg;
	CString numRightImg;
	CString matchRate;
	CString numMatchResult;
	CString promptMessage;
};
