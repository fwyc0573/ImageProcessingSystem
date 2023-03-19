
// MFCpictryDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CMFCpictryDlg 对话框
class CMFCpictryDlg : public CDialogEx
{
// 构造
public:
	CMFCpictryDlg(CWnd* pParent = NULL);	// 标准构造函数
	CMenu m_Menu;

// 对话框数据
	enum { IDD = IDD_MFCPICTRY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
