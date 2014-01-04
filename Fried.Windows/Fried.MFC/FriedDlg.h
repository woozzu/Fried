
// FriedDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "../../Fried.Core/Fried.h"
#include "Libs/sqlite3/CppSQLite3U.h"
#include "C2dSliderCtrl.h"

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


private:
	MachineLearner m_ml;
	CppSQLite3DB m_db;
	int m_baseTypeCur;
	int m_baseTypePos[2];
	CImageList m_imageList;
	CImageList m_searchImageList;
	int m_numPredictions;
	CBrush* m_pOrangeBrush;
	CBrush* m_pWhiteBrush;
	CFont* m_pSemilight18;
	CFont* m_pSemibold18;
	CFont* m_pRegular14;
	CFont* m_pRegular12;

// �����Դϴ�.
protected:
	HICON m_hIcon;

	void AddImage(LPCTSTR filename, CImageList &imageList, CListCtrl &listImage);
	void ShowImage(LPCTSTR filename);

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnThreadMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On2dSliderMove(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedBrowse();
	CEdit m_imagePath;
	CStatic m_status;
	afx_msg void OnLbnSelchangeImageList();
	CStatic m_imageThumb;
	afx_msg void OnBnClickedSearch();
	CStatic m_baseType1;
	CStatic m_baseType2;
	CStatic m_xaxisValue;
	CStatic m_yaxisValue;
	afx_msg void OnNMClickListImage(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CListCtrl m_listImage;
	CListCtrl m_listBaseType;
	CComboBox m_comboPresets;
	afx_msg void OnCbnSelchangeComboPreset();
	afx_msg void OnNMClickListBasetype(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListSearchImage(NMHDR *pNMHDR, LRESULT *pResult);
	CListCtrl m_listSearchImage;
	afx_msg void OnBnClickedBtnReset();
	C2dSliderCtrl m_slider;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CStatic m_imageName;
	CStatic m_staticImagePath;
	afx_msg void OnBnClickedBtnOpenFile();
	afx_msg void OnNMDblclkListBasetype(NMHDR *pNMHDR, LRESULT *pResult);
};
