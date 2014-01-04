
// FriedDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Fried.h"
#include "FriedDlg.h"
#include "afxdialogex.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

UINT LoadData(LPVOID pParam);
UINT SearchFilesFunc(LPVOID pParam);
void FindFile(LPCTSTR dir, HWND *hWnd);

UINT Scan(LPVOID pParam);

typedef struct _loadDataThreadParam {
	HWND *hWnd;
	MachineLearner *ml;
	CppSQLite3DB *db;
} LoadDataThreadParam;

typedef struct _searchThreadParam {
	HWND *hWnd;
	CString dir;
} SearchThreadParam;

typedef struct _scanThreadParam {
	HWND *hWnd;
	MachineLearner *ml;
	CppSQLite3DB *db;
	vector<CString> imageList;
	int nPredictions;
} ScanThreadParam;

// CFriedDlg 대화 상자



CFriedDlg::CFriedDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFriedDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pWhiteBrush = new CBrush(RGB(255, 255, 255));
	m_pOrangeBrush = new CBrush(RGB(247, 154, 6));
	m_pSemilight18 = new CFont();
	m_pSemilight18->CreatePointFont((int)(18 * 7.5), _T("Segoe UI Semilight"));
	m_pSemibold18 = new CFont();
	m_pSemibold18->CreatePointFont((int)(18 * 7.5), _T("Segoe UI Semibold"));
	m_pRegular14 = new CFont();
	m_pRegular14->CreatePointFont((int)(14 * 7.5), _T("Segoe UI"));
	m_pRegular12 = new CFont();
	m_pRegular12->CreatePointFont((int)(12 * 7.5), _T("Segoe UI"));
}

void CFriedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_PATH, m_imagePath);
	DDX_Control(pDX, IDC_STATE, m_status);
	DDX_Control(pDX, IDC_IMAGE, m_imageThumb);
	DDX_Control(pDX, IDC_BASETYPE_1, m_baseType1);
	DDX_Control(pDX, IDC_BASETYPE_2, m_baseType2);
	DDX_Control(pDX, IDC_XAXIS_V, m_xaxisValue);
	DDX_Control(pDX, IDC_YAXIS_V, m_yaxisValue);
	DDX_Control(pDX, IDC_LIST_IMAGE, m_listImage);
	DDX_Control(pDX, IDC_LIST_BASETYPE, m_listBaseType);
	DDX_Control(pDX, IDC_COMBO_PRESET, m_comboPresets);
	DDX_Control(pDX, IDC_LIST_SEARCH_IMAGE, m_listSearchImage);
	DDX_Control(pDX, IDC_SLIDER, m_slider);
	DDX_Control(pDX, IDC_STATIC_IMAGE_NAME, m_imageName);
	DDX_Control(pDX, IDC_EDIT_IMAGE_PATH, m_staticImagePath);
}

BEGIN_MESSAGE_MAP(CFriedDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_USER + 1, &CFriedDlg::OnThreadMessage)
	ON_BN_CLICKED(IDC_SEARCH, &CFriedDlg::OnBnClickedSearch)
	ON_NOTIFY(NM_CLICK, IDC_LIST_IMAGE, &CFriedDlg::OnNMClickListImage)
	ON_WM_DROPFILES()
	ON_CBN_SELCHANGE(IDC_COMBO_PRESET, &CFriedDlg::OnCbnSelchangeComboPreset)
	ON_NOTIFY(NM_CLICK, IDC_LIST_BASETYPE, &CFriedDlg::OnNMClickListBasetype)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SEARCH_IMAGE, &CFriedDlg::OnNMClickListSearchImage)
	ON_BN_CLICKED(IDC_BTN_RESET, &CFriedDlg::OnBnClickedBtnReset)
	ON_MESSAGE(WM_2D_SLIDER_MOVE, &CFriedDlg::On2dSliderMove)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_OPEN_FILE, &CFriedDlg::OnBnClickedBtnOpenFile)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BASETYPE, &CFriedDlg::OnNMDblclkListBasetype)
END_MESSAGE_MAP()


// CFriedDlg 메시지 처리기

BOOL CFriedDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_baseTypeCur = 0;
	m_baseTypePos[0] = m_baseTypePos[1] = 0;

	GetDlgItem(IDC_STATIC_LABEL_TOP)->SetFont(m_pSemilight18);
	GetDlgItem(IDC_STATIC_FOCUSON)->SetFont(m_pSemilight18);
	GetDlgItem(IDC_STATIC_BASETYPES)->SetFont(m_pSemilight18);
	GetDlgItem(IDC_STATIC_PRESETS)->SetFont(m_pSemilight18);
	GetDlgItem(IDC_IMAGE_PATH)->SetFont(m_pRegular14);
	GetDlgItem(IDC_STATIC_IMAGE_NAME)->SetFont(m_pSemibold18);
	GetDlgItem(IDC_EDIT_IMAGE_PATH)->SetFont(m_pRegular12);

	m_imagePath.SetWindowTextW(_T("Open Directory"));
	m_imagePath.SetMargins(5, 5);

	m_imageList.Create(110, 110, ILC_COLOR32, 4, 1);
	m_searchImageList.Create(110, 110, ILC_COLOR32, 4, 1);
	m_listImage.SetImageList(&m_imageList, LVSIL_NORMAL);
	m_listSearchImage.SetImageList(&m_searchImageList, LVSIL_NORMAL);
	m_listSearchImage.ShowWindow(SW_HIDE);
	m_listImage.SetIconSpacing(130, 150);
	m_listSearchImage.SetIconSpacing(130, 150);

	m_listBaseType.SetExtendedStyle(LVS_EX_CHECKBOXES);
	m_listBaseType.InsertColumn(0, _T(""), LVCFMT_LEFT, 24);
	m_listBaseType.InsertColumn(1, _T("BaseType"), LVCFMT_LEFT, 96);
	m_listBaseType.InsertColumn(2, _T("%"), LVCFMT_LEFT, 45);

	m_status.SetWindowTextW(_T("Loading data..."));
	LoadDataThreadParam* ldtp = new LoadDataThreadParam;
	ldtp->hWnd = &m_hWnd;
	ldtp->ml = &m_ml;
	ldtp->db = &m_db;
	AfxBeginThread(LoadData, (LPVOID)ldtp);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFriedDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFriedDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CFriedDlg::OnThreadMessage(WPARAM wParam, LPARAM lParam)
{
	CppSQLite3Query q;
	static vector<CString> imageList;
	switch (wParam) {
	case 0:
		// Initialize
		q = m_db.execQuery(_T("select * from base_types;"));
		while (!q.eof()) {
			m_listBaseType.InsertItem(m_listBaseType.GetItemCount(), _T(""));
			m_listBaseType.SetItem(m_listBaseType.GetItemCount() - 1, 1, LVIF_TEXT, q.fieldValue(0), 0, 0, 0, NULL);
			q.nextRow();
		}
		
		m_comboPresets.AddString(_T("none"));
		q = m_db.execQuery(_T("select * from presets;"));
		while (!q.eof()) {
			m_comboPresets.AddString(q.fieldValue(0));
			q.nextRow();
		}
		m_comboPresets.SetCurSel(0);

		q = m_db.execQuery(_T("select * from scenes;"));
		while (!q.eof()) {
			AddImage(q.fieldValue(0), m_imageList, m_listImage);
			q.nextRow();
		}

		q = m_db.execQuery(_T("select count(rowid) from base_types;"));
		if (!q.eof()) {
			m_numPredictions = q.fieldValueInt(0);
		}

		m_status.SetWindowTextW(_T("Done."));
		break;
	case 1:
		// Searching
		if (lParam == NULL) {
			m_status.SetWindowTextW(_T("Done."));

			ScanThreadParam* stp = new ScanThreadParam;
			stp->hWnd = &m_hWnd;
			stp->db = &m_db;
			stp->ml = &m_ml;
			stp->nPredictions = m_numPredictions;
			for (vector<CString>::iterator iter = imageList.begin(); iter != imageList.end(); iter++)
				stp->imageList.push_back(*iter);
			m_status.SetWindowTextW(_T("Scanning..."));
			AfxBeginThread(Scan, (LPVOID)stp);

			imageList.clear();
		} else {
			CString* filePath = (CString*)lParam;
			imageList.push_back(*filePath);
			AddImage(*filePath, m_imageList, m_listImage);
		}
		break;
	case 2:
		// Scan
		if (lParam == NULL) {
			m_status.SetWindowTextW(_T("Done."));
		} else {
			CString msg;
			msg.Format(_T("Analyzing: %s"), *((CString*)lParam));
			m_status.SetWindowTextW(msg);
		}
		break;
	}
	return 0;
}

