#pragma once
#include "afxdialogex.h"
#include "MainWindow.h"

// Діалог налаштувань

class Setting : public CDialogEx
{
	DECLARE_DYNAMIC(Setting)

public:
	static Setting& Getsetting();
	
	virtual ~Setting();
private:
	Setting(CWnd* pParent = nullptr);   // Стандартний конструктор
// Дані діалогу
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Підтримка DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	

	void Setinfo(MainWindowInfo& showinfo);

	
	// Встановити відображення гарячих клавіш у вікні
	bool SetKeyShow(CHotKeyCtrl&, std::vector<int>&);
	// Передати гарячі клавіші до обробника
	bool SetKeyQuick();
	// Перетворити гарячі клавіші у послідовність значень
	std::vector<int> GetkeyList(CHotKeyCtrl&);
	
	MainWindowInfo *info;


	CHotKeyCtrl HOTKEY1;
	CHotKeyCtrl HOTKEY2;
	CHotKeyCtrl HOTKEY3;
	CHotKeyCtrl HOTKEY4;
	//CHotKeyCtrl HOTKEY5;
	CHotKeyCtrl HOTKEY6;
	CHotKeyCtrl HOTKEY7;
	CHotKeyCtrl HOTKEY8;

	CComboBox CComboBox_List;
	
	CEdit EDIT;
	CEdit EDIT2;
	CEdit EDIT_X;
	CEdit EDIT_Y;
	CEdit EDIT_BOTTOM;
	CEdit EDIT_RIGHT;

	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnEnKillfocusEdit1();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnKillfocusEdit4();
	afx_msg void OnEnKillfocusEdit5();
	afx_msg void OnEnKillfocusEdit6();
	afx_msg void OnEnKillfocusEdit7();
	afx_msg void OnBnClickedCancel();
private:
	CButton Chcke1;
};
