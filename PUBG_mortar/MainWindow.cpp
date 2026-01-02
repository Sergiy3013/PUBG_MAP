#include "pch.h"
#include "MainWindow.h"
#include "HookHandler.h"
#include "Setting.h"



NOTIFYICONDATA MainWindow::Tray;
bool MainWindow::DrawPoint = false;
bool MainWindow::Map_open = false;
std::vector<KeyboardRegister>::iterator MainWindow::it;
RECT MainWindow::WindowSize;
std::vector<POINT> MainWindow::pointList;
MainWindowInfo MainWindow::info;
int MainWindow::Map_Size = 0;
HINSTANCE MainWindow::hinstance;


std::string GetExecutableDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}
std::string GetConfigFilePath() {
    std::string exeDir = GetExecutableDirectory();
    return exeDir + "\\config.dat"; // Використовувати шлях виконуваного файлу
}


MainWindow& MainWindow::GteWindow()
{
    static MainWindow my;
    POINT p1{10,10}, p2{20,20};
    pointList.push_back(p1);
    pointList.push_back(p2);
    return my;
}

void MainWindow::SetHinstance(HINSTANCE hinstanceF)
{
    hinstance = hinstanceF;
}

HWND MainWindow::CreateWin() const
{
    return hWnd;
}

// Конструктор класу MainWindow
MainWindow::MainWindow()
{
    std::cout << "Ініціалізація MainWindow" << std::endl;
    // Ініціалізація значень за замовчуванням
    wc = {};
    wc.lpfnWndProc = this->WndProc;
    wc.hInstance = hinstance;
    wc.lpszClassName = TEXT("My_window");
    // Зареєструвати клас вікна
    RegisterClass(&wc);
    // Створити вікно
    hWnd = CreateMyWindow();
    if (hWnd==nullptr)
    {
        exit(0);
    }
    if (!LoadConfigurationBinary("config.dat"))
    {   
        std::cout<<"Не вдалося завантажити файл конфігурації, використовуються налаштування за замовчуванням"<< std::endl;
        info.BackGround = { 0, 20, 220, 150 };

        info.QuickKey[1] = { 0xA2 ,0x47 };
        info.QuickKey[2] = { 0x31,0x01 };
        info.QuickKey[3] = { 0x32,0x01 };
        info.QuickKey[4] = { 0x60 };
        info.QuickKey[5] = {};
        info.QuickKey[6] = {};
        info.QuickKey[7] = {};
        info.QuickKey[8] = { 0x4D };

        info.QuickKey[100] = { 0x1000 };
        info.QuickKey[101] = { 0x1001 };

        info.POINT_100M.push_back(100);
        info.POINT_100M.push_back(200);
        info.POINT_100M.push_back(300);
        info.POINT_100M.push_back(400);
        info.POINT_100M.push_back(500);

        info.PointSize = 5;

        info.console = false;
    }
    HookHandler &h= HookHandler::GetHookHandler();
    it= h.RegisterKeyboard(messageProc, info.QuickKey);
    /*if (info.console) {
        AllocConsole();
        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w", stdout);
        freopen_s(&stream, "CONOUT$", "w", stderr);
        std::cout << "Hello, console!\n " << std::endl;
    }*/
}
// Створення вікна
HWND MainWindow::CreateMyWindow() const
{
    WindowSize = { 0,0 };
    HWND desktop = GetDesktopWindow();
    GetClientRect(desktop, &WindowSize);
    return CreateWindowEx(
        WS_EX_TRANSPARENT | WS_EX_LAYERED,  // Розширені стилі: WS_EX_LAYERED | WS_EX_TRANSPARENT
        wc.lpszClassName,                   // Ім'я класу
        TEXT("Прозоре вікно"),         // Заголовок вікна
        WS_POPUP,                           // Стиль вікна
        0, 0,                               // Початкова позиція
        WindowSize.right, WindowSize.bottom,                         // Розмір вікна
        nullptr,                            // Батьківське вікно
        nullptr,                            // Меню
        hinstance,                          // HINSTANCE програми
        nullptr                             // Додаткові параметри
    );
}

void MainWindow::updataDraw() 
{
    InvalidateRect(hWnd, NULL, true);
    UpdateWindow(hWnd);
}

void MainWindow::SetKeyValue(std::map<int, std::vector<int>> Quickkey)
{
    info.QuickKey = Quickkey;
}

