
// FriedDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "../../Fried.Core/Fried.h"
#include "Libs/sqlite3/CppSQLite3U.h"
#include "afxcmn.h"

// CFriedDlg ��ȭ ����
class CFriedDlg : public CDialogEx
{
// �����Դϴ�.
public:
	CFriedDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FRIEDMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnThreadMessage(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedBrowse();
	CEdit m_imagePath;
	CListBox m_imageList;
	CStatic m_status;
	afx_msg void OnLbnSelchangeImageList();
	CStatic m_imageThumb;

private:
	MachineLearner m_ml;
	CppSQLite3DB m_db;
	int m_baseTypeCur;
public:
	afx_msg void OnBnClickedScan();
	CSliderCtrl m_xaxis;
	CSliderCtrl m_yaxis;
	CListBox m_searchResultList;
	afx_msg void OnBnClickedSearch();
	CListBox m_baseTypeList;
	afx_msg void OnBnClickedSelect();
	CStatic m_baseType1;
	CStatic m_baseType2;
	afx_msg void OnLbnSelchangeSearchResult();
	afx_msg void OnNMReleasedcaptureXaxis(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_xaxisValue;
	CStatic m_yaxisValue;
	afx_msg void OnNMReleasedcaptureYaxis(NMHDR *pNMHDR, LRESULT *pResult);
};
