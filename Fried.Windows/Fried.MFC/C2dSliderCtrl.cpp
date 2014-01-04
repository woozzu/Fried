// C2dSliderCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "C2dSliderCtrl.h"
#include "resource.h"

// C2dSliderCtrl

IMPLEMENT_DYNAMIC(C2dSliderCtrl, CWnd)

C2dSliderCtrl::C2dSliderCtrl()
{
	RegisterWindowClass();

	m_xpos = 50;
	m_ypos = 50;
	m_isMouseDown = FALSE;
}

C2dSliderCtrl::~C2dSliderCtrl()
{
}

BOOL C2dSliderCtrl::RegisterWindowClass()
{
	WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, _T("C2dSliderCtrl"), &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style            = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc      = ::DefWindowProc;
        wndcls.cbClsExtra       = wndcls.cbWndExtra = 0;
        wndcls.hInstance        = hInst;
        wndcls.hIcon            = NULL;
		wndcls.hCursor          = AfxGetApp()->LoadStandardCursor(IDC_CROSS);
        wndcls.hbrBackground    = (HBRUSH) (COLOR_3DFACE + 1);
        wndcls.lpszMenuName     = NULL;
        wndcls.lpszClassName    = _T("C2dSliderCtrl");

        if (!AfxRegisterClass(&wndcls))
        {
            AfxThrowResourceException();
            return FALSE;
        }
    }

    return TRUE;
}

void C2dSliderCtrl::SetPos(int x, int y)
{
	m_xpos = x;
	m_ypos = y;
	Invalidate();
}

void C2dSliderCtrl::SetXPos(int x)
{
	m_xpos = x;
	Invalidate();
}

void C2dSliderCtrl::SetYPos(int y)
{
	m_ypos = y;
	Invalidate();
}

int C2dSliderCtrl::GetXPos()
{
	return m_xpos;
}

int C2dSliderCtrl::GetYPos()
{
	return m_ypos;
}


BEGIN_MESSAGE_MAP(C2dSliderCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// C2dSliderCtrl 메시지 처리기입니다.




BOOL C2dSliderCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CWnd::OnEraseBkgnd(pDC);
}


void C2dSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_isMouseDown = TRUE;
	CRect rect;
	GetClientRect(&rect);

	m_xpos = (int)((float)point.x / rect.Width() * 100);
	m_ypos = 100 - (int)((float)point.y / rect.Height() * 100);
	Invalidate();

		
	CPoint pos(m_xpos, m_ypos);
	AfxGetMainWnd()->SendMessageW(WM_2D_SLIDER_MOVE, 0U, (LPARAM)&pos);

	CWnd::OnLButtonDown(nFlags, point);
}


void C2dSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_isMouseDown = FALSE;
	CWnd::OnLButtonUp(nFlags, point);
}


void C2dSliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_isMouseDown) {
		CRect rect;
		GetClientRect(&rect);

		m_xpos = (int)((float)point.x / rect.Width() * 100);
		m_ypos = 100 - (int)((float)point.y / rect.Height() * 100);
		Invalidate();

		
		CPoint pos(m_xpos, m_ypos);
		AfxGetMainWnd()->SendMessageW(WM_2D_SLIDER_MOVE, 0U, (LPARAM)&pos);
	}

	CWnd::OnMouseMove(nFlags, point);
}


void C2dSliderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rect;
	GetClientRect(&rect);

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));

	dc.FillRect(&rect, &brush);
	dc.MoveTo(0, rect.Height() / 2);
	dc.LineTo(rect.Width(), rect.Height() / 2);
	dc.MoveTo(rect.Width() / 2, 0);
	dc.LineTo(rect.Width() / 2, rect.Height());

	HICON hIcon = AfxGetApp()->LoadIconW(IDI_CROSS);
	dc.DrawIcon((int)(m_xpos / 100.0 * rect.Width()) - 16, rect.Height() - (int)(m_ypos / 100.0 * rect.Height()) - 16, hIcon);
}