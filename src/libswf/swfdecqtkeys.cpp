/******************************************************************************
 * Copyright (C) 2007  Michael Hofmann <mh21@piware.de>                       *
 *                                                                            *
 * This library is free software; you can redistribute it and/or modify       *
 * it under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation; either version 2.1 of the License, or     *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Lesser General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with this program; if not, write to the Free Software Foundation,    *
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.          *
 ******************************************************************************/

#include "swfdecqtkeys.h"

#include <QMap>
#include <Qt>

class KeyMap : public QMap<int, SwfdecKey>
{
public:
    KeyMap()
    {
	insert (Qt::Key_Escape, SWFDEC_KEY_ESCAPE);
	insert (Qt::Key_Tab, SWFDEC_KEY_TAB);
	// insert (Qt::Key_Backtab, .....);
	insert (Qt::Key_Backspace, SWFDEC_KEY_BACKSPACE);
	insert (Qt::Key_Return, SWFDEC_KEY_ENTER);
	insert (Qt::Key_Enter, SWFDEC_KEY_ENTER);
	insert (Qt::Key_Insert, SWFDEC_KEY_INSERT);
	insert (Qt::Key_Delete, SWFDEC_KEY_DELETE);
	// insert (Qt::Key_Pause, .....);
	// insert (Qt::Key_Print, .....);
	// insert (Qt::Key_SysReq, .....);
	insert (Qt::Key_Clear, SWFDEC_KEY_CLEAR);
	insert (Qt::Key_Home, SWFDEC_KEY_HOME);
	insert (Qt::Key_End, SWFDEC_KEY_END);
	insert (Qt::Key_Left, SWFDEC_KEY_LEFT);
	insert (Qt::Key_Up, SWFDEC_KEY_UP);
	insert (Qt::Key_Right, SWFDEC_KEY_RIGHT);
	insert (Qt::Key_Down, SWFDEC_KEY_DOWN);
	insert (Qt::Key_PageUp, SWFDEC_KEY_PAGE_UP);
	insert (Qt::Key_PageDown, SWFDEC_KEY_PAGE_DOWN);
	insert (Qt::Key_Shift, SWFDEC_KEY_SHIFT);
	insert (Qt::Key_Control, SWFDEC_KEY_CONTROL);
	// insert (Qt::Key_Meta, .....);
	insert (Qt::Key_Alt, SWFDEC_KEY_ALT);
	insert (Qt::Key_AltGr, SWFDEC_KEY_ALT);
	insert (Qt::Key_CapsLock, SWFDEC_KEY_CAPS_LOCK);
	insert (Qt::Key_NumLock, SWFDEC_KEY_NUM_LOCK);
	// insert (Qt::Key_ScrollLock, .....);
	insert (Qt::Key_F1, SWFDEC_KEY_F1);
	insert (Qt::Key_F2, SWFDEC_KEY_F2);
	insert (Qt::Key_F3, SWFDEC_KEY_F3);
	insert (Qt::Key_F4, SWFDEC_KEY_F4);
	insert (Qt::Key_F5, SWFDEC_KEY_F5);
	insert (Qt::Key_F6, SWFDEC_KEY_F6);
	insert (Qt::Key_F7, SWFDEC_KEY_F7);
	insert (Qt::Key_F8, SWFDEC_KEY_F8);
	insert (Qt::Key_F9, SWFDEC_KEY_F9);
	insert (Qt::Key_F10, SWFDEC_KEY_F10);
	insert (Qt::Key_F11, SWFDEC_KEY_F11);
	insert (Qt::Key_F12, SWFDEC_KEY_F12);
	insert (Qt::Key_F13, SWFDEC_KEY_F13);
	insert (Qt::Key_F14, SWFDEC_KEY_F14);
	insert (Qt::Key_F15, SWFDEC_KEY_F15);
	// insert (Qt::Key_F16, .....);
	// insert (Qt::Key_F17, .....);
	// insert (Qt::Key_F18, .....);
	// insert (Qt::Key_F19, .....);
	// insert (Qt::Key_F20, .....);
	// insert (Qt::Key_F21, .....);
	// insert (Qt::Key_F22, .....);
	// insert (Qt::Key_F23, .....);
	// insert (Qt::Key_F24, .....);
	// insert (Qt::Key_F25, .....);
	// insert (Qt::Key_F26, .....);
	// insert (Qt::Key_F27, .....);
	// insert (Qt::Key_F28, .....);
	// insert (Qt::Key_F29, .....);
	// insert (Qt::Key_F30, .....);
	// insert (Qt::Key_F31, .....);
	// insert (Qt::Key_F32, .....);
	// insert (Qt::Key_F33, .....);
	// insert (Qt::Key_F34, .....);
	// insert (Qt::Key_F35, .....);
	// insert (Qt::Key_Super_L, .....);
	// insert (Qt::Key_Super_R, .....);
	// insert (Qt::Key_Menu, .....);
	// insert (Qt::Key_Hyper_L, .....);
	// insert (Qt::Key_Hyper_R, .....);
	insert (Qt::Key_Help, SWFDEC_KEY_HELP);
	// insert (Qt::Key_Direction_L, .....);
	// insert (Qt::Key_Direction_R, .....);
	insert (Qt::Key_Space, SWFDEC_KEY_SPACE);
	// insert (Qt::Key_Any, .....);
	// insert (Qt::Key_Exclam, .....);
	// insert (Qt::Key_QuoteDbl, .....);
	// insert (Qt::Key_NumberSign, .....);
	// insert (Qt::Key_Dollar, .....);
	// insert (Qt::Key_Percent, .....);
	// insert (Qt::Key_Ampersand, .....);
	insert (Qt::Key_Apostrophe, SWFDEC_KEY_APOSTROPHE);
	// insert (Qt::Key_ParenLeft, .....);
	// insert (Qt::Key_ParenRight, .....);
	insert (Qt::Key_Asterisk, SWFDEC_KEY_NUMPAD_MULTIPLY);
	insert (Qt::Key_Plus, SWFDEC_KEY_NUMPAD_ADD);
	// insert (Qt::Key_Comma, .....);
	insert (Qt::Key_Minus, SWFDEC_KEY_MINUS);
	// insert (Qt::Key_Period, .....);
	insert (Qt::Key_Slash, SWFDEC_KEY_SLASH);
	insert (Qt::Key_0, SWFDEC_KEY_0);
	insert (Qt::Key_1, SWFDEC_KEY_1);
	insert (Qt::Key_2, SWFDEC_KEY_2);
	insert (Qt::Key_3, SWFDEC_KEY_3);
	insert (Qt::Key_4, SWFDEC_KEY_4);
	insert (Qt::Key_5, SWFDEC_KEY_5);
	insert (Qt::Key_6, SWFDEC_KEY_6);
	insert (Qt::Key_7, SWFDEC_KEY_7);
	insert (Qt::Key_8, SWFDEC_KEY_8);
	insert (Qt::Key_9, SWFDEC_KEY_9);
	// insert (Qt::Key_Colon, .....);
	insert (Qt::Key_Semicolon, SWFDEC_KEY_SEMICOLON);
	// insert (Qt::Key_Less, .....);
	insert (Qt::Key_Equal, SWFDEC_KEY_EQUAL);
	// insert (Qt::Key_Greater, .....);
	// insert (Qt::Key_Question, .....);
	// insert (Qt::Key_At, .....);
	insert (Qt::Key_A, SWFDEC_KEY_A);
	insert (Qt::Key_B, SWFDEC_KEY_B);
	insert (Qt::Key_C, SWFDEC_KEY_C);
	insert (Qt::Key_D, SWFDEC_KEY_D);
	insert (Qt::Key_E, SWFDEC_KEY_E);
	insert (Qt::Key_F, SWFDEC_KEY_F);
	insert (Qt::Key_G, SWFDEC_KEY_G);
	insert (Qt::Key_H, SWFDEC_KEY_H);
	insert (Qt::Key_I, SWFDEC_KEY_I);
	insert (Qt::Key_J, SWFDEC_KEY_J);
	insert (Qt::Key_K, SWFDEC_KEY_K);
	insert (Qt::Key_L, SWFDEC_KEY_L);
	insert (Qt::Key_M, SWFDEC_KEY_M);
	insert (Qt::Key_N, SWFDEC_KEY_N);
	insert (Qt::Key_O, SWFDEC_KEY_O);
	insert (Qt::Key_P, SWFDEC_KEY_P);
	insert (Qt::Key_Q, SWFDEC_KEY_Q);
	insert (Qt::Key_R, SWFDEC_KEY_R);
	insert (Qt::Key_S, SWFDEC_KEY_S);
	insert (Qt::Key_T, SWFDEC_KEY_T);
	insert (Qt::Key_U, SWFDEC_KEY_U);
	insert (Qt::Key_V, SWFDEC_KEY_V);
	insert (Qt::Key_W, SWFDEC_KEY_W);
	insert (Qt::Key_X, SWFDEC_KEY_X);
	insert (Qt::Key_Y, SWFDEC_KEY_Y);
	insert (Qt::Key_Z, SWFDEC_KEY_Z);
	insert (Qt::Key_BracketLeft, SWFDEC_KEY_LEFT_BRACKET);
	insert (Qt::Key_Backslash, SWFDEC_KEY_BACKSLASH);
	insert (Qt::Key_BracketRight, SWFDEC_KEY_RIGHT_BRACKET);
	// insert (Qt::Key_AsciiCircum, .....);
	// insert (Qt::Key_Underscore, .....);
	// insert (Qt::Key_QuoteLeft, .....);
	// insert (Qt::Key_BraceLeft, .....);
	// insert (Qt::Key_Bar, .....);
	// insert (Qt::Key_BraceRight, .....);
	// insert (Qt::Key_AsciiTilde, .....);
	// insert (Qt::Key_nobreakspace, .....);
	// insert (Qt::Key_exclamdown, .....);
	// insert (Qt::Key_cent, .....);
	// insert (Qt::Key_sterling, .....);
	// insert (Qt::Key_currency, .....);
	// insert (Qt::Key_yen, .....);
	// insert (Qt::Key_brokenbar, .....);
	// insert (Qt::Key_section, .....);
	// insert (Qt::Key_diaeresis, .....);
	// insert (Qt::Key_copyright, .....);
	// insert (Qt::Key_ordfeminine, .....);
	// insert (Qt::Key_guillemotleft, .....);
	// insert (Qt::Key_notsign, .....);
	// insert (Qt::Key_hyphen, .....);
	// insert (Qt::Key_registered, .....);
	// insert (Qt::Key_macron, .....);
	// insert (Qt::Key_degree, .....);
	// insert (Qt::Key_plusminus, .....);
	// insert (Qt::Key_twosuperior, .....);
	// insert (Qt::Key_threesuperior, .....);
	// insert (Qt::Key_acute, .....);
	// insert (Qt::Key_mu, .....);
	// insert (Qt::Key_paragraph, .....);
	// insert (Qt::Key_periodcentered, .....);
	// insert (Qt::Key_cedilla, .....);
	// insert (Qt::Key_onesuperior, .....);
	// insert (Qt::Key_masculine, .....);
	// insert (Qt::Key_guillemotright, .....);
	// insert (Qt::Key_onequarter, .....);
	// insert (Qt::Key_onehalf, .....);
	// insert (Qt::Key_threequarters, .....);
	// insert (Qt::Key_questiondown, .....);
	// insert (Qt::Key_Agrave, .....);
	// insert (Qt::Key_Aacute, .....);
	// insert (Qt::Key_Acircumflex, .....);
	// insert (Qt::Key_Atilde, .....);
	// insert (Qt::Key_Adiaeresis, .....);
	// insert (Qt::Key_Aring, .....);
	// insert (Qt::Key_AE, .....);
	// insert (Qt::Key_Ccedilla, .....);
	// insert (Qt::Key_Egrave, .....);
	// insert (Qt::Key_Eacute, .....);
	// insert (Qt::Key_Ecircumflex, .....);
	// insert (Qt::Key_Ediaeresis, .....);
	// insert (Qt::Key_Igrave, .....);
	// insert (Qt::Key_Iacute, .....);
	// insert (Qt::Key_Icircumflex, .....);
	// insert (Qt::Key_Idiaeresis, .....);
	// insert (Qt::Key_ETH, .....);
	// insert (Qt::Key_Ntilde, .....);
	// insert (Qt::Key_Ograve, .....);
	// insert (Qt::Key_Oacute, .....);
	// insert (Qt::Key_Ocircumflex, .....);
	// insert (Qt::Key_Otilde, .....);
	// insert (Qt::Key_Odiaeresis, .....);
	// insert (Qt::Key_multiply, .....);
	// insert (Qt::Key_Ooblique, .....);
	// insert (Qt::Key_Ugrave, .....);
	// insert (Qt::Key_Uacute, .....);
	// insert (Qt::Key_Ucircumflex, .....);
	// insert (Qt::Key_Udiaeresis, .....);
	// insert (Qt::Key_Yacute, .....);
	// insert (Qt::Key_THORN, .....);
	// insert (Qt::Key_ssharp, .....);
	// insert (Qt::Key_division, .....);
	// insert (Qt::Key_ydiaeresis, .....);
	// insert (Qt::Key_Multi_key, .....);
	// insert (Qt::Key_Codeinput, .....);
	// insert (Qt::Key_SingleCandidate, .....);
	// insert (Qt::Key_MultipleCandidate, .....);
	// insert (Qt::Key_PreviousCandidate, .....);
	// insert (Qt::Key_Mode_switch, .....);
	// insert (Qt::Key_Kanji, .....);
	// insert (Qt::Key_Muhenkan, .....);
	// insert (Qt::Key_Henkan, .....);
	// insert (Qt::Key_Romaji, .....);
	// insert (Qt::Key_Hiragana, .....);
	// insert (Qt::Key_Katakana, .....);
	// insert (Qt::Key_Hiragana_Katakana, .....);
	// insert (Qt::Key_Zenkaku, .....);
	// insert (Qt::Key_Hankaku, .....);
	// insert (Qt::Key_Zenkaku_Hankaku, .....);
	// insert (Qt::Key_Touroku, .....);
	// insert (Qt::Key_Massyo, .....);
	// insert (Qt::Key_Kana_Lock, .....);
	// insert (Qt::Key_Kana_Shift, .....);
	// insert (Qt::Key_Eisu_Shift, .....);
	// insert (Qt::Key_Eisu_toggle, .....);
	// insert (Qt::Key_Hangul, .....);
	// insert (Qt::Key_Hangul_Start, .....);
	// insert (Qt::Key_Hangul_End, .....);
	// insert (Qt::Key_Hangul_Hanja, .....);
	// insert (Qt::Key_Hangul_Jamo, .....);
	// insert (Qt::Key_Hangul_Romaja, .....);
	// insert (Qt::Key_Hangul_Jeonja, .....);
	// insert (Qt::Key_Hangul_Banja, .....);
	// insert (Qt::Key_Hangul_PreHanja, .....);
	// insert (Qt::Key_Hangul_PostHanja, .....);
	// insert (Qt::Key_Hangul_Special, .....);
	insert (Qt::Key_Dead_Grave, SWFDEC_KEY_GRAVE);
	// insert (Qt::Key_Dead_Acute, .....);
	// insert (Qt::Key_Dead_Circumflex, .....);
	// insert (Qt::Key_Dead_Tilde, .....);
	// insert (Qt::Key_Dead_Macron, .....);
	// insert (Qt::Key_Dead_Breve, .....);
	// insert (Qt::Key_Dead_Abovedot, .....);
	// insert (Qt::Key_Dead_Diaeresis, .....);
	// insert (Qt::Key_Dead_Abovering, .....);
	// insert (Qt::Key_Dead_Doubleacute, .....);
	// insert (Qt::Key_Dead_Caron, .....);
	// insert (Qt::Key_Dead_Cedilla, .....);
	// insert (Qt::Key_Dead_Ogonek, .....);
	// insert (Qt::Key_Dead_Iota, .....);
	// insert (Qt::Key_Dead_Voiced_Sound, .....);
	// insert (Qt::Key_Dead_Semivoiced_Sound, .....);
	// insert (Qt::Key_Dead_Belowdot, .....);
	// insert (Qt::Key_Dead_Hook, .....);
	// insert (Qt::Key_Dead_Horn, .....);
	// insert (Qt::Key_Back, .....);
	// insert (Qt::Key_Forward, .....);
	// insert (Qt::Key_Stop, .....);
	// insert (Qt::Key_Refresh, .....);
	// insert (Qt::Key_VolumeDown, .....);
	// insert (Qt::Key_VolumeMute, .....);
	// insert (Qt::Key_VolumeUp, .....);
	// insert (Qt::Key_BassBoost, .....);
	// insert (Qt::Key_BassUp, .....);
	// insert (Qt::Key_BassDown, .....);
	// insert (Qt::Key_TrebleUp, .....);
	// insert (Qt::Key_TrebleDown, .....);
	// insert (Qt::Key_MediaPlay, .....);
	// insert (Qt::Key_MediaStop, .....);
	// insert (Qt::Key_MediaPrevious, .....);
	// insert (Qt::Key_MediaNext, .....);
	// insert (Qt::Key_MediaRecord, .....);
	// insert (Qt::Key_HomePage, .....);
	// insert (Qt::Key_Favorites, .....);
	// insert (Qt::Key_Search, .....);
	// insert (Qt::Key_Standby, .....);
	// insert (Qt::Key_OpenUrl, .....);
	// insert (Qt::Key_LaunchMail, .....);
	// insert (Qt::Key_LaunchMedia, .....);
	// insert (Qt::Key_Launch0, .....);
	// insert (Qt::Key_Launch1, .....);
	// insert (Qt::Key_Launch2, .....);
	// insert (Qt::Key_Launch3, .....);
	// insert (Qt::Key_Launch4, .....);
	// insert (Qt::Key_Launch5, .....);
	// insert (Qt::Key_Launch6, .....);
	// insert (Qt::Key_Launch7, .....);
	// insert (Qt::Key_Launch8, .....);
	// insert (Qt::Key_Launch9, .....);
	// insert (Qt::Key_LaunchA, .....);
	// insert (Qt::Key_LaunchB, .....);
	// insert (Qt::Key_LaunchC, .....);
	// insert (Qt::Key_LaunchD, .....);
	// insert (Qt::Key_LaunchE, .....);
	// insert (Qt::Key_LaunchF, .....);
	// insert (Qt::Key_MediaLast, .....);
	// insert (Qt::Key_unknown, .....);
	// insert (Qt::Key_Call, .....);
	// insert (Qt::Key_Context1, .....);
	// insert (Qt::Key_Context2, .....);
	// insert (Qt::Key_Context3, .....);
	// insert (Qt::Key_Context4, .....);
	// insert (Qt::Key_Flip, .....);
	// insert (Qt::Key_Hangup, .....);
	// insert (Qt::Key_No, .....);
	// insert (Qt::Key_Select, .....);
	// insert (Qt::Key_Yes, .....);
	// insert (Qt::Key_Execute, .....);
	// insert (Qt::Key_Printer, .....);
	// insert (Qt::Key_Play, .....);
	// insert (Qt::Key_Sleep, .....);
	// insert (Qt::Key_Zoom, .....);
	// insert (Qt::Key_Cancel, .....);

        // insert (....., SWFDEC_KEY_NUMPAD_0);
        // insert (....., SWFDEC_KEY_NUMPAD_1);
        // insert (....., SWFDEC_KEY_NUMPAD_2);
        // insert (....., SWFDEC_KEY_NUMPAD_3);
        // insert (....., SWFDEC_KEY_NUMPAD_4);
        // insert (....., SWFDEC_KEY_NUMPAD_5);
        // insert (....., SWFDEC_KEY_NUMPAD_6);
        // insert (....., SWFDEC_KEY_NUMPAD_7);
        // insert (....., SWFDEC_KEY_NUMPAD_8);
        // insert (....., SWFDEC_KEY_NUMPAD_9);
        // insert (....., SWFDEC_KEY_NUMPAD_SUBTRACT);
        // insert (....., SWFDEC_KEY_NUMPAD_DECIMAL);
        // insert (....., SWFDEC_KEY_NUMPAD_DIVIDE);
    }
};

Q_GLOBAL_STATIC (KeyMap, keyMap)

SwfdecKey swfdecQtKeyMapping (int key)
{
    return keyMap()->value (key);
}