UINT LoadData(LPVOID pParam)
{
	LoadDataThreadParam* ldtp = (LoadDataThreadParam*)pParam;
	ldtp->ml->Load("..\\fried_svm.dat");
	ldtp->db->open(_T("..\\fried.db"));
	SendMessage(*(ldtp->hWnd), WM_USER + 1, 0, 0);
	delete ldtp;
	return 0;
}

UINT SearchFilesFunc(LPVOID pParam)
{
	SearchThreadParam* stp = (SearchThreadParam*)pParam;
	FindFile(stp->dir, stp->hWnd);
	SendMessage(*(stp->hWnd), WM_USER + 1, 1, 0);
	delete stp;
	return 0;
}

void FindFile(LPCTSTR dir, HWND *hWnd)
{
	CFileFind fileFind;
	CString dirPath;
	dirPath.Format(_T("%s\\*.*"), dir);
	BOOL found = fileFind.FindFile(dirPath);
	while (found) {
		found = fileFind.FindNextFileW();
		if (fileFind.IsDots())
			continue;
		if (fileFind.IsDirectory()) {
			FindFile(fileFind.GetFilePath(), hWnd);
		} else {
			CString filePath = fileFind.GetFilePath();
			SendMessage(*hWnd, WM_USER + 1, 1, (LPARAM)&filePath);
		}
	}
}

UINT Scan(LPVOID pParam)
{
	ScanThreadParam* stp = (ScanThreadParam*)pParam;

	try {
		Mat segments[100];
		for (unsigned int i = 0; i < stp->imageList.size(); i++) {
			CString path = stp->imageList.at(i);
			SendMessage(*(stp->hWnd), WM_USER + 1, 2, (LPARAM)&path);

			int *predictions = new int[stp->nPredictions + 1];
			for (int j = 1; j <= stp->nPredictions; j++)
				predictions[j] = 0;

			ImageSegmenter::Segment((CStringA)path, segments, 10, 10);
			for (int j = 0; j < 100; j++) {
				Mat feature;
				FeatureExtractor::GetFeatures_linHSVhist_hog(segments[j], feature, 24, 26, 4);
				int p = (int)(stp->ml->Predict(feature));
				predictions[p]++;
			}

			CString query;
			query.Format(_T("select rowid from scenes where image_path='%s';"), path);
			CppSQLite3Query q = stp->db->execQuery(query);
			if (q.eof()) {
				query.Format(_T("insert into scenes values ('%s');"), path);
				stp->db->execDML(query);

				query.Format(_T("select rowid from scenes where image_path='%s';"), path);
				q = stp->db->execQuery(query);
			}

			int rowid = 0;
			if (!q.eof())
				rowid = q.fieldValueInt(0);
			for (int j = 1; j <= stp->nPredictions; j++) {
				query.Format(_T("select rowid from classifications where of_base_type_id=%d and of_scene_id=%d;"), j, rowid);
				q = stp->db->execQuery(query);
				if (q.eof()) {
					query.Format(_T("insert into classifications values (%d, %d, '%d');"), j, rowid, predictions[j]);
					stp->db->execDML(query);
				} else {
					query.Format(_T("update classifications set prop='%d' where rowid=%d;"), predictions[j], q.fieldValueInt(0));
				}
			}

			delete []predictions;
		}

		SendMessage(*(stp->hWnd), WM_USER + 1, 2, 0);
		delete stp;
	} catch (exception e) {
		//AfxMessageBox(_T("An error occurred."));
	}
	return 0;
}

void CFriedDlg::OnBnClickedSearch()
{
	CString baseType1, baseType2;
	m_baseType1.GetWindowTextW(baseType1);
	m_baseType2.GetWindowTextW(baseType2);
	int x = m_slider.GetXPos();
	int y = m_slider.GetYPos();

	m_listSearchImage.ShowWindow(SW_SHOW);
	m_listImage.ShowWindow(SW_HIDE);
	m_listSearchImage.DeleteAllItems();
	while (m_searchImageList.GetImageCount() > 0)
		m_searchImageList.Remove(0);

	CString query;
	query.Format(_T("select * from scenes where rowid in (select of_scene_id from classifications where of_base_type_id=(select rowid from base_types where label='%s') and (prop between %d and %d)) and rowid in (select of_scene_id from classifications where of_base_type_id=(select rowid from base_types where label='%s') and (prop between %d and %d));"), baseType1, x - 10, x + 10, baseType2, y - 10, y + 10);
	CppSQLite3Query q = m_db.execQuery(query);
	while (!q.eof()) {
		AddImage(q.fieldValue(0), m_searchImageList, m_listSearchImage);
		q.nextRow();
	}
}

