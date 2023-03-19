
// MFCpictryDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMFCpictryDlg �Ի���
class CMFCpictryDlg : public CDialogEx
{
// ����
public:
	CMFCpictryDlg(CWnd* pParent = NULL);	// ��׼���캯��
	CMenu m_Menu;

// �Ի�������
	enum { IDD = IDD_MFCPICTRY_DIALOG };

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
	CStatic m_PictureControl;
	afx_msg void OnBnClickedButton1();
	afx_msg void On32771();
	afx_msg void OnMeanShiftFilter_binary();
	afx_msg void OnBack();
	afx_msg void OnBinarization();
	afx_msg void OnDiedaithresh_Cut();
	afx_msg void OnGetGrayObj();
	afx_msg void OnSavepic();
	afx_msg void OnAverageFilter();
	afx_msg void OnMiddleFilter();
	afx_msg void OnCoverPic();
	int m_cover;
	CSliderCtrl x_control;
	int x_value;
	CSliderCtrl y_control;
	int y_value;
	afx_msg void OnWaitTosolve();
	afx_msg void OnCamSetup();
	afx_msg void OnCamCrenncut();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheck1();
	float size_value;
	CEdit size_control;
	CSliderCtrl angle_control;
	afx_msg void OnDetectPixel();
	afx_msg void OnCloseDetect();
	afx_msg void OnBnClickedCheck2();
};
