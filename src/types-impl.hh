// ----------------------------------------------------------------------------
// Copyright © 2014-2023 Konstantin Shmelkov <mrcashe@gmail.com>.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
// OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#ifndef TAU_TYPES_IMPL_HH
#define TAU_TYPES_IMPL_HH

#include <tau/types.hh>
#include <memory>

namespace tau {

class Absolute_impl;
using Absolute_ptr = std::shared_ptr<Absolute_impl>;
using Absolute_cptr = std::shared_ptr<const Absolute_impl>;

class Bin_impl;
using Bin_ptr = std::shared_ptr<Bin_impl>;
using Bin_cptr = std::shared_ptr<const Bin_impl>;

class Box_impl;
using Box_ptr = std::shared_ptr<Box_impl>;
using Box_cptr = std::shared_ptr<const Box_impl>;

class Button_base_impl;
using Button_base_ptr = std::shared_ptr<Button_base_impl>;
using Button_base_cptr = std::shared_ptr<const Button_base_impl>;

class Button_impl;
using Button_ptr = std::shared_ptr<Button_impl>;
using Button_cptr = std::shared_ptr<const Button_impl>;

class Card_impl;
using Card_ptr = std::shared_ptr<Card_impl>;
using Card_cptr = std::shared_ptr<const Card_impl>;

class Check_impl;
using Check_ptr = std::shared_ptr<Check_impl>;
using Check_cptr = std::shared_ptr<const Check_impl>;

class Check_menu_impl;
using Check_menu_ptr = std::shared_ptr<Check_menu_impl>;
using Check_menu_cptr = std::shared_ptr<const Check_menu_impl>;

class Colorsel_impl;
using Colorsel_ptr = std::shared_ptr<Colorsel_impl>;
using Colorsel_cptr = std::shared_ptr<const Colorsel_impl>;

class Container_impl;

class Counter_impl;
using Counter_ptr = std::shared_ptr<Counter_impl>;
using Counter_cptr = std::shared_ptr<const Counter_impl>;

class Cycle_impl;
using Cycle_ptr = std::shared_ptr<Cycle_impl>;
using Cycle_cptr = std::shared_ptr<const Cycle_impl>;

class Cycle_text_impl;
using Cycle_text_ptr = std::shared_ptr<Cycle_text_impl>;
using Cycle_text_cptr = std::shared_ptr<const Cycle_text_impl>;

class Doc_impl;
using Doc_impl_ptr = std::shared_ptr<Doc_impl>;
using Doc_impl_cptr = std::shared_ptr<const Doc_impl>;

class Edit_impl;
using Edit_ptr = std::shared_ptr<Edit_impl>;
using Edit_cptr = std::shared_ptr<const Edit_impl>;

class Entry_impl;
using Entry_ptr = std::shared_ptr<Entry_impl>;
using Entry_cptr = std::shared_ptr<const Entry_impl>;

class File_monitor;
using File_monitor_ptr = std::shared_ptr<File_monitor>;

class Fontsel_impl;
using Fontsel_ptr = std::shared_ptr<Fontsel_impl>;
using Fontsel_cptr = std::shared_ptr<const Fontsel_impl>;

class Frame_impl;
using Frame_ptr = std::shared_ptr<Frame_impl>;
using Frame_cptr = std::shared_ptr<const Frame_impl>;

class Icon_impl;
using Icon_ptr = std::shared_ptr<Icon_impl>;
using Icon_cptr = std::shared_ptr<const Icon_impl>;

class Image_impl;
using Image_ptr = std::shared_ptr<Image_impl>;
using Image_cptr = std::shared_ptr<const Image_impl>;

class List_impl;
using List_ptr = std::shared_ptr<List_impl>;
using List_cptr = std::shared_ptr<const List_impl>;

class List_text_impl;
using List_text_ptr = std::shared_ptr<List_text_impl>;
using List_text_cptr = std::shared_ptr<const List_text_impl>;

class Master_glyph;
using Master_glyph_ptr = std::shared_ptr<Master_glyph>;

class Menu_impl;
using Menu_wptr = std::weak_ptr<Menu_impl>;
using Menu_ptr = std::shared_ptr<Menu_impl>;
using Menu_cptr = std::shared_ptr<const Menu_impl>;

class Menubar_impl;
using Menubar_ptr = std::shared_ptr<Menubar_impl>;
using Menubar_cptr = std::shared_ptr<const Menubar_impl>;

class Menubox_impl;
using Menubox_ptr = std::shared_ptr<Menubox_impl>;
using Menubox_cptr = std::shared_ptr<const Menubox_impl>;

class Menu_item_impl;
using Menu_item_ptr = std::shared_ptr<Menu_item_impl>;
using Menu_item_cptr = std::shared_ptr<const Menu_item_impl>;

class Navigator_impl;
using Navigator_ptr = std::shared_ptr<Navigator_impl>;
using Navigator_cptr = std::shared_ptr<const Navigator_impl>;

class Painter_impl;
using Painter_impl_ptr = std::shared_ptr<Painter_impl>;
using Painter_impl_cptr = std::shared_ptr<const Painter_impl>;

class Roller_impl;
using Roller_ptr = std::shared_ptr<Roller_impl>;
using Roller_cptr = std::shared_ptr<const Roller_impl>;

class Scroller_impl;
using Scroller_ptr = std::shared_ptr<Scroller_impl>;
using Scroller_cptr = std::shared_ptr<const Scroller_impl>;

class Separator_impl;
using Separator_ptr = std::shared_ptr<Separator_impl>;
using Separator_cptr = std::shared_ptr<const Separator_impl>;

class Slider_impl;
using Slider_ptr = std::shared_ptr<Slider_impl>;
using Slider_cptr = std::shared_ptr<const Slider_impl>;

class Submenu_impl;
using Submenu_ptr = std::shared_ptr<Submenu_impl>;
using Submenu_cptr = std::shared_ptr<const Submenu_impl>;

class Table_impl;
using Table_ptr = std::shared_ptr<Table_impl>;
using Table_cptr = std::shared_ptr<const Table_impl>;

class Text_impl;
using Text_ptr = std::shared_ptr<Text_impl>;
using Text_cptr = std::shared_ptr<const Text_impl>;

class Toggle_impl;
using Toggle_ptr = std::shared_ptr<Toggle_impl>;
using Toggle_cptr = std::shared_ptr<const Toggle_impl>;

class Toggle_action_impl;
using Toggle_action_ptr = std::shared_ptr<Toggle_action_impl>;
using Toggle_action_cptr = std::shared_ptr<const Toggle_action_impl>;

class Twins_impl;
using Twins_ptr = std::shared_ptr<Twins_impl>;

class Window_impl;
using Window_ptr = std::shared_ptr<Window_impl>;
using Window_cptr = std::shared_ptr<const Window_impl>;

class Winface;
using Winface_ptr = std::shared_ptr<Winface>;
using Winface_cptr = std::shared_ptr<const Winface>;

class Toplevel_impl;
using Toplevel_ptr = std::shared_ptr<Toplevel_impl>;
using Toplevel_cptr = std::shared_ptr<const Toplevel_impl>;

class Popup_impl;
using Popup_ptr = std::shared_ptr<Popup_impl>;
using Popup_cptr = std::shared_ptr<const Popup_impl>;

class Dialog_impl;
using Dialog_ptr = std::shared_ptr<Dialog_impl>;
using Dialog_cptr = std::shared_ptr<const Dialog_impl>;

} // namespace tau

#endif // TAU_TYPES_IMPL_HH