void CFriedDlg::OnNMClickListImage(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	int pos = pNMItemActivate->iItem;
	if (pos >= 0 && pos < m_listImage.GetItemCount()) {
		CString fileName = m_listImage.GetItemText(pos, 0);
		ShowImage(fileName);
	}

	*pResult = 0;
}

void CFriedDlg::OnDropFiles(HDROP hDropInfo)
{
	ScanThreadParam* stp = new ScanThreadParam;
	stp->hWnd = &m_hWnd;
	stp->db = &m_db;
	stp->ml = &m_ml;
	stp->nPredictions = m_numPredictions;

	int count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	WCHAR lpszFile[1025];
	for (int i = 0; i < count; i++) {
		DragQueryFile(hDropInfo, i, lpszFile, 1024);
		stp->imageList.push_back(lpszFile);
		AddImage(lpszFile, m_imageList, m_listImage);
	}
	DragFinish(hDropInfo);

	m_listImage.SetItemState(m_listImage.GetItemCount() - 1, LVIS_SELECTED, LVIS_SELECTED);
	m_listImage.SetSelectionMark(m_listImage.GetItemCount() - 1);
	m_listImage.SetFocus();
	ShowImage(stp->imageList.at(stp->imageList.size() - 1));
	m_status.SetWindowTextW(_T("Scanning..."));
	AfxBeginThread(Scan, (LPVOID)stp);

	CDialogEx::OnDropFiles(hDropInfo);
}

