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

/// @file types.hh Forward class declarations and smart pointer's declarations.

#ifndef TAU_TYPES_HH
#define TAU_TYPES_HH

#include <memory>

namespace tau {

class Accel;

class Action_menu_item;

class Action;
class Action_base;

class Base64_encoder;
class Base64_decoder;

class Brush;
class Brush_impl;
using Brush_ptr = std::shared_ptr<Brush_impl>;
using Brush_cptr = std::shared_ptr<const Brush_impl>;

class Buffer;
struct Buffer_impl;
using Buffer_ptr = std::shared_ptr<Buffer_impl>;
using Buffer_cptr = std::shared_ptr<const Buffer_impl>;

class Buffer_citer;
class Buffer_citer_impl;

class Check;
class Check_menu_item;
class Color;
class Color_style;
class Container;
class Contour;

class Cursor;
class Cursor_impl;
using Cursor_ptr = std::shared_ptr<Cursor_impl>;
using Cursor_cptr = std::shared_ptr<const Cursor_impl>;

class Curve;

class Data_element;
struct Data_element_impl;
using Data_element_ptr = std::shared_ptr<Data_element_impl>;
using Data_element_cptr = std::shared_ptr<const Data_element_impl>;

class Decl_element;
struct Decl_element_impl;
using Decl_element_ptr = std::shared_ptr<Decl_element_impl>;
using Decl_element_cptr = std::shared_ptr<const Decl_element_impl>;

class Display;
class Display_impl;
using Display_ptr = std::shared_ptr<Display_impl>;
using Display_cptr = std::shared_ptr<const Display_impl>;

class Doc;
class Doc_impl;
using Doc_ptr = std::shared_ptr<Doc_impl>;
using Doc_cptr = std::shared_ptr<const Doc_impl>;

class Doctype;
struct Doctype_impl;
using Doctype_ptr = std::shared_ptr<Doctype_impl>;
using Doctype_cptr = std::shared_ptr<const Doctype_impl>;

class Element;
struct Element_impl;
using Element_ptr = std::shared_ptr<Element_impl>;
using Element_cptr = std::shared_ptr<const Element_impl>;

class Encoding;

class Event;
class Event_impl;
using Event_ptr = std::shared_ptr<Event_impl>;
using Event_cptr = std::shared_ptr<const Event_impl>;

class Fileinfo;
class Fileinfo_impl;
using Fileinfo_ptr = std::shared_ptr<Fileinfo_impl>;
using Fileinfo_cptr = std::shared_ptr<const Fileinfo_impl>;

class Fileman;
class Fileman_impl;
using Fileman_ptr = std::shared_ptr<Fileman_impl>;
using Fileman_cptr = std::shared_ptr<const Fileman_impl>;

class Font;
class Font_impl;
using Font_ptr = std::shared_ptr<Font_impl>;
using Font_cptr = std::shared_ptr<const Font_impl>;

class Font_style;

class Glyph;
class Glyph_impl;
using Glyph_ptr = std::shared_ptr<Glyph_impl>;
using Glyph_cptr = std::shared_ptr<const Glyph_impl>;

class Inst_element;
struct Inst_element_impl;
using Inst_element_ptr = std::shared_ptr<Inst_element_impl>;
using Inst_element_cptr = std::shared_ptr<const Inst_element_impl>;

class Key_file;
struct Key_file_impl;
struct Key_section;

class Language;
class Locale;

class Loop;
class Loop_impl;
using Loop_ptr = std::shared_ptr<Loop_impl>;
using Loop_cptr = std::shared_ptr<const Loop_impl>;

class Master_action;
class Matrix;

class Menu;
class Menu_item;
class Menubar;
class Menubox;

class Node_element;
struct Node_element_impl;
using Node_element_ptr = std::shared_ptr<Node_element_impl>;
using Node_element_cptr = std::shared_ptr<const Node_element_impl>;

class Painter;
class Painter_impl;
using Painter_ptr = std::shared_ptr<Painter_impl>;
using Painter_cptr = std::shared_ptr<const Painter_impl>;

class Pen;
struct Pen_impl;
using Pen_ptr = std::shared_ptr<Pen_impl>;
using Pen_cptr = std::shared_ptr<const Pen_impl>;

class Pixmap;
class Pixmap_impl;
using Pixmap_ptr = std::shared_ptr<Pixmap_impl>;
using Pixmap_cptr = std::shared_ptr<const Pixmap_impl>;

class Point;
class Rect;
class Size;

class Style;
struct Style_impl;

class Territory;

class Text_element;
struct Text_element_impl;
using Text_element_ptr = std::shared_ptr<Text_element_impl>;
using Text_element_cptr = std::shared_ptr<const Text_element_impl>;

class Timer;
struct Timer_impl;
using Timer_ptr = std::shared_ptr<Timer_impl>;
using Timer_cptr = std::shared_ptr<const Timer_impl>;

class Theme;
class Theme_impl;
using Theme_ptr = std::shared_ptr<Theme_impl>;
using Theme_cptr = std::shared_ptr<const Theme_impl>;

class Toggle_action;
class Toplevel;
class ustring;
class Vector;

class Widget;
class Widget_impl;
using Widget_ptr = std::shared_ptr<Widget_impl>;
using Widget_cptr = std::shared_ptr<const Widget_impl>;

} // namespace tau

#endif // TAU_TYPES_HH
