#pragma once

#define WM_2D_SLIDER_MOVE WM_USER + 123

// C2dSliderCtrl

class C2dSliderCtrl : public CWnd
{
	DECLARE_DYNAMIC(C2dSliderCtrl)

public:
	C2dSliderCtrl();
	virtual ~C2dSliderCtrl();
	void SetPos(int x, int y);
	void SetXPos(int x);
	void SetYPos(int y);
	int GetXPos();
	int GetYPos();

private:
	int m_xpos;
	int m_ypos;
	BOOL m_isMouseDown;

protected:
	BOOL RegisterWindowClass();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
};


