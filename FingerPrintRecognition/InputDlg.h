#pragma once


// InputDlg �Ի���

class InputDlg : public CDialog
{
	DECLARE_DYNAMIC(InputDlg)

public:
	InputDlg(CWnd* pParent = NULL);   // ��׼���캯��
	InputDlg(UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~InputDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_Input };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	CString m_text;
};
