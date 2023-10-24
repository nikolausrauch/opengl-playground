#include "keyboard.h"

#include <array>

namespace core
{

const char* keyboard::key_name(key key)
{
    static constexpr std::array<const char*, static_cast<int>(key::last) + 1> s_name =
    {
            "unkown", "space", "apostrophe", "comma", "minus", "period", "slash",
            "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
            "semicolon", "equal",
            "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
            "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
            "left bracket", "backlash", "right bracket", "grave accent", "world 1", "world 2",

            "escape", "enter", "tab", "backspace", "insert", "delete", "right", "left", "down", "up",
            "page up", "page down", "home", "end", "caps lock", "scroll lock", "num lock", "print screen", "pause",

            "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
            "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20",
            "F21", "F22", "F23", "F24", "F25",

            "KP 0", "KP 1", "KP 2", "KP 3", "KP 4", "KP 5", "KP 6", "KP 7", "KP 8", "KP 9",
            "KP decimanl", "KP divide", "KP multiply", "KP subtract", "KP add", "KP enter", "KP equal",
            "left shift", "left control", "left alt", "left super",
            "right shift", "right control", "right alt", "right super",
            "menu"
    };

    return s_name[static_cast<int>(key)];
}

}
