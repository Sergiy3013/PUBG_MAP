// Setting.cpp: реалізація діалогу налаштувань
//

#include "pch.h"
#include "afxdialogex.h"
#include "Setting.h"
#include "HookHandler.h"
#include "MainWindow.h"

// Діалог налаштувань (інформаційна мітка)

IMPLEMENT_DYNAMIC(Setting, CDialogEx)


Setting& Setting::Getsetting()
{
	static Setting s;
	return s;
}

Setting::Setting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	if (!Create(IDD_DIALOG1, pParent))
	{
		AfxMessageBox(_T("Не вдалося створити діалог налаштувань."));
	}
	info = { 0 };
}

Setting::~Setting()
{
}

void Setting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	// Прив'язка елементів введення
	// Контроли для налаштування гарячих клавіш
	DDX_Control(pDX, IDC_HOTKEY1, HOTKEY1);
	DDX_Control(pDX, IDC_HOTKEY2, HOTKEY2);
	DDX_Control(pDX, IDC_HOTKEY3, HOTKEY3);
	DDX_Control(pDX, IDC_HOTKEY4, HOTKEY4);
	DDX_Control(pDX, IDC_HOTKEY6, HOTKEY6);
	DDX_Control(pDX, IDC_HOTKEY7, HOTKEY7);
	DDX_Control(pDX, IDC_HOTKEY8, HOTKEY8);
	DDX_Control(pDX, IDC_COMBO1, CComboBox_List);

	DDX_Control(pDX, IDC_EDIT1, EDIT);
	DDX_Control(pDX, IDC_EDIT2, EDIT2);

	DDX_Control(pDX, IDC_EDIT4, EDIT_X);
	DDX_Control(pDX, IDC_EDIT5, EDIT_Y);
	DDX_Control(pDX, IDC_EDIT6, EDIT_BOTTOM);
	DDX_Control(pDX, IDC_EDIT7, EDIT_RIGHT);


	DDX_Control(pDX, IDC_CHECK1, Chcke1);
}



BEGIN_MESSAGE_MAP(Setting, CDialogEx)
	ON_BN_CLICKED(IDOK, &Setting::OnBnClickedOk)

	ON_CBN_SELCHANGE(IDC_COMBO1, &Setting::OnCbnSelchangeCombo1)
	ON_EN_KILLFOCUS(IDC_EDIT1, &Setting::OnEnKillfocusEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, &Setting::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Setting::OnBnClickedButton2)
	ON_EN_KILLFOCUS(IDC_EDIT4, &Setting::OnEnKillfocusEdit4)
	ON_EN_KILLFOCUS(IDC_EDIT5, &Setting::OnEnKillfocusEdit5)
	ON_EN_KILLFOCUS(IDC_EDIT6, &Setting::OnEnKillfocusEdit6)
	ON_EN_KILLFOCUS(IDC_EDIT7, &Setting::OnEnKillfocusEdit7)
	ON_BN_CLICKED(IDCANCEL, &Setting::OnBnClickedCancel)
END_MESSAGE_MAP()


// Обробники повідомлень для Setting


void Setting::OnBnClickedOk()
{
	if (info!=nullptr)
	{
		SetKeyQuick();
		CString str;
		EDIT2.GetWindowTextW(str);
		info->PointSize = _ttoi(str);
		MainWindow::SaveConfigurationBinary("1");
	}
    
}

