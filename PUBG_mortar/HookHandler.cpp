#include "pch.h"
#include "HookHandler.h"
#include "MainWindow.h"


// Обробник подій клавіатури/миші
// Примітки:
/*
    Додаткові коди колеса миші зберігаються як int
        Колесо вгору: 0x1000
        Колесо вниз: 0x1001

    Використовується для порівняння послідовностей клавіш
*/


std::vector<KeyboardRegister> HookHandler::keyRegisters;

HookHandler::HookHandler() {

}

HookHandler& HookHandler::GetHookHandler()
{
    static HookHandler myHookHandler;
    return myHookHandler;
}

LRESULT HookHandler::ALLMessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{

    static std::vector<int>  pressedKeys;
    static LPARAM temp;
    if (nCode >= 0&&!keyRegisters.empty()) {
        switch (wParam)
        {
        case WM_KEYDOWN:
        {
            KBDLLHOOKSTRUCT* keycode = (KBDLLHOOKSTRUCT*)lParam;
            if (pressedKeys.empty())
            {
                pressedKeys.push_back(keycode->vkCode);
            }
            else if (pressedKeys.back() != keycode->vkCode)
            {
                pressedKeys.push_back(keycode->vkCode);
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            pressedKeys.push_back(0x01);
            compareKeyList(pressedKeys, lParam);
            removeKey(pressedKeys, 0x01);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            pressedKeys.push_back(0x02);
            compareKeyList(pressedKeys, lParam);
            removeKey(pressedKeys, 0x02);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            MSLLHOOKSTRUCT* s=(MSLLHOOKSTRUCT*)lParam;
            int x = GET_WHEEL_DELTA_WPARAM(s->mouseData) / WHEEL_DELTA;
            if (x>0)
            {
                pressedKeys.push_back(0x1000);
            }
            else if (x < 0)
            {

                pressedKeys.push_back(0x1001);
                
            }
            compareKeyList(pressedKeys, lParam);
            removeKey(pressedKeys, 0x1000);
            removeKey(pressedKeys, 0x1001);
            break;
        }
        case WM_KEYUP:
        {
            KBDLLHOOKSTRUCT* keycode = (KBDLLHOOKSTRUCT*)lParam;
            compareKeyList(pressedKeys, lParam);
            removeKey(pressedKeys, keycode->vkCode);
            break;
        }
        default:
            break;
        }

    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

bool HookHandler::installHook(HOOKPROC proc)
{
    // Встановити хуки; у разі помилки — вивести повідомлення
    MouseHook = SetWindowsHookEx(WH_MOUSE_LL, proc, NULL, 0);
    if (MouseHook==nullptr)
    {
        std::cout << "Не вдалось встановити мишачий хук" << std::endl;

        return false;
    }
    KeyboardHook=SetWindowsHookEx(WH_KEYBOARD_LL, proc, NULL, 0);
    if (KeyboardHook==nullptr)
    {
        std::cout << "Не вдалось встановити клавіатурний хук" << std::endl;
        UnhookWindowsHookEx(MouseHook);
        return false;
    }
    std::cout << "Хуки успішно встановлені" << std::endl;
    return true;
}

void HookHandler::UnistallHook() const
{
    if (MouseHook!=nullptr)
    {
        UnhookWindowsHookEx(MouseHook);
    }
    if (KeyboardHook!=nullptr)
    {
        UnhookWindowsHookEx(KeyboardHook);
    }
}



std::vector<KeyboardRegister>::iterator HookHandler::RegisterKeyboard(std::function<void(int,LPARAM)> callback, std::map<int, std::vector<int>> key) {
    static int id = 0;
    KeyboardRegister KeyRegister;
    KeyRegister.callback = callback;
    KeyRegister.keyBindings = key;
    KeyRegister.id = id++;

    return keyRegisters.insert(keyRegisters.end(), KeyRegister);
}

bool HookHandler::RemoveRegisterKeyboard(std::vector<KeyboardRegister>::iterator it)
{
    if (it == keyRegisters.end()) {
        // Неправильний ітератор — повернути false
        return false;
    }

    keyRegisters.erase(it);
    return true;
}


void HookHandler::compareKeyList(const std::vector<int>& b, LPARAM lparm)
{
    if (!keyRegisters.empty() && !b.empty()) {
        for (const auto& registerInfo : keyRegisters) {
            for (const auto& binding : registerInfo.keyBindings) {
                // Перевіряємо тільки зв'язки, довжина яких не перевищує поточної послідовності
                if (binding.second.size() <= b.size()&& binding.second.size()>0) {
                    bool isMatch = true;
                    size_t matchStartIndex = 0;

                    // �������У�����Ƿ����ҵ�ƥ��
                    for (size_t i = 0; i <= b.size() - binding.second.size(); ++i) {
                        isMatch = true;
                        for (size_t j = 0; j < binding.second.size(); ++j) {
                            if (binding.second[j] != b[i + j]) {
                                isMatch = false;
                                break;
                            }
                        }
                        if (isMatch) {
                            matchStartIndex = i;
                            break;
                        }
                    }

                    // Якщо знайдено відповідність — викликаємо зворотній виклик
                    if (isMatch) {
                        registerInfo.callback(binding.first, lparm);
                    }
                }
            }
        }
    }
}

void HookHandler::removeKey(std::vector<int>& keyValue, int valueToRemove)
{

    // Використовуємо std::remove + erase для видалення значення зі вектора
    auto newEnd = std::remove(keyValue.begin(), keyValue.end(), valueToRemove);
    keyValue.erase(newEnd, keyValue.end());
}

