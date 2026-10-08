
#pragma once

#include "ViewTree.h"
#include <map>


class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CFileView : public CDockablePane
{
// 构造
public:
	CFileView() noexcept;

	void AdjustLayout();
	void OnChangeVisualStyle();

// 特性
protected:

	CViewTree m_wndFileView;
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;

protected:
    HTREEITEM m_hRoot{ NULL };
    HTREEITEM m_hActive{ NULL };
    HTREEITEM m_hInactive{ NULL };
    std::map<SOCKET, HTREEITEM> m_viewItems;
    SOCKET m_socket{ 0 };
	void FillFileView();
public:
    enum{ em_unknow, em_connect, em_close};
    int InsertActiveFile(const CString& szFileName, SOCKET nSocket);
    int MoveFileToInactive(SOCKET nSocket);
// 实现
public:
	virtual ~CFileView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

    afx_msg LRESULT OnRecv(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()
};