void Setting::Setinfo(MainWindowInfo &showinfo)
{
	info = &showinfo;
	if (info->QuickKey.empty())
	{
		return;
	}

	// Встановити відображення гарячих клавіш у контролах
	if (info != NULL)
	{
		SetKeyShow(HOTKEY1, info->QuickKey[1]);
		std::vector<int>list = info->QuickKey[2];
		list.pop_back();
		SetKeyShow(HOTKEY2, list);
		list = info->QuickKey[3];
		list.pop_back();
		SetKeyShow(HOTKEY3, list);
		SetKeyShow(HOTKEY4, info->QuickKey[4]);
		//SetKeyShow(HOTKEY5, info->QuickKey[5]);
		SetKeyShow(HOTKEY6, info->QuickKey[6]);
		SetKeyShow(HOTKEY7, info->QuickKey[7]);
		SetKeyShow(HOTKEY8, info->QuickKey[8]);

		for (int x = 0; x < info->POINT_100M.size(); x++)
		{
			if (x == 0)
				CComboBox_List.SetCurSel(0);
			std::wstring str = L"Збільшити ";
			str += std::to_wstring(x);
			str += L" раз";
			CComboBox_List.AddString(str.c_str());
		}

		EDIT2.SetWindowTextW(std::to_wstring(info->PointSize).c_str());

		// Обчислити висоту та ширину
		int height = info->BackGround.bottom - info->BackGround.top;
		int width = info->BackGround.right - info->BackGround.left;

		// Встановити текст у полях введення
		EDIT_X.SetWindowTextW(std::to_wstring(info->BackGround.left).c_str());
		EDIT_Y.SetWindowTextW(std::to_wstring(info->BackGround.top).c_str());
		EDIT_BOTTOM.SetWindowTextW(std::to_wstring(height).c_str());
		EDIT_RIGHT.SetWindowTextW(std::to_wstring(width).c_str());
		if (info->console)
			Chcke1.SetCheck(BST_CHECKED);
		else
			Chcke1.SetCheck(BST_UNCHECKED);
		
	}

	// Збільшити розміри контролів і застосувати шрифт, що підтримує кирилицю
	// Створюємо шрифт із більшим розміром і восточноєвропейською кодировкою
	CFont* pNewFont = new CFont();
	LOGFONTW lf = {0};
	lf.lfHeight = -18; // приблизно 18pt
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = EASTEUROPE_CHARSET;
	wcscpy_s(lf.lfFaceName, L"Segoe UI");
	pNewFont->CreateFontIndirectW(&lf);

	// Застосувати шрифт до діалогу та основних контролів
	SetFont(pNewFont);
	HOTKEY1.SetFont(pNewFont);
	HOTKEY2.SetFont(pNewFont);
	HOTKEY3.SetFont(pNewFont);
	HOTKEY4.SetFont(pNewFont);
	HOTKEY6.SetFont(pNewFont);
	HOTKEY7.SetFont(pNewFont);
	HOTKEY8.SetFont(pNewFont);
	CComboBox_List.SetFont(pNewFont);
	EDIT.SetFont(pNewFont);
	EDIT2.SetFont(pNewFont);
	EDIT_X.SetFont(pNewFont);
	EDIT_Y.SetFont(pNewFont);
	EDIT_BOTTOM.SetFont(pNewFont);
	EDIT_RIGHT.SetFont(pNewFont);
	Chcke1.SetFont(pNewFont);

	// (kept font changes only; control sizes remain unchanged)
}

// установка відображення гарячих клавіш у контролі
bool Setting::SetKeyShow(CHotKeyCtrl& HOYKEY, std::vector<int>& keyvalue)
{
	if (keyvalue.empty()) return false;

	WORD wVirtualKeyCode = 0;
	WORD wModifiers = 0;

	// Check if the first value in keyvalue indicates a modifier key
	if (keyvalue.size() > 1) {
		// Determine modifiers
		if (keyvalue.at(0) == 0x11) { // Ctrl
			wModifiers |= HOTKEYF_CONTROL;
		}
		else if (keyvalue.at(0) == 0xA2 || keyvalue.at(0) == 0xA3) { // Right or Left Ctrl
			wModifiers |= HOTKEYF_CONTROL;
		}
		else if (keyvalue.at(0) == 0x10) { // Shift
			wModifiers |= HOTKEYF_SHIFT;
		}
		else if (keyvalue.at(0) == 0x12) { // Alt
			wModifiers |= HOTKEYF_ALT;
		}

		// The second value is the actual virtual key code
		wVirtualKeyCode = keyvalue.at(1);
	}
	else {
		// If there's only one key value, it's treated as a single key hotkey
		wVirtualKeyCode = keyvalue.at(0);
	}

	HOYKEY.SetHotKey(wVirtualKeyCode, wModifiers);
	return true;
}