void MainWindow::AddIconTray()
{
    ZeroMemory(&Tray, sizeof(Tray));

    Tray.cbSize = sizeof(NOTIFYICONDATA);
    Tray.hWnd = CreateWin();
    Tray.uID = 1; // ID іконки в треї
    Tray.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    Tray.uCallbackMessage = WM_USER + 1; // Повідомлення зворотнього виклику
    Tray.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1)); // Завантажити іконку з ресурсів
    lstrcpy(Tray.szTip, _T("Панель карти"));
    
    Shell_NotifyIcon(NIM_ADD,&Tray);
}

void MainWindow::RemoveTray()
{
    Shell_NotifyIcon(NIM_DELETE, &Tray);
}

bool MainWindow::LoadConfigurationBinary(const std::string& filename)
{
    std::ifstream file(GetConfigFilePath(), std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        MessageBox(NULL, L"Failed to open configuration file for reading:", L"", MB_OK);
        return false;
    }

    try {
        // Read POINT_100M vector
        uint32_t pointSize;
        file.read(reinterpret_cast<char*>(&pointSize), sizeof(pointSize));
        info.POINT_100M.resize(pointSize);
        file.read(reinterpret_cast<char*>(info.POINT_100M.data()), pointSize * sizeof(int));

        // Read BackGround
        file.read(reinterpret_cast<char*>(&info.BackGround), sizeof(info.BackGround));

        // Read PointSize
        file.read(reinterpret_cast<char*>(&info.PointSize), sizeof(info.PointSize));

        // **Read console flag**
        file.read(reinterpret_cast<char*>(&info.console), sizeof(info.console));

        // Read QuickKey size
        uint32_t quickKeySize;
        file.read(reinterpret_cast<char*>(&quickKeySize), sizeof(quickKeySize));

        info.QuickKey.clear();
        for (uint32_t i = 0; i < quickKeySize; ++i) {
            // Read key
            int key;
            file.read(reinterpret_cast<char*>(&key), sizeof(key));

            // Read vector size
            uint32_t vecSize;
            file.read(reinterpret_cast<char*>(&vecSize), sizeof(vecSize));

            // Read vector elements
            std::vector<int> values(vecSize);
            if (vecSize > 0) {
                file.read(reinterpret_cast<char*>(values.data()), vecSize * sizeof(int));
            }

            info.QuickKey[key] = std::move(values);
        }

        file.close(); // Ensure the file is properly closed
    }
    catch (const std::exception& e) {
        MessageBox(NULL, L"Exception occurred while reading the binary file:", L"", MB_OK);
        return false;
    }

    return true;
}

void MainWindow::SaveConfigurationBinary(const std::string& filename)
{
    it->keyBindings = info.QuickKey;

    std::ofstream file(GetConfigFilePath(), std::ios::out | std::ios::binary);
    if (!file.is_open()) {
        MessageBox(NULL, L"Failed to open configuration file for writing:", L"", MB_OK);
        return;
    }

    MainWindow& m = MainWindow::GteWindow();
    m.updataDraw();

    try {
        // Write POINT_100M vector
        uint32_t pointSize = static_cast<uint32_t>(info.POINT_100M.size());
        file.write(reinterpret_cast<const char*>(&pointSize), sizeof(pointSize));
        if (pointSize > 0) {
            file.write(reinterpret_cast<const char*>(info.POINT_100M.data()), pointSize * sizeof(int));
        }

        // Write BackGround
        file.write(reinterpret_cast<const char*>(&info.BackGround), sizeof(info.BackGround));

        // Write PointSize
        file.write(reinterpret_cast<const char*>(&info.PointSize), sizeof(info.PointSize));

        // **Write console flag**
        file.write(reinterpret_cast<const char*>(&info.console), sizeof(info.console));

        // Write QuickKey size
        uint32_t quickKeySize = static_cast<uint32_t>(info.QuickKey.size());
        file.write(reinterpret_cast<const char*>(&quickKeySize), sizeof(quickKeySize));

        for (const auto& el : info.QuickKey) {
            // Write key
            int key = el.first;
            file.write(reinterpret_cast<const char*>(&key), sizeof(key));

            // Write vector size
            uint32_t vecSize = static_cast<uint32_t>(el.second.size());
            file.write(reinterpret_cast<const char*>(&vecSize), sizeof(vecSize));

            // Write vector elements
            if (vecSize > 0) {
                file.write(reinterpret_cast<const char*>(el.second.data()), vecSize * sizeof(int));
            }
        }

        file.flush();
        file.close(); // Ensure the file is properly closed
    }
    catch (const std::exception& e) {
        MessageBox(NULL, L"Exception occurred while writing the binary file:", L"", MB_OK);
    }
}

LRESULT MainWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
        break;
    case WM_USER + 1:
    {
        switch (lParam)
        {
        case WM_RBUTTONDOWN:
            HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDR_MENU1));
            if (hMenu == NULL) {
                MessageBox(hwnd, TEXT("Failed to load menu resource"), TEXT("Error"), MB_OK);
                
                return 0;
            }

            HMENU hSubMenu = GetSubMenu(hMenu, 0);
            if (hSubMenu == NULL) {
                MessageBox(hwnd, TEXT("Failed to get submenu"), TEXT("Error"), MB_OK);
                DestroyMenu(hMenu);
                return 0;
            }


            POINT pt;
            GetCursorPos(&pt);
            SetForegroundWindow(hwnd);

            TrackPopupMenu(hSubMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);

            DestroyMenu(hMenu);
            break;
        }
    }
        break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId) 
        {
        case ID_SETTING:
        {
            Setting& s = Setting::Getsetting();
            s.Setinfo(info);
            s.ShowWindow(SW_SHOW);
            s.BringWindowToTop();

        } 
            break;
        case ID_EXIT:
        {
            SaveConfigurationBinary("config.dat");
            PostQuitMessage(0);
        }
            break;
        }
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // Створити сумісний пам'яті DC
        HDC memDC = CreateCompatibleDC(hdc);
        // Створити сумісний bitmap для DC
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top);
        // Вибрати bitmap у DC
        SelectObject(memDC, memBitmap);

        RECT rect = { 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top };
        FillRect(memDC, &rect, (HBRUSH)GetStockObject(NULL_BRUSH)); // Очищуємо пам'ять DC
        
        // Заповнення фону відповідно до info.BackGround
        HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0)); // Вибір кольору для фону
        FillRect(memDC, &info.BackGround, hBrush);
        // Малюємо вміст карти
        Draw(memDC);


        // Перенести малюнок з пам'яті на екран
        BitBlt(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, memDC, 0, 0, SRCCOPY);
        DeleteObject(memBitmap);
        DeleteObject(hBrush);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
       
    }
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

void MainWindow::messageProc(int message, LPARAM lpramr)
{
    std::cout << "Повідомлення: " << message << std::endl;
    MSLLHOOKSTRUCT* mouse = (MSLLHOOKSTRUCT*)lpramr;
    switch (message)
    {
    case 1:
    {
        DrawPoint = !DrawPoint;
        break;
    }
    
    case 2:
    {
        if (Map_open)
        {
            pointList.at(0) = mouse->pt;
        }
        
    }
        break;
    case 3:
    {
        if (Map_open)
        {
            pointList.at(1) = mouse->pt;
        }
    }
        break;
    case 4:
    {
        Map_open = !Map_open;
    }
    break;
    case 6:
    {
        if (Map_open)
        {
            pointList.at(0).x = WindowSize.right / 2;
            pointList.at(0).y = WindowSize.bottom / 2;
        }
       
    }
        break;
    case 7:
    {
        if (Map_open)
        {
            pointList.at(1).x = WindowSize.right / 2;
            pointList.at(1).y = WindowSize.bottom / 2;
        }
        
    }
        break;
    case 8: 
    {
        Map_open = !Map_open;
    }
        break;
    case 100:
    {
        if (Map_Size<info.POINT_100M.size()-1&& Map_open)
        {
            Map_Size++;
        }
        break;
    }
    case 101:
    {
        if (Map_Size > 0&& Map_open)
        {
            Map_Size--;
        }
        break;

    }
    }
    MainWindow::GteWindow().updataDraw();
}