void CFriedDlg::AddImage(LPCTSTR filename, CImageList &imageList, CListCtrl &listImage)
{
	for (int i = 0; i < listImage.GetItemCount(); i++) {
		if (listImage.GetItemText(i, 0).Compare(filename) == 0)
			return;
	}

	CImage image, thumb;
	image.Load(filename);
	thumb.Create(110, 110, 32);

	CDC srcDC, destDC;
	srcDC.CreateCompatibleDC(this->GetDC());
	srcDC.SelectObject(CBitmap::FromHandle(image));
	destDC.Attach(thumb.GetDC());
	destDC.SetStretchBltMode(HALFTONE);
	destDC.StretchBlt(0, 0, 110, 110, &srcDC, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
	destDC.Detach();
	thumb.ReleaseDC();
	srcDC.DeleteDC();
	destDC.DeleteDC();

	imageList.Add(CBitmap::FromHandle(thumb), RGB(255, 255, 255));
	listImage.InsertItem(listImage.GetItemCount(), filename, listImage.GetItemCount());
}

void CFriedDlg::OnCbnSelchangeComboPreset()
{
	int pos = m_comboPresets.GetCurSel();
	if (pos == 0) {
		for (int i = 0; i < m_listBaseType.GetItemCount(); i++)
			m_listBaseType.SetCheck(i, false);
	} else {
		CString query, label;
		m_comboPresets.GetLBText(pos, label);
		query.Format(_T("select * from presets where label='%s';"), label);
		CppSQLite3Query q = m_db.execQuery(query);
		if (!q.eof()) {
			for (int i = 0; i < m_listBaseType.GetItemCount(); i++)
				m_listBaseType.SetCheck(i, false);

			for (int i = 1; i <= 2; i++) {
				query.Format(_T("select * from base_types where rowid=%d;"), q.fieldValueInt(i));
				CppSQLite3Query q1 = m_db.execQuery(query);
				if (!q1.eof()) {
					for (int j = 0; j < m_listBaseType.GetItemCount(); j++) {
						if (m_listBaseType.GetItemText(j, 1).Compare(q1.fieldValue(0)) == 0) {
							m_listBaseType.SetCheck(j);
							if (m_baseTypeCur == 0)
								m_baseType1.SetWindowTextW(m_listBaseType.GetItemText(j, 1));
							else
								m_baseType2.SetWindowTextW(m_listBaseType.GetItemText(j, 1));
							m_baseTypePos[m_baseTypeCur] = j;
							m_baseTypeCur ^= 1;

							int v = q.fieldValueInt(i + 2);
							CString value;
							value.Format(_T("%d%%"), v);
							if (i == 1) {
								m_slider.SetXPos(v);
								m_xaxisValue.SetWindowTextW(value);
							} else {
								m_slider.SetYPos(v);
								m_yaxisValue.SetWindowTextW(value);
							}
						}
					}
				}
			}
		}
	}
}

void CFriedDlg::OnNMClickListBasetype(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	LVHITTESTINFO hitTestInfo;
	hitTestInfo.pt = pNMItemActivate->ptAction;
	m_listBaseType.HitTest(&hitTestInfo);
	
	int result = 0;
	if (hitTestInfo.flags == LVHT_ONITEMSTATEICON) {
		int count = 0;
		for (int i = 0; i < m_listBaseType.GetItemCount(); i++) {
			if (m_listBaseType.GetCheck(i))
				count++;
		}

		BOOL checked = m_listBaseType.GetCheck(hitTestInfo.iItem);
		if (checked && count == 2) {
			result = 1;
		} else {
			if (!checked && count == 2)
				m_listBaseType.SetCheck(m_baseTypePos[m_baseTypeCur], false);
			if (m_baseTypeCur == 0)
				m_baseType1.SetWindowTextW(m_listBaseType.GetItemText(hitTestInfo.iItem, 1));
			else
				m_baseType2.SetWindowTextW(m_listBaseType.GetItemText(hitTestInfo.iItem, 1));
			m_baseTypePos[m_baseTypeCur] = hitTestInfo.iItem;
			m_baseTypeCur ^= 1;
		}
	}

	*pResult = result;
}


void CFriedDlg::OnNMDblclkListBasetype(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnNMClickListBasetype(pNMHDR, pResult);
}


void CFriedDlg::OnNMClickListSearchImage(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	
	int pos = pNMItemActivate->iItem;
	if (pos >= 0 && pos < m_listSearchImage.GetItemCount()) {
		CString fileName = m_listSearchImage.GetItemText(pos, 0);
		ShowImage(fileName);
	}

	*pResult = 0;
}

void CFriedDlg::OnBnClickedBtnReset()
{
	m_listSearchImage.ShowWindow(SW_HIDE);
	m_listImage.ShowWindow(SW_SHOW);
	m_listSearchImage.DeleteAllItems();
	while (m_searchImageList.GetImageCount() > 0)
		m_searchImageList.Remove(0);
	if (m_listImage.GetItemCount() > 0) {
		m_listImage.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
		m_listImage.SetSelectionMark(0);
		m_listImage.SetFocus();
		ShowImage(m_listImage.GetItemText(0, 0));
	}
}

void CFriedDlg::ShowImage(LPCTSTR filename)
{
	Invalidate();
	UpdateWindow();

	CString name = filename;
	m_imageName.SetWindowTextW(name.Mid(name.ReverseFind('\\') + 1));
	m_staticImagePath.SetWindowTextW(filename);

	CImage image;
	image.Load(filename);
	CDC *pDC = m_imageThumb.GetDC();
	pDC->SetStretchBltMode(HALFTONE);

	int width = (int)(400 * (image.GetWidth() / (float)image.GetHeight()));
	int height = (int)(400 * (image.GetHeight() / (float)image.GetWidth()));
	image.StretchBlt(pDC->m_hDC, 0, 0, (width > 400)? 400 : width, (height > 400)? 400 : height, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);

	try {
		CString query;
		query.Format(_T("select base_types.label, classifications.prop from classifications, base_types where classifications.of_scene_id=(select rowid from scenes where image_path='%s') and classifications.of_base_type_id=base_types.rowid;"), filename);
		CppSQLite3Query q = m_db.execQuery(query);
		while (!q.eof()) {
			for (int i = 0; i < m_listBaseType.GetItemCount(); i++) {
				if (m_listBaseType.GetItemText(i, 1).Compare(q.fieldValue(0)) == 0) {
					m_listBaseType.SetItem(i, 2, LVIF_TEXT, q.fieldValue(1), 0, 0, 0, NULL);
					break;
				}
			}
			q.nextRow();
		}
	} catch (CppSQLite3Exception e) {
	}
}

LRESULT CFriedDlg::On2dSliderMove(WPARAM wParam, LPARAM lParam)
{
	CPoint* pos = (CPoint*)lParam;
	CString x, y;
	x.Format(_T("%d%%"), pos->x);
	y.Format(_T("%d%%"), pos->y);
	m_xaxisValue.SetWindowTextW(x);
	m_yaxisValue.SetWindowTextW(y);
	return 0L;
}

HBRUSH CFriedDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (pWnd->GetDlgCtrlID()) {
	case IDD_FRIEDMFC_DIALOG:
		return (HBRUSH)(m_pWhiteBrush->GetSafeHandle());
	case IDC_STATIC_LABEL_TOP:
		pDC->SetBkColor(RGB(247, 154, 6));
		pDC->SetTextColor(RGB(255, 255, 255));
		return (HBRUSH)(m_pOrangeBrush->GetSafeHandle());
	case IDC_BG_TOP:
		return (HBRUSH)(m_pOrangeBrush->GetSafeHandle());
	case IDC_IMAGE_PATH:
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetTextColor(RGB(247, 154, 6));
		return (HBRUSH)(m_pWhiteBrush->GetSafeHandle());
	case IDC_STATIC_FOCUSON:
	case IDC_STATIC_BASETYPES:
	case IDC_STATIC_PRESETS:
		pDC->SetTextColor(RGB(247, 154, 6));
		return hbr;
	case IDC_STATIC_BG_RIGHT:
		return (HBRUSH)(m_pWhiteBrush->GetSafeHandle());
	case IDC_IMAGE:
		return (HBRUSH)(m_pWhiteBrush->GetSafeHandle());
	case IDC_STATIC_IMAGE_NAME:
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetTextColor(RGB(247, 154, 6));
		return (HBRUSH)(m_pWhiteBrush->GetSafeHandle());
	case IDC_EDIT_IMAGE_PATH:
		pDC->SetBkColor(RGB(255, 255, 255));
		pDC->SetTextColor(RGB(102, 102, 102));
		return (HBRUSH)(m_pWhiteBrush->GetSafeHandle());
	}

	if (nCtlColor == CTLCOLOR_STATIC) {
		pDC->SetTextColor(RGB(102, 102, 102));
	}
	return hbr;
}


void CFriedDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	delete m_pWhiteBrush;
	delete m_pOrangeBrush;
	delete m_pSemilight18;
	delete m_pSemibold18;
	delete m_pRegular12;
	delete m_pRegular14;
}


BOOL CFriedDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_imagePath.GetSafeHwnd() && pMsg->message == WM_LBUTTONUP) {
		BROWSEINFO bi; 
		ZeroMemory(&bi, sizeof(bi));
		TCHAR szDisplayName[MAX_PATH]; 
		szDisplayName[0] = ' '; 

		bi.hwndOwner = NULL;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = szDisplayName;
		bi.lpszTitle = _T("Please select a folder.");
		bi.ulFlags = BIF_RETURNONLYFSDIRS;
		bi.lParam = NULL;
		bi.iImage = 0;

		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		TCHAR szPathName[MAX_PATH];
		if (NULL != pidl)  {
			BOOL bRet = SHGetPathFromIDList(pidl, szPathName);
			if (bRet) {
				m_imagePath.SetWindowTextW(szPathName);
				m_status.SetWindowTextW(_T("Searching image files..."));
				SearchThreadParam *stp = new SearchThreadParam;
				stp->hWnd = &m_hWnd;
				stp->dir = szPathName;
				AfxBeginThread(SearchFilesFunc, (LPVOID)stp);
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CFriedDlg::OnBnClickedBtnOpenFile()
{
	TCHAR szFilters[]= _T("Image Files (*.bmp, *.jpg, *.png)|*.bmp;*.jpg;*.png|"); 
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT, szFilters, this);

	CString strFile;
	fileDlg.m_ofn.lpstrFile = strFile.GetBuffer(MAX_PATH * 100);
	fileDlg.m_ofn.nMaxFile = 100;
 
	if (fileDlg.DoModal() == IDOK) {
	ScanThreadParam* stp = new ScanThreadParam;
	stp->hWnd = &m_hWnd;
	stp->db = &m_db;
	stp->ml = &m_ml;
	stp->nPredictions = m_numPredictions;

	for (POSITION pos = fileDlg.GetStartPosition(); pos != NULL;) {
		CString pathName = fileDlg.GetNextPathName(pos);
		stp->imageList.push_back(pathName);
		AddImage(pathName, m_imageList, m_listImage);
	}

	m_listImage.SetItemState(m_listImage.GetItemCount() - 1, LVIS_SELECTED, LVIS_SELECTED);
	m_listImage.SetSelectionMark(m_listImage.GetItemCount() - 1);
	m_listImage.SetFocus();
	ShowImage(stp->imageList.at(stp->imageList.size() - 1));
	m_status.SetWindowTextW(_T("Scanning..."));
	AfxBeginThread(Scan, (LPVOID)stp);
	}

	strFile.ReleaseBuffer();
}