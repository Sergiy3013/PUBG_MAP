#pragma once

//struct KeyboardRegister {
//	int id;
//	std::function<void(int,LPARAM)> callback;
//	std::map<int, std::vector<int>>  keyBindings;
//};


class HookHandler
{
public:
	// Отримати доступ до єдиного екземпляру (singleton)
	static HookHandler& GetHookHandler();
	static LRESULT CALLBACK ALLMessageProc(int nCode, WPARAM wParam, LPARAM lParam);

	bool installHook(HOOKPROC proc = ALLMessageProc);
	void UnistallHook() const;
	// Зареєструвати обробник клавіатурних комбінацій
	std::vector<KeyboardRegister>::iterator RegisterKeyboard(std::function<void(int,LPARAM)>, std::map<int, std::vector<int>>);

	bool RemoveRegisterKeyboard(std::vector<KeyboardRegister>::iterator it);


private:
	HookHandler();
	HHOOK MouseHook=nullptr;
	HHOOK KeyboardHook=nullptr;

	// Список поточних реєстрацій клавіш
	static std::vector<KeyboardRegister> keyRegisters;

	
	// Порівняння списків клавіш
	static void compareKeyList(const std::vector<int>& b,LPARAM lparm);
	// Видалити значення з поточного списку клавіш
	static void removeKey(std::vector<int>& keyValue, int valueToRemove);
};
