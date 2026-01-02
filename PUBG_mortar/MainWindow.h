#pragma once
#include "Setting.h"



class MainWindow
{
public:
	static MainWindow& GteWindow();
	static void SetHinstance(HINSTANCE hinstanceF);
	HWND CreateWin() const;

	// Заборонити використання неявних копіювань (delete copy semantics)
	MainWindow(const MainWindow&) = delete;
	MainWindow& operator=(const MainWindow&) = delete;
	void updataDraw();
	void SetKeyValue(std::map<int, std::vector<int>>);

	// Інформація про точки для відображення на карті
	static std::vector<POINT> pointList;
	static MainWindowInfo info;
	static int Map_Size;
	// Додавання/видалення іконки в трей
	void AddIconTray();
	void RemoveTray();
	static RECT WindowSize;
	static bool DrawPoint;
	static bool Map_open;
	


	// Завантажити/зберегти бінарний файл конфігурації
	bool LoadConfigurationBinary(const std::string& filename);
	static void SaveConfigurationBinary(const std::string& filename);
private:
	// Обробник вікнових повідомлень
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	// Обробник повідомлень від HookHandler (виклик функцій по повідомленнях)
	static void CALLBACK messageProc(int message,LPARAM lpramr);

	static std::vector<KeyboardRegister>::iterator it;

	HWND CreateMyWindow() const;
	MainWindow();
	const static void Draw(HDC &);

	// Інформація про іконку в треї
	static NOTIFYICONDATA Tray;

	static HINSTANCE hinstance;
	// Глобальний екземпляр HINSTANCE програми
	HWND hWnd;
	
	WNDCLASS wc;
	
};