// Передати гарячі клавіші до обробника
bool Setting::SetKeyQuick()
{
	info->QuickKey[1] = GetkeyList(HOTKEY1);
	std::vector<int> list = GetkeyList(HOTKEY2);
	list.push_back(0x01);
	info->QuickKey[2] = list;
	list =  GetkeyList(HOTKEY3);
	list.push_back(0x01);
	info->QuickKey[3] = list;
	info->QuickKey[4] = GetkeyList(HOTKEY4);
	//info->QuickKey[5] = GetkeyList(HOTKEY5);
	info->QuickKey[6] = GetkeyList(HOTKEY6);
	info->QuickKey[7] = GetkeyList(HOTKEY7);
	info->QuickKey[8] = GetkeyList(HOTKEY8);
	if (Chcke1.GetCheck()== BST_UNCHECKED) {
		info->console = false;
	}
	else {
		info->console = true;
	}
	return false;
}

// Перетворити гарячі клавіші в послідовність значень
std::vector<int> Setting::GetkeyList(CHotKeyCtrl& HOTKEY)
{
	std::vector<int> key;
	WORD wVirtualKeyCode = 0;
	WORD wModifiers = 0;

	// Використати правильний перевантажений метод для отримання гарячої клавіші
	HOTKEY.GetHotKey(wVirtualKeyCode, wModifiers);

	if (wModifiers & HOTKEYF_SHIFT) key.push_back(0xA0);
	if (wModifiers & HOTKEYF_CONTROL)key.push_back(0xA2);
	if (wModifiers & HOTKEYF_ALT)key.push_back(0xA4);

	key.push_back(wVirtualKeyCode);
	return key;
}

void Setting::OnCbnSelchangeCombo1()
{
	// TODO: Додати обробник повідомлення контролу
	int nSel = CComboBox_List.GetCurSel();
	std::cout << nSel << std::endl;
	
	EDIT.SetWindowTextW(std::to_wstring(info->POINT_100M[nSel]).c_str());
}

void Setting::OnEnKillfocusEdit1()
{
	// TODO: Додати обробник повідомлення контролу
	
	if (info->POINT_100M.size()==0||CComboBox_List.GetCurSel()== CB_ERR)
	{
		return;
	}
	CString str;
	EDIT.GetWindowTextW(str);
	info->POINT_100M[CComboBox_List.GetCurSel()] = _ttoi(str);
}

void Setting::OnBnClickedButton1()
{
	info->POINT_100M.push_back(0);
	std::wstring str = L"Збільшити ";
	str += std::to_wstring(info->POINT_100M.size() - 1);
	str += L" раз";
	CComboBox_List.AddString(str.c_str());
	// TODO: Додати обробник повідомлення контролу
}

void Setting::OnBnClickedButton2()
{

	int itemCount = CComboBox_List.GetCount(); // Отримати кількість елементів у комбобоксі

	if (itemCount!=0)
	{
		CComboBox_List.DeleteString(itemCount - 1);
		CComboBox_List.SetCurSel(itemCount - 2);
		info->POINT_100M.pop_back();
		if (itemCount>1)
		{
			OnCbnSelchangeCombo1();
		}
		
	}
}


void Setting::OnEnKillfocusEdit4()
{
	CString str;
	EDIT_X.GetWindowTextW(str);
	info->BackGround.left = _ttoi(str);

	EDIT_RIGHT.GetWindowTextW(str);
	int width = _ttoi(str);

	info->BackGround.right = info->BackGround.left+ width;
}

void Setting::OnEnKillfocusEdit5()
{
	CString str;
	EDIT_Y.GetWindowTextW(str);
	info->BackGround.top = _ttoi(str);

	EDIT_BOTTOM.GetWindowTextW(str);
	int height = _ttoi(str);
	info->BackGround.bottom = info->BackGround.top + height;
}

void Setting::OnEnKillfocusEdit6()
{
	CString str;
	EDIT_BOTTOM.GetWindowTextW(str);
	int height = _ttoi(str);

	// Обчислити значення Bottom, враховуючи Left і Top
	info->BackGround.bottom = info->BackGround.top + height;
}

void Setting::OnEnKillfocusEdit7()
{
	CString str;
	EDIT_RIGHT.GetWindowTextW(str);
	int width = _ttoi(str);

	// Обчислити значення Right, враховуючи Left і Top
	info->BackGround.right = info->BackGround.left + width;
}


void Setting::OnBnClickedCancel()
{
	// TODO: Додати код обробника повідомлення контролу
	CDialogEx::OnCancel();
	ShowWindow(SW_HIDE);
}
