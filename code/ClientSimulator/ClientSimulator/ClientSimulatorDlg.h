
// ClientSimulatorDlg.h : ͷ�ļ�
//

#pragma once


// CClientSimulatorDlg �Ի���
class CClientSimulatorDlg : public CDialogEx
{
// ����
public:
	CClientSimulatorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CLIENTSIMULATOR_DIALOG };

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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedAppSend();
	afx_msg void OnBnClickedCancel();
	CString m_strAccountMsg;
	CString m_strPassword;
	afx_msg void OnBnClickedWebSend();
};
