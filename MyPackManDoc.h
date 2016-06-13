
// MyPackManDoc.h : CMyPackManDoc Ŭ������ �������̽�
//


#pragma once
#include "Scene.h"
#include "Icon.h"
#include "Game.h"

class CMyPackManDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CMyPackManDoc();
	DECLARE_DYNCREATE(CMyPackManDoc)

public:

	Scene mScene;
	Icon mIcon;
	Game mGame;

	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// �����Դϴ�.
public:
	virtual ~CMyPackManDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// �˻� ó���⿡ ���� �˻� �������� �����ϴ� ����� �Լ�
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
};