const void MainWindow::Draw(HDC & hdc)
{
    if (Map_open)
    {
        HBRUSH brushGreen = CreateSolidBrush(RGB(0, 255, 0));
        SelectObject(hdc, brushGreen);
        Ellipse(hdc, pointList[0].x - info.PointSize, pointList[0].y - info.PointSize, pointList[0].x + info.PointSize, pointList[0].y + info.PointSize);

        // Малюємо червону мітку для другого пункту
        HBRUSH brushRed = CreateSolidBrush(RGB(255, 0, 0));
        SelectObject(hdc, brushRed);
        Ellipse(hdc, pointList[1].x - info.PointSize, pointList[1].y - info.PointSize, pointList[1].x + info.PointSize, pointList[1].y + info.PointSize);

        // Видалити тимчасові кисті
        DeleteObject(brushGreen);
        DeleteObject(brushRed);
    }

    // Фон для текстової інформації
    HBRUSH brushRed = CreateSolidBrush(RGB(255, 0, 255));
    FillRect(hdc, &info.BackGround, brushRed);
    DeleteObject(brushRed);

    // Параметри шрифту
    int fontHeight = 20; // висота шрифту в пунктах
    HFONT hFont = CreateFontW(
        fontHeight,                // висота шрифту
        0,                         // ширина символу (0 - автоматично)
        0,                         // кут нахилу
        0,                         // орієнтація
        FW_NORMAL,                 // вага шрифту
        FALSE,                     // курсив
        FALSE,                     // підкреслення
        FALSE,                     // закреслення
        EASTEUROPE_CHARSET,        // набір символів (підтримка кирилиці)
        OUT_DEFAULT_PRECIS,        // точність виходу
        CLIP_DEFAULT_PRECIS,       // точність обрізки
        DEFAULT_QUALITY,           // якість відтворення
        DEFAULT_PITCH | FF_SWISS,  // сімейство і шаблон
        L"Segoe UI"                 // ім'я шрифту (краще підтримує кирилицю)
    );

    // Вибираємо шрифт у DC
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);

    // Підготовка рядків для відображення
    std::wstring str1 = L"\u0421\u0442\u0430\u043D \u043A\u0430\u0440\u0442\u0438: ";
    str1 += (Map_open) ? std::wstring(L"\u0412\u0456\u0434\u043A\u0440\u0438\u0442\u043E") : std::wstring(L"\u0417\u0430\u043A\u0440\u0438\u0442\u043E");
    std::wstring str3 = std::wstring(L"\u0420\u0435\u0436\u0438\u043C \u0437\u0431\u0456\u043B\u044C\u0448\u0435\u043D\u043D\u044F \u043A\u0430\u0440\u0442\u0438: ") + std::to_wstring(Map_Size);
    std::wstring str4 = std::wstring(L"\u041C\u0430\u0441\u0448\u0442\u0430\u0431 \u0434\u043B\u044F 100\u043C: ") + std::to_wstring(info.POINT_100M[Map_Size]);
    std::wstring str5 = std::wstring(L"\u0412\u0456\u0434\u0441\u0442\u0430\u043D\u044C \u043C\u0456\u0436 \u0442\u043E\u0447\u043A\u0430\u043C\u0438: ") + std::to_wstring((int)(std::sqrt(std::pow(pointList[0].x - pointList[1].x, 2) + std::pow(pointList[0].y - pointList[1].y, 2))));
    std::wstring str6 = std::wstring(L"\u0412\u0456\u0434\u0441\u0442\u0430\u043D\u044C \u0443 \u043C\u0435\u0442\u0440\u0430\u0445: ") + std::to_wstring((int)(std::sqrt(std::pow(pointList[0].x - pointList[1].x, 2) + std::pow(pointList[0].y - pointList[1].y, 2)) * ((double)100 / info.POINT_100M[Map_Size])));

    
    // Параметри для DrawTextExW
    DRAWTEXTPARAMS dtp;
    ZeroMemory(&dtp, sizeof(DRAWTEXTPARAMS));
    dtp.cbSize = sizeof(DRAWTEXTPARAMS);
    dtp.iTabLength = 4;
    dtp.iLeftMargin = 0;
    dtp.iRightMargin = 0;

    RECT textRect = info.BackGround;
    textRect.left += 5;
    textRect.top += 5;
    const int padding = 4;

    auto DrawAndAdvance = [&](const std::wstring& s) {
        RECT calc = textRect;
        UINT calcFlags = DT_LEFT | DT_WORDBREAK | DT_CALCRECT;
        DrawTextExW(hdc, const_cast<LPWSTR>(s.c_str()), -1, &calc, calcFlags, &dtp);
        // намалювати текст у видимій області
        UINT drawFlags = DT_LEFT | DT_WORDBREAK;
        DrawTextExW(hdc, const_cast<LPWSTR>(s.c_str()), -1, &textRect, drawFlags, &dtp);
        textRect.top += (calc.bottom - calc.top) + padding;
    };

    DrawAndAdvance(str1);
    DrawAndAdvance(str3);
    DrawAndAdvance(str4);
    DrawAndAdvance(str5);
    DrawAndAdvance(str6);


    // �ָ��ɵ�����
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
}
