//add mine
#include<cv.h>
#include<highgui.h>
#include "CvvImage.h"


// FingerPrintRecognitionDlg.h : ͷ�ļ�
//

#pragma once


// CFingerPrintRecognitionDlg �Ի���
class CFingerPrintRecognitionDlg : public CDialogEx
{
// ����
public:
	CFingerPrintRecognitionDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FINGERPRINTRECOGNITION_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	//add mine
	afx_msg void OnBnClickedButton1();
	void drawPicToHDC(UINT ID);
	CvvImage leftImage;
	CvvImage rightImage;
};
