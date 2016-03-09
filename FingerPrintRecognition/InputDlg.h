#pragma once


// InputDlg 对话框

class InputDlg : public CDialog
{
	DECLARE_DYNAMIC(InputDlg)

public:
	InputDlg(CWnd* pParent = NULL);   // 标准构造函数
	InputDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~InputDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_Input };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	CString m_text;
};
