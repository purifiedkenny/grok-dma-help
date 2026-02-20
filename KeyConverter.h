#pragma once
#include <xutility>
#include <Windows.h>

enum INPUT_KEY
{
	// 鼠标按键 - APEX值
	Leftmousebutton = 107,    // 验证正确
	RightMousebutton = 108,   // 验证正确
	Middlemousebutton = 109,  // 验证正确
	Sidemousebutton = 110,    // Mouse4
	Sidemouse2button = 111,   // Mouse5

	// 特殊控制键
	SHIFTkey = 79,           // 左SHIFT
	ALTkey = 81,             // 左ALT
	CTRLkey = 83,            // 左CTRL

	// 数字键 (0-9) - 保持原有映射
	KEY_0 = 1,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,

	// 字母键 (A-Z)
	EKEY_A = 11,    // 从11开始，与数字键连续
	EKEY_B,
	EKEY_C,
	EKEY_D,
	EKEY_E,
	EKEY_F,
	EKEY_G,
	EKEY_H,
	EKEY_I,
	EKEY_J,
	EKEY_K,
	EKEY_L,
	EKEY_M,
	EKEY_N,
	EKEY_O,
	EKEY_P,
	EKEY_Q,
	EKEY_R,
	EKEY_S,
	EKEY_T,
	EKEY_U,
	EKEY_V,
	EKEY_W,
	EKEY_X,
	EKEY_Y,
	EKEY_Z,

	// 小键盘
	KEY_PAD_0 = 37,    // 从37开始
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,

	// 功能键
	KEY_SPACE = 65,
	EKEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	EKEY_ESCAPE,
	KEY_SCROLLLOCK,
	EKEY_INSERT,
	EKEY_DELETE,
	EKEY_HOME,
	KEY_END,
	EKEY_PAGEUP,
	EKEY_PAGEDOWN,
	KEY_BREAK,

	// 方向键
	KEY_UP = 88,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,

	// F键
	EKEY_F1 = 92,
	EKEY_F2,
	EKEY_F3,
	EKEY_F4,
	EKEY_F5,
	EKEY_F6,
	EKEY_F7,
	EKEY_F8,
	EKEY_F9,
	EKEY_F10,
	EKEY_F11,
	EKEY_F12
};

class KeyConverter {
private:
	static constexpr size_t MAP_SIZE = 256;
	static KeyConverter& instance() {
		static KeyConverter inst;
		return inst;
	}
	int mapping[MAP_SIZE];

	KeyConverter() {
		std::fill(mapping, mapping + MAP_SIZE, 0);

		// 鼠标按键映射 - 使用APEX确认的值
		mapping[0x01] = Leftmousebutton;   // 107
		mapping[0x02] = RightMousebutton;  // 108
		mapping[0x04] = Middlemousebutton; // 109
		mapping[0x05] = Sidemousebutton;   // 110
		mapping[0x06] = Sidemouse2button;  // 111

		// 特殊键 - 使用APEX确认的值
		mapping[VK_LSHIFT] = SHIFTkey - 1;     // 79
		mapping[VK_LMENU] = ALTkey - 1;        // 81
		mapping[VK_LCONTROL] = CTRLkey - 1;    // 83

		// 数字键映射 (0-9)
		for (int i = 0; i <= 9; i++) {
			mapping[0x30 + i] = KEY_0 + i;  // 主键盘数字
			mapping[VK_NUMPAD0 + i] = KEY_PAD_0 + i; // 小键盘数字
		}

		// 字母键映射 (A-Z)
		for (int i = 0; i < 26; i++) {
			mapping[0x41 + i] = EKEY_A + i;
		}

		// 功能键映射
		mapping[VK_SPACE] = KEY_SPACE - 1;     // 空格
		mapping[VK_TAB] = EKEY_TAB;        // TAB
		mapping[VK_CAPITAL] = KEY_CAPSLOCK; // CAPS LOCK
		mapping[VK_NUMLOCK] = KEY_NUMLOCK;  // NUM LOCK
		mapping[VK_ESCAPE] = EKEY_ESCAPE;   // ESC
		mapping[VK_SCROLL] = KEY_SCROLLLOCK;// SCROLL LOCK
		mapping[VK_INSERT] = EKEY_INSERT;   // INSERT
		mapping[VK_DELETE] = EKEY_DELETE;   // DELETE
		mapping[VK_HOME] = EKEY_HOME;       // HOME
		mapping[VK_END] = KEY_END;          // END
		mapping[VK_PRIOR] = EKEY_PAGEUP;    // PAGE UP
		mapping[VK_NEXT] = EKEY_PAGEDOWN;   // PAGE DOWN

		// 方向键映射
		mapping[VK_UP] = KEY_UP;           // 上
		mapping[VK_LEFT] = KEY_LEFT;       // 左
		mapping[VK_DOWN] = KEY_DOWN;       // 下
		mapping[VK_RIGHT] = KEY_RIGHT;     // 右

		// 小键盘运算符
		mapping[VK_DIVIDE] = KEY_PAD_DIVIDE;    // 除
		mapping[VK_MULTIPLY] = KEY_PAD_MULTIPLY;// 乘
		mapping[VK_SUBTRACT] = KEY_PAD_MINUS;   // 减
		mapping[VK_ADD] = KEY_PAD_PLUS;        // 加
		mapping[VK_DECIMAL] = KEY_PAD_DECIMAL;  // 小数点

		// F1-F12映射
		for (int i = 0; i < 12; i++) {
			mapping[VK_F1 + i] = EKEY_F1 + i - 1;
		}
	}

public:
	static inline int virkey_to_inputsystem(int key) {
		return instance().mapping[key] + 1;
	}
};

inline bool is_gamepad_value(int code) {
	// 检查是否为手柄按键的APEX输入系统值
	return (
		(code >= 115 && code <= 122) || // 面部按钮、肩部按钮和功能按钮
		(code >= 126 && code <= 127) || // 扳机键
		(code >= 243 && code <= 246)    // 十字键
		);
}

// 全局函数
inline int virkey_to_inputsystem(int key) {

	if (is_gamepad_value(key)) {
		return key;
	}
	return KeyConverter::virkey_to_inputsystem(key);
}