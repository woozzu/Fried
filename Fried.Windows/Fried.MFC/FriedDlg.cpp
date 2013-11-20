
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
} ScanThreadParam;

// CFriedDlg 대화 상자



CFriedDlg::CFriedDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFriedDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFriedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_PATH, m_imagePath);
	DDX_Control(pDX, IDC_IMAGE_LIST, m_imageList);
	DDX_Control(pDX, IDC_STATE, m_status);
	DDX_Control(pDX, IDC_IMAGE, m_imageThumb);
	DDX_Control(pDX, IDC_XAXIS, m_xaxis);
	DDX_Control(pDX, IDC_YAXIS, m_yaxis);
	DDX_Control(pDX, IDC_SEARCH_RESULT, m_searchResultList);
	DDX_Control(pDX, IDC_BASETYPE_LIST, m_baseTypeList);
	DDX_Control(pDX, IDC_BASETYPE_1, m_baseType1);
	DDX_Control(pDX, IDC_BASETYPE_2, m_baseType2);
	DDX_Control(pDX, IDC_XAXIS_V, m_xaxisValue);
	DDX_Control(pDX, IDC_YAXIS_V, m_yaxisValue);
}

BEGIN_MESSAGE_MAP(CFriedDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BROWSE, &CFriedDlg::OnBnClickedBrowse)
	ON_MESSAGE(WM_USER + 1, &CFriedDlg::OnThreadMessage)
	ON_LBN_SELCHANGE(IDC_IMAGE_LIST, &CFriedDlg::OnLbnSelchangeImageList)
	ON_BN_CLICKED(IDC_SCAN, &CFriedDlg::OnBnClickedScan)
	ON_BN_CLICKED(IDC_SEARCH, &CFriedDlg::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_SELECT, &CFriedDlg::OnBnClickedSelect)
	ON_LBN_SELCHANGE(IDC_SEARCH_RESULT, &CFriedDlg::OnLbnSelchangeSearchResult)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_XAXIS, &CFriedDlg::OnNMReleasedcaptureXaxis)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_YAXIS, &CFriedDlg::OnNMReleasedcaptureYaxis)
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
	m_xaxis.SetRange(0, 100);
	m_yaxis.SetRange(0, 100);
	m_baseTypeCur = 0;

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


