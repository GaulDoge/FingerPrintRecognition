// InputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FingerPrintRecognition.h"
#include "InputDlg.h"
#include "afxdialogex.h"


// InputDlg 对话框

IMPLEMENT_DYNAMIC(InputDlg, CDialog)

InputDlg::InputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(InputDlg::IDD, pParent)
{
	m_text = "";
}

InputDlg::InputDlg(UINT nIDTemplate, CWnd* pParentWnd /*= NULL*/)
	: CDialog(nIDTemplate, pParentWnd)
{
	m_text = "";
}

InputDlg::~InputDlg()
{
}


void InputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX); 
	DDX_Text(pDX, IDC_InputText, m_text);
}


BEGIN_MESSAGE_MAP(InputDlg, CDialog)
END_MESSAGE_MAP()


// InputDlg 消息处理程序