void CFriedDlg::OnBnClickedBrowse()
{
	BROWSEINFO bi; 
	ZeroMemory(&bi, sizeof(bi)); 
	TCHAR szDisplayName[MAX_PATH]; 
	szDisplayName[0] = ' '; 

	bi.hwndOwner = NULL;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDisplayName;
	bi.lpszTitle = _T("Please select a folder:");
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

LRESULT CFriedDlg::OnThreadMessage(WPARAM wParam, LPARAM lParam)
{
	CppSQLite3Query q;
	switch (wParam) {
	case 0:
		q = m_db.execQuery(_T("select * from base_types;"));
		while (!q.eof()) {
			m_baseTypeList.AddString(q.fieldValue(0));
			q.nextRow();
		}
		m_status.SetWindowTextW(_T("Done."));
		break;
	case 1:
		if (lParam == NULL) {
			m_status.SetWindowTextW(_T("Done."));
		} else {
			CString* filePath = (CString*)lParam;
			m_imageList.AddString(*filePath);
		}
		break;
	case 2:
		if (lParam == NULL) {
			m_status.SetWindowTextW(_T("Done."));
		} else {
			m_status.SetWindowTextW(*((CString*)lParam));
		}
		break;
	}
	return 0;
}

UINT LoadData(LPVOID pParam)
{
	LoadDataThreadParam* ldtp = (LoadDataThreadParam*)pParam;
	ldtp->ml->Load("..\\fried_segements_svm.dat");
	ldtp->db->open(_T("..\\fried.db"));
	SendMessage(*(ldtp->hWnd), WM_USER + 1, 0, 0);
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

void CFriedDlg::OnLbnSelchangeImageList()
{
	CString fileName;
	m_imageList.GetText(m_imageList.GetCurSel(), fileName);
	CImage image;
	image.Load(fileName);
	CDC *pDC = m_imageThumb.GetDC();
	pDC->SetStretchBltMode(HALFTONE);
	image.StretchBlt(pDC->m_hDC, 0, 0, 200, 200, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
}


void CFriedDlg::OnBnClickedScan()
{
	ScanThreadParam* stp = new ScanThreadParam;
	stp->hWnd = &m_hWnd;
	stp->db = &m_db;
	stp->ml = &m_ml;
	for (int i = 0; i < m_imageList.GetCount(); i++) {
		CString path;
		m_imageList.GetText(i, path);
		stp->imageList.push_back(path);
	}

	m_status.SetWindowTextW(_T("Scanning..."));
	AfxBeginThread(Scan, (LPVOID)stp);
}

UINT Scan(LPVOID pParam)
{
	ScanThreadParam* stp = (ScanThreadParam*)pParam;

	Mat* segments = new Mat[100];
	for (unsigned int i = 0; i < stp->imageList.size(); i++) {
		CString path = stp->imageList.at(i);
		SendMessage(*(stp->hWnd), WM_USER + 1, 2, (LPARAM)&path);

		int predictions[9];
		for (int j = 1; j < 9; j++)
			predictions[j] = 0;

		ImageSegmenter::Segment((CStringA)path, segments, 10, 10);
		for (int j = 0; j < 100; j++) {
			Mat feature;
			FeatureExtractor::GetFeatures_linHSVhist_hog(segments[j], feature);
			int p = (int)(stp->ml->Predict(feature));
			predictions[p]++;
		}

		CString query;
		query.Format(_T("insert into scenes values ('%s');"), path);
		stp->db->execDML(query);

		int rowid = 0;
		query.Format(_T("select rowid from scenes where image_path='%s';"), path);
		CppSQLite3Query q = stp->db->execQuery(query);
		if (!q.eof())
			rowid = q.fieldValueInt(0);
		for (int j = 1; j < 9; j++) {
			query.Format(_T("insert into classifications values (%d, %d, '%d');"), j, rowid, predictions[j]);
			stp->db->execDML(query);
		}
	}

	SendMessage(*(stp->hWnd), WM_USER + 1, 2, 0);
	delete segments;
	delete stp;
	return 0;
}

void CFriedDlg::OnBnClickedSearch()
{
	CString baseType1, baseType2;
	m_baseType1.GetWindowTextW(baseType1);
	m_baseType2.GetWindowTextW(baseType2);
	int x = m_xaxis.GetPos();
	int y = m_yaxis.GetPos();
	m_searchResultList.ResetContent();

	CString query;
	query.Format(_T("select * from scenes where rowid in (select of_scene_id from classifications where of_base_type_id=(select rowid from base_types where label='%s') and (prop between %d and %d)) and rowid in (select of_scene_id from classifications where of_base_type_id=(select rowid from base_types where label='%s') and (prop between %d and %d));"), baseType1, x - 10, x + 10, baseType2, y - 10, y + 10);
	CppSQLite3Query q = m_db.execQuery(query);
	while (!q.eof()) {
		m_searchResultList.AddString(q.fieldValue(0));
		q.nextRow();
	}
}

void CFriedDlg::OnBnClickedSelect()
{
	CString basetype;
	m_baseTypeList.GetText(m_baseTypeList.GetCurSel(), basetype);
	switch (m_baseTypeCur) {
	case 0:
		m_baseType1.SetWindowTextW(basetype);
		m_baseTypeCur = 1;
		break;
	case 1:
		m_baseType2.SetWindowTextW(basetype);
		m_baseTypeCur = 0;
		break;
	}
}

void CFriedDlg::OnLbnSelchangeSearchResult()
{
	CString fileName;
	m_searchResultList.GetText(m_searchResultList.GetCurSel(), fileName);
	CImage image;
	image.Load(fileName);
	CDC *pDC = m_imageThumb.GetDC();
	pDC->SetStretchBltMode(HALFTONE);
	image.StretchBlt(pDC->m_hDC, 0, 0, 200, 200, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
}

void CFriedDlg::OnNMReleasedcaptureXaxis(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString value;
	value.Format(_T("%d%%"), m_xaxis.GetPos());
	m_xaxisValue.SetWindowTextW(value);

	*pResult = 0;
}

void CFriedDlg::OnNMReleasedcaptureYaxis(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString value;
	value.Format(_T("%d%%"), m_yaxis.GetPos());
	m_yaxisValue.SetWindowTextW(value);

	*pResult = 0;
}
