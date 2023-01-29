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

// Doxygen source file.

namespace tau {

/**
@page Resources
Stuff that didn't find yet better location

@section icon_sect Standard Icons
Standard icon names used by library.
Some of icons are compliant to XDG icon name standards (https://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html).
Some are special, like "picto-*".
The presence of these icons is mandatory for normal library work.
The below information related to following methods:
- Theme::find_icon()
- Theme::get_icon()
- Theme::find_pixmap()

Library provide constant access string variables containing actual icon names. You can use these strings
to prevent typos while source text edit and strict compile time icon name check. Of course, you can also use
the actual values. The variable name consist of "ICON_" prefix and all uppercased letters from icon name,
where dash characters ('-') replaced by underscores ('_').
Here is summary of all possible at the moment values.
<table>
<tr><th>String variable providing icon name         <th>Actual icon name        <th>Classes using icon            <th>Is Standard?  <th>%Icon sample
<tr><td>@link ICON_CONFIGURE               @endlink <td>configure               <td>Fileman                       <td>YES           <td> @image html configure.png
<tr><td>@link ICON_DIALOG_CANCEL           @endlink <td>dialog-cancel           <td>Fileman, Colorsel, Fontsel    <td>YES           <td> @image html dialog-cancel.png
<tr><td>@link ICON_DIALOG_OK               @endlink <td>dialog-ok               <td>Fontsel, Colorsel, Fileman    <td>YES           <td> @image html dialog-ok.png
<tr><td>@link ICON_DRIVE_OPTICAL           @endlink <td>drive-optical           <td>Fileman                       <td>YES           <td> @image html drive-optical.png
<tr><td>@link ICON_DRIVE_HARDDISK          @endlink <td>drive-harddisk          <td>Fileman                       <td>YES           <td> @image html drive-harddisk.png
<tr><td>@link ICON_DRIVE_REMOVABLE_MEDIA   @endlink <td>drive-removable-media   <td>Fileman                       <td>YES           <td> @image html drive-removable-media.png
<tr><td>@link ICON_FOLDER                  @endlink <td>folder                  <td>Fileman, Navigator            <td>YES           <td> @image html folder.png
<tr><td>@link ICON_FOLDER_NEW              @endlink <td>folder-new              <td>Fileman                       <td>YES           <td> @image html folder-new.png
<tr><td>@link ICON_FOLDER_REMOTE           @endlink <td>folder-remote           <td>Fileman                       <td>YES           <td> @image html folder-remote.png
<tr><td>@link ICON_GO_HOME                 @endlink <td>go-home                 <td>Fileman                       <td>YES           <td> @image html go-home.png
<tr><td>@link ICON_GO_NEXT                 @endlink <td>go-next                 <td>Fileman                       <td>YES           <td> @image html go-next.png
<tr><td>@link ICON_GO_PREVIOUS             @endlink <td>go-previous             <td>Fileman                       <td>YES           <td> @image html go-previous.png
<tr><td>@link ICON_GO_UP                   @endlink <td>go-up                   <td>Fileman                       <td>YES           <td> @image html go-up.png
<tr><td>@link ICON_PICTO_DEC               @endlink <td>picto-dec               <td>Cycle, Counter                <td>NO            <td> @image html picto-dec.png
<tr><td>@link ICON_PICTO_INC               @endlink <td>picto-inc               <td>Cycle, Counter                <td>NO            <td> @image html picto-inc.png
<tr><td>@link ICON_PICTO_CLOSE             @endlink <td>picto-close             <td>Fileman                       <td>NO            <td> @image html picto-close.png
<tr><td>@link ICON_PICTO_DOWN              @endlink <td>picto-down              <td>Roller, Slider, List          <td>NO            <td> @image html picto-down.png
<tr><td>@link ICON_PICTO_LEFT              @endlink <td>picto-left              <td>Roller, Slider                <td>NO            <td> @image html picto-left.png
<tr><td>@link ICON_PICTO_RIGHT             @endlink <td>picto-right             <td>Roller, Slider, Menu_item     <td>NO            <td> @image html picto-right.png
<tr><td>@link ICON_PICTO_UP                @endlink <td>picto-up                <td>Roller, Slider, List          <td>NO            <td> @image html picto-up.png
<tr><td>@link ICON_UNKNOWN                 @endlink <td>unknown                 <td>Navigator                     <td>NO            <td> @image html unknown.png
<tr><td>@link ICON_VIEW_REFRESH            @endlink <td>view-refresh            <td>Fileman                       <td>YES           <td> @image html view-refresh.png
</table>

@section color_sect Color Names
The color management done with the Color class.
It can accept also values based on textual data. Color can be defined in HTML notation (@#RGB or @#RRGGBB) or a
CSS color name. Despite the fact that these values are standardized (https://www.w3docs.com/learn-css/css-color-names.html),
I found it useful to define a constant access string variables containing actual values.
This prevents typos that may occur while editing the source text. The names of these variables consist of
prefix "COLOR_" plus CSS color name with all uppercased letters and where words are separated by underscore
character ('_').
<table>
<tr><th>String variable providing color name             <th>Actual color name
<tr><td>@link COLOR_ALICE_BLUE                  @endlink <td>AliceBlue
<tr><td>@link COLOR_ANTIQUE_WHITE               @endlink <td>AntiqueWhite
<tr><td>@link COLOR_AQUA                        @endlink <td>Aqua
<tr><td>@link COLOR_AQUAMARINE                  @endlink <td>Aquamarine
<tr><td>@link COLOR_AZURE                       @endlink <td>Azure
<tr><td>@link COLOR_BEIGE                       @endlink <td>Beige
<tr><td>@link COLOR_BISQUE                      @endlink <td>Bisque
<tr><td>@link COLOR_BLACK                       @endlink <td>Black
<tr><td>@link COLOR_BLANCHED_ALMOND             @endlink <td>BlanchedAlmond
<tr><td>@link COLOR_BLUE                        @endlink <td>Blue
<tr><td>@link COLOR_BLUE_VIOLET                 @endlink <td>BlueViolet
<tr><td>@link COLOR_BROWN                       @endlink <td>Brown
<tr><td>@link COLOR_BURLY_WOOD                  @endlink <td>BurlyWood
<tr><td>@link COLOR_CADET_BLUE                  @endlink <td>CadetBlue
<tr><td>@link COLOR_CHARTREUSE                  @endlink <td>Chartreuse
<tr><td>@link COLOR_CHOCOLATE                   @endlink <td>Chocolate
<tr><td>@link COLOR_CORAL                       @endlink <td>Coral
<tr><td>@link COLOR_CORNFLOWER_BLUE             @endlink <td>CornflowerBlue
<tr><td>@link COLOR_CORNSILK                    @endlink <td>Cornsilk
<tr><td>@link COLOR_CRIMSON                     @endlink <td>Crimson
<tr><td>@link COLOR_CYAN                        @endlink <td>Cyan
<tr><td>@link COLOR_DARK_BLUE                   @endlink <td>DarkBlue
<tr><td>@link COLOR_DARK_CYAN                   @endlink <td>DarkCyan
<tr><td>@link COLOR_DARK_GOLDEN_ROD             @endlink <td>DarkGoldenRod
<tr><td>@link COLOR_DARK_GRAY                   @endlink <td>DarkGray
<tr><td>@link COLOR_DARK_GREEN                  @endlink <td>DarkGreen
<tr><td>@link COLOR_DARK_KHAKI                  @endlink <td>DarkKhaki
<tr><td>@link COLOR_DARK_MAGENTA                @endlink <td>DarkMagenta
<tr><td>@link COLOR_DARK_OLIVE_GREEN            @endlink <td>DarkOliveGreen
<tr><td>@link COLOR_DARK_ORANGE                 @endlink <td>DarkOrange
<tr><td>@link COLOR_DARK_ORCHID                 @endlink <td>DarkOrchid
<tr><td>@link COLOR_DARK_RED                    @endlink <td>DarkRed
<tr><td>@link COLOR_DARK_SALMON                 @endlink <td>DarkSalmon
<tr><td>@link COLOR_DARK_SEA_GREEN              @endlink <td>DarkSeaGreen
<tr><td>@link COLOR_DARK_SLATE_BLUE             @endlink <td>DarkSlateBlue
<tr><td>@link COLOR_DARK_SLATE_GRAY             @endlink <td>DarkSlateGray
<tr><td>@link COLOR_DARK_TURQUOISE              @endlink <td>DarkTurquoise
<tr><td>@link COLOR_DARK_VIOLET                 @endlink <td>DarkViolet
<tr><td>@link COLOR_DEEP_PINK                   @endlink <td>DeepPink
<tr><td>@link COLOR_DEEP_SKY_BLUE               @endlink <td>DeepSkyBlue
<tr><td>@link COLOR_DIM_GRAY                    @endlink <td>DimGray
<tr><td>@link COLOR_DODGER_BLUE                 @endlink <td>DodgerBlue
<tr><td>@link COLOR_FIRE_BRICK                  @endlink <td>FireBrick
<tr><td>@link COLOR_FLORAL_WHITE                @endlink <td>FloralWhite
<tr><td>@link COLOR_FOREST_GREEN                @endlink <td>ForestGreen
<tr><td>@link COLOR_FUCHSIA                     @endlink <td>Fuchsia
<tr><td>@link COLOR_GAINSBORO                   @endlink <td>Gainsboro
<tr><td>@link COLOR_GHOST_WHITE                 @endlink <td>GhostWhite
<tr><td>@link COLOR_GOLD                        @endlink <td>Gold
<tr><td>@link COLOR_GOLDEN_ROD                  @endlink <td>GoldenRod
<tr><td>@link COLOR_GRAY                        @endlink <td>Gray
<tr><td>@link COLOR_GREEN                       @endlink <td>Green
<tr><td>@link COLOR_GREEN_YELLOW                @endlink <td>GreenYellow
<tr><td>@link COLOR_HONEY_DEW                   @endlink <td>HoneyDew
<tr><td>@link COLOR_HOT_PINK                    @endlink <td>HotPink
<tr><td>@link COLOR_INDIAN_RED                  @endlink <td>IndianRed
<tr><td>@link COLOR_INDIGO                      @endlink <td>Indigo
<tr><td>@link COLOR_IVORY                       @endlink <td>Ivory
<tr><td>@link COLOR_KHAKI                       @endlink <td>Khaki
<tr><td>@link COLOR_LAVENDER                    @endlink <td>Lavender
<tr><td>@link COLOR_LAVENDER_BLUSH              @endlink <td>LavenderBlush
<tr><td>@link COLOR_LAWN_GREEN                  @endlink <td>LawnGreen
<tr><td>@link COLOR_LEMON_CHIFFON               @endlink <td>LemonChiffon
<tr><td>@link COLOR_LIGHT_BLUE                  @endlink <td>LightBlue
<tr><td>@link COLOR_LIGHT_CORAL                 @endlink <td>LightCoral
<tr><td>@link COLOR_LIGHT_CYAN                  @endlink <td>LightCyan
<tr><td>@link COLOR_LIGHT_GOLDEN_ROD_YELLOW     @endlink <td>LightGoldenRodYellow
<tr><td>@link COLOR_LIGHT_GRAY                  @endlink <td>LightGray
<tr><td>@link COLOR_LIGHT_GREEN                 @endlink <td>LightGreen
<tr><td>@link COLOR_LIGHT_PINK                  @endlink <td>LightPink
<tr><td>@link COLOR_LIGHT_SALMON                @endlink <td>LightSalmon
<tr><td>@link COLOR_LIGHT_SEA_GREEN             @endlink <td>LightSeaGreen
<tr><td>@link COLOR_LIGHT_SKY_BLUE              @endlink <td>LightSkyBlue
<tr><td>@link COLOR_LIGHT_SLATE_GRAY            @endlink <td>LightSlateGray
<tr><td>@link COLOR_LIGHT_STEEL_BLUE            @endlink <td>LightSteelBlue
<tr><td>@link COLOR_LIGHT_YELLOW                @endlink <td>LightYellow
<tr><td>@link COLOR_LIME                        @endlink <td>Lime
<tr><td>@link COLOR_LIME_GREEN                  @endlink <td>LimeGreen
<tr><td>@link COLOR_LINEN                       @endlink <td>Linen
<tr><td>@link COLOR_MAGENTA                     @endlink <td>Magenta
<tr><td>@link COLOR_MAROON                      @endlink <td>Maroon
<tr><td>@link COLOR_MEDIUM_AQUA_MARINE          @endlink <td>MediumAquaMarine
<tr><td>@link COLOR_MEDIUM_BLUE                 @endlink <td>MediumBlue
<tr><td>@link COLOR_MEDIUM_ORCHID               @endlink <td>MediumOrchid
<tr><td>@link COLOR_MEDIUM_PURPLE               @endlink <td>MediumPurple
<tr><td>@link COLOR_MEDIUM_SEA_GREEN            @endlink <td>MediumSeaGreen
<tr><td>@link COLOR_MEDIUM_SLATE_BLUE           @endlink <td>MediumSlateBlue
<tr><td>@link COLOR_MEDIUM_SPRING_GREEN         @endlink <td>MediumSpringGreen
<tr><td>@link COLOR_MEDIUM_TURQUOISE            @endlink <td>MediumTurquoise
<tr><td>@link COLOR_MEDIUM_VIOLET_RED           @endlink <td>MediumVioletRed
<tr><td>@link COLOR_MIDNIGHT_BLUE               @endlink <td>MidnightBlue
<tr><td>@link COLOR_MINT_CREAM                  @endlink <td>MintCream
<tr><td>@link COLOR_MISTY_ROSE                  @endlink <td>MistyRose
<tr><td>@link COLOR_MOCCASIN                    @endlink <td>Moccasin
<tr><td>@link COLOR_NAVAJO_WHITE                @endlink <td>NavajoWhite
<tr><td>@link COLOR_NAVY                        @endlink <td>Navy
<tr><td>@link COLOR_OLD_LACE                    @endlink <td>OldLace
<tr><td>@link COLOR_OLIVE                       @endlink <td>Olive
<tr><td>@link COLOR_OLIVE_DRAB                  @endlink <td>OliveDrab
<tr><td>@link COLOR_ORANGE                      @endlink <td>Orange
<tr><td>@link COLOR_ORANGE_RED                  @endlink <td>OrangeRed
<tr><td>@link COLOR_ORCHID                      @endlink <td>Orchid
<tr><td>@link COLOR_PALE_GOLDEN_ROD             @endlink <td>PaleGoldenRod
<tr><td>@link COLOR_PALE_GREEN                  @endlink <td>PaleGreen
<tr><td>@link COLOR_PALE_TURQUOISE              @endlink <td>PaleTurquoise
<tr><td>@link COLOR_PALE_VIOLET_RED             @endlink <td>PaleVioletRed
<tr><td>@link COLOR_PAPAYA_WHIP                 @endlink <td>PapayaWhip
<tr><td>@link COLOR_PEACH_PUFF                  @endlink <td>PeachPuff
<tr><td>@link COLOR_PERU                        @endlink <td>Peru
<tr><td>@link COLOR_PINK                        @endlink <td>Pink
<tr><td>@link COLOR_PLUM                        @endlink <td>Plum
<tr><td>@link COLOR_POWDER_BLUE                 @endlink <td>PowderBlue
<tr><td>@link COLOR_PURPLE                      @endlink <td>Purple
<tr><td>@link COLOR_RED                         @endlink <td>Red
<tr><td>@link COLOR_ROSY_BROWN                  @endlink <td>RosyBrown
<tr><td>@link COLOR_ROYAL_BLUE                  @endlink <td>RoyalBlue
<tr><td>@link COLOR_SADDLE_BROWN                @endlink <td>SaddleBrown
<tr><td>@link COLOR_SALMON                      @endlink <td>Salmon
<tr><td>@link COLOR_SANDY_BROWN                 @endlink <td>SandyBrown
<tr><td>@link COLOR_SEA_GREEN                   @endlink <td>SeaGreen
<tr><td>@link COLOR_SEA_SHELL                   @endlink <td>SeaShell
<tr><td>@link COLOR_SIENNA                      @endlink <td>Sienna
<tr><td>@link COLOR_SILVER                      @endlink <td>Silver
<tr><td>@link COLOR_SKY_BLUE                    @endlink <td>SkyBlue
<tr><td>@link COLOR_SLATE_BLUE                  @endlink <td>SlateBlue
<tr><td>@link COLOR_SLATE_GRAY                  @endlink <td>SlateGray
<tr><td>@link COLOR_SNOW                        @endlink <td>Snow
<tr><td>@link COLOR_SPRING_GREEN                @endlink <td>SpringGreen
<tr><td>@link COLOR_STEEL_BLUE                  @endlink <td>SteelBlue
<tr><td>@link COLOR_TAN                         @endlink <td>Tan
<tr><td>@link COLOR_TEAL                        @endlink <td>Teal
<tr><td>@link COLOR_THISTLE                     @endlink <td>Thistle
<tr><td>@link COLOR_TOMATO                      @endlink <td>Tomato
<tr><td>@link COLOR_TURQUOISE                   @endlink <td>Turquoise
<tr><td>@link COLOR_VIOLET                      @endlink <td>Violet
<tr><td>@link COLOR_WHEAT                       @endlink <td>Wheat
<tr><td>@link COLOR_WHITE                       @endlink <td>White
<tr><td>@link COLOR_WHITE_SMOKE                 @endlink <td>WhiteSmoke
<tr><td>@link COLOR_YELLOW                      @endlink <td>Yellow
<tr><td>@link COLOR_YELLOW_GREEN                @endlink <td>YellowGreen
</table>

@section style_sect Style names, types and built-in (default) values
Style class has string-based item naming scheme.
The below information related to following methods:
- Style::set()
- Style::get()
- Style::font()
- Style::color()
- Style::redirect()
- Style::unset()

Library provide constant access string variables containing actual style item names. You can use these strings
to prevent typos while source text edit and strict compile time icon name check. Of course, you can also use
the actual values. The variable name consist of "STYLE_" prefix and all uppercased letters from icon name,
where slash characters ('/') replaced by underscores ('_').
Here is summary of all possible at the moment values.
<table>
<tr><th>%String variable providing style item name  <th>Actual style item name  <th>Item type   <th>Built-In Value
<tr><td>@link STYLE_FONT                   @endlink <td>font                    <td>font        <td>
<tr><td>@link STYLE_FOREGROUND             @endlink <td>foreground              <td>color       <td>Black
<tr><td>@link STYLE_MENU_FOREGROUND        @endlink <td>menu/foreground         <td>color       <td>DarkSlateGray
<tr><td>@link STYLE_SLIDER_FOREGROUND      @endlink <td>slider/foreground       <td>color       <td>SteelBlue
<tr><td>@link STYLE_PROGRESS_FOREGROUND    @endlink <td>progress/foreground     <td>color       <td>Blue
<tr><td>@link STYLE_ACCEL_FOREGROUND       @endlink <td>accel/foreground        <td>color       <td>Magenta
<tr><td>@link STYLE_BACKGROUND             @endlink <td>background              <td>color       <td>LightGray
<tr><td>@link STYLE_PROGRESS_BACKGROUND    @endlink <td>progress/background     <td>color       <td>DeepSkyBlue
<tr><td>@link STYLE_WHITESPACE_BACKGROUND  @endlink <td>whitespace/background   <td>color       <td>Snow
<tr><td>@link STYLE_MENU_BACKGROUND        @endlink <td>menu/background         <td>color       <td>Silver
<tr><td>@link STYLE_SELECT_BACKGROUND      @endlink <td>select/background       <td>color       <td>DeepSkyBlue
<tr><td>@link STYLE_TOOLTIP_BACKGROUND     @endlink <td>tooltip/background      <td>color       <td>Aquamarine
<tr><td>@link STYLE_BUTTON_BACKGROUND      @endlink <td>button/background       <td>color       <td>Gainsboro
<tr><td>@link STYLE_SLIDER_BACKGROUND      @endlink <td>slider/background       <td>color       <td>DarkGray
</table>

@section action_sect Standard Actions
Standard action names used by library.

Actions provide:
- Key bindings
- Standard icon names
- Labels
- Tooltips

Some actions provide more than one keyboard accelerator, in such cases
key bindings shown here separated by spaces.

To obtain a standard action, use Theme::find_action() method.

Library provide constant access string variables containing actual standard actions names. You can use these strings
to prevent typos while source text edit and strict compile time info item name check. Of course, you can also use
the actual values. The variable name consist of "ACTION_" prefix and all uppercased letters from actual standard action name.
Here is summary of all possible at the moment values.
<table>
<tr><th>String variable providing action name   <th>Actual action name  <th>Meaning                                 <th>Key bindings                    <th>%Icon sample
<tr><td>@link ACTION_FOCUS_NEXT     @endlink    <td>focus-next          <td>Focus next child within container       <td><b>Tab                      </b><td> @image html go-next.png
<tr><td>@link ACTION_FOCUS_PREVIOUS @endlink    <td>focus-previous      <td>Focus previous child within container   <td><b>Shift+Tab Shift+LeftTab  </b><td> @image html go-previous.png
<tr><td>@link ACTION_PAN_LEFT       @endlink    <td>pan-left            <td>Pan left                                <td><b>Ctrl+Alt+Left            </b><td>
<tr><td>@link ACTION_PAN_RIGHT      @endlink    <td>pan-right           <td>Pan right                               <td><b>Ctrl+Alt+Right           </b><td>
<tr><td>@link ACTION_PAN_UP         @endlink    <td>pan-up              <td>Pan up                                  <td><b>Ctrl+Up                  </b><td>
<tr><td>@link ACTION_PAN_DOWN       @endlink    <td>pan-down            <td>Pan down                                <td><b>Ctrl+Down                </b><td>
</table>

@section navigator_info Navigator and Fileman classes info item names
Navigator and Fileman classes has text-based info items management. The following information related to methods:
-  Navigator::show_info()
-  Navigator::hide_info()
-  Navigator::info_visible()
-  Navigator::visible_info_items()
-  Navigator::invisible_info_items()
-  Fileman::show_info()
-  Fileman::hide_info()
-  Fileman::info_visible()
-  Fileman::visible_info_items()
-  Fileman::invisible_info_items()

Library provide constant access string variables containing actual info item names. You can use these strings
to prevent typos while source text edit and strict compile time info item name check. Of course, you can also use
the actual values. The variable name consist of "NAVIGATOR_INFO_" prefix and all uppercased letters from actual info item name.
Here is summary of all possible at the moment values.
<table>
<tr><th>%Info key                               <th>Actual item name    <th>Meaning
<tr><td>@link NAVIGATOR_INFO_NAME   @endlink    <td>name                <td>Show/hide file/folder name
<tr><td>@link NAVIGATOR_INFO_BYTES  @endlink    <td>bytes               <td>Show/hide file/folder size in bytes
<tr><td>@link NAVIGATOR_INFO_DATE   @endlink    <td>date                <td>Show/hide file/folder modify time
<tr><td>@link NAVIGATOR_INFO_HIDDEN @endlink    <td>hidden              <td>Show/hide hidden files
<tr><td>@link NAVIGATOR_INFO_PLACES @endlink    <td>places              <td>Show/hide places
</table>

@section kbd_sect Everything Regarding the Keyboard
Keyboard management done with using pairs of values key_code:key_modifier named <b>key specification</b>.
The possible values of key codes are summarized in #Key_codes enumeration, key modifiers are summarized
in #Key_modifiers enumeration. The prefix for key code constants is @b KC_ (KC is for Key Code), the
prefix for key modifiers constants is @b KM_ (KM is for Key Modifier). The #Key_codes enumeration currently contains
only non-Unicode key codes (except @link KC_SPACE @endlink, which is a "Space" character code, U+0020) but it might change.
The Unicode key codes handled using its Unicode value.

The system processing of keyboard events perform by Display class object. Display translates key codes and key
modifiers into internal representation and emit series of signals to the current focused Window in the following order:
-   If key pressed:
    +   <em>Widget_impl::signal_accel() [implementation API, opaque]</em>.
        This signal is connected to the keyboard accelerator objects, named Accel. If any accelerator within
        current focus path handle emitted signal, the emission of the signal is interrupted.
    +   Widget::signal_input() [public API].
        If entered key value belong to Unicode range of codes, this signal emitted. If any of Widget within
        current focus path handle emitted signal, the emission of the signal is interrupted.
    +   Widget::signal_key_down() [public API].
        If signal was not handled earlier, this signal is emmited. It can handle both Unicode and
        non-Unicode key codes.
-   If key released:
    +   Widget::signal_key_up() [public API].

Typically, the ordinar user must not use above signals directly. The best way to handle keyboard events is to use
specially designed for this purpose classes Accel, Action and Toggle_action. For Unicode input, you can use
specially designed classes Edit and Entry.

Each pair of key_code:key_modifier values can be represented as text, using for serialization, displaying, saving
within a file etc. See also @ref km_sect and @ref kc_sect.

#### Related functions and methods:

- key_modifiers_to_string()
- key_modifiers_from_string()
- key_spec_to_string()
- key_spec_to_label()
- key_spec_from_string()
- key_code_is_modifier()
- Widget::signal_input()
- Widget::signal_key_down()
- Widget::signal_key_up()
- Accel::Accel(char32_t kc, int km)
- Accel::Accel(char32_t kc, int km, slot<bool()>)
- Accel::Accel(const ustring & spec)
- Accel::Accel(const ustring & spec, slot<bool()>)
- bool Accel::equals(char32_t kc, int km) const
- bool Accel::equals(const ustring & spec) const
- void Accel::assign(char32_t kc, int km)
- void Accel::assign(const ustring & spec)
- Accel::get_keys()
- Accel::spec()
- Accel::label()
- Master_action::Master_action(char32_t kc, int km)
- Master_action::Master_action(const ustring & accels)
- Master_action::Master_action(char32_t kc, int km, const ustring &)
- Master_action::Master_action(const ustring & accels, const ustring &)
- Master_action::Master_action(char32_t kc, int km, const ustring &, const ustring &)
- Master_action::Master_action(const ustring & accels, const ustring &, const ustring &)
- Master_action::Master_action(char32_t kc, int km, const ustring &, const ustring &, const ustring &)
- Master_action::Master_action(const ustring & accels, const ustring &, const ustring &, const ustring &)
- Master_action::add_accel()
- Master_action::add_accels()
- Master_action::remove_accel()
- Master_action::remove_accels()
- Action_base::Action_base(char32_t kc, int km)
- Action_base::Action_base(const ustring & accels)
- Action_base::Action_base(char32_t kc, int km, const ustring &)
- Action_base::Action_base(const ustring & accels, const ustring &)
- Action_base::Action_base(char32_t kc, int km, const ustring &, const ustring &)
- Action_base::Action_base(const ustring & accels, const ustring &, const ustring &)
- Action_base::Action_base(char32_t kc, int km, const ustring &, const ustring &, const ustring &)
- Action_base::Action_base(const ustring & accels, const ustring &, const ustring &, const ustring &)
- Action_base::add_accel()
- Action_base::add_accels()
- Action_base::remove_accel()
- Action_base::remove_accels()
- Action::Action(char32_t kc, int km, slot<void()>)
- Action::Action(const ustring & accels, slot<void()>)
- Action::Action(char32_t kc, int km, const ustring &, slot<void()>)
- Action::Action(const ustring & accels, const ustring &, slot<void()>)
- Action::Action(char32_t kc, int km, const ustring &, const ustring &, slot<void()>)
- Action::Action(const ustring & accels, const ustring &, const ustring &, slot<void()>)
- Action::Action(char32_t kc, int km, const ustring &, const ustring &, const ustring &, slot<void()>)
- Action::Action(const ustring & accels, const ustring &, const ustring &, const ustring &, slot<void()>)
- Toggle_action::Toggle_action(char32_t kc, int km, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, slot<void(bool)>)
- Toggle_action::Toggle_action(char32_t kc, int km, const ustring &, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, const ustring &, slot<void(bool)>)
- Toggle_action::Toggle_action(char32_t kc, int km, const ustring &, const ustring &, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, const ustring &, const ustring &, slot<void(bool)>)
- Toggle_action::Toggle_action(char32_t kc, int km, const ustring &, const ustring &, const ustring &, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, const ustring &, const ustring &, const ustring &, slot<void(bool)>)

@section km_sect Key modifier textual representation
Each key modifier from #Key_modifiers enumeration can be represented as text in two ways:
1.  As part of key specification. In this case the representation of the key modifiers looks like, for example,
    <b>"\<CONTROL\>" </b>, <b>"\<ALT\>\<SHIFT\>" </b> and so on.
2.  As a human readable text, for the tooltips and menu items. In this case the textual representation looks like,
    for example, <b>"Ctrl+BackSpace"</b>, <b>"Alt+Shift+N"</b>. There are no way to represent key modifier only as a
    human readable text.

At the basic level, the first conversion can be done with calling key_modifiers_to_string() function, the
back conversion can be done with calling key_modifiers_from_string() function. For addition, Accel::spec()
can be used, which calls key_modifiers_to_string().

The second case is handled by key_spec_to_label() function, which converts pair key_code:key_modifier to
human readable representation. For addition, Accel::label() can be used, which calls key_spec_to_label().

#### Related functions and methods using textual key modifiers representation:

- key_modifiers_to_string()
- key_modifiers_from_string()
- key_spec_to_string()
- key_spec_to_label()
- key_spec_from_string()
- Accel::Accel(const ustring & spec)
- Accel::Accel(const ustring & spec, slot<bool()>)
- void Accel::assign(const ustring & spec)
- Accel::spec()
- Accel::label()
- Master_action::Master_action(const ustring & accels)
- Master_action::Master_action(const ustring & accels, const ustring &)
- Master_action::Master_action(const ustring & accels, const ustring &, const ustring &)
- Master_action::Master_action(const ustring & accels, const ustring &, const ustring &, const ustring &)
- Master_action::add_accels()
- Master_action::remove_accels()
- Action_base::Action_base(const ustring & accels)
- Action_base::Action_base(const ustring & accels, const ustring &)
- Action_base::Action_base(const ustring & accels, const ustring &, const ustring &)
- Action_base::Action_base(const ustring & accels, const ustring &, const ustring &, const ustring &)
- Action_base::add_accels()
- Action_base::remove_accels()
- Action::Action(const ustring & accels, slot<void()>)
- Action::Action(const ustring & accels, const ustring &, slot<void()>)
- Action::Action(const ustring & accels, const ustring &, const ustring &, slot<void()>)
- Action::Action(const ustring & accels, const ustring &, const ustring &, const ustring &, slot<void()>)
- Toggle_action::Toggle_action(const ustring & accels, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, const ustring &, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, const ustring &, const ustring &, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, const ustring &, const ustring &, const ustring &, slot<void(bool)>)

Library provide constant access string variables containing actual key modifier names. You can use these strings
to prevent typos while source text edit and strict compile time modifier name check. Of course, you can also use
the actual values. The variable name consist of key modifier enumerator name plus "_NAME" suffix. Here is summary
of all possible at the moment values.
<table>
<tr><th><b>KM_ constant (#Key_modifiers enum)</b><th>String variable providing modifier name <th>Actual value
<tr><td>@link KM_NONE       @endlink<td>@link KM_NONE_NAME      @endlink<td>@b NONE
<tr><td>@link KM_SHIFT      @endlink<td>@link KM_SHIFT_NAME     @endlink<td>@b SHIFT
<tr><td>@link KM_CONTROL    @endlink<td>@link KM_CONTROL_NAME   @endlink<td>@b CONTROL
<tr><td>@link KM_ALT        @endlink<td>@link KM_ALT_NAME       @endlink<td>@b ALT
<tr><td>@link KM_META       @endlink<td>@link KM_META_NAME      @endlink<td>@b META
<tr><td>@link KM_WIN        @endlink<td>@link KM_WIN_NAME       @endlink<td>@b WIN
<tr><td>@link KM_SUPER      @endlink<td>@link KM_SUPER_NAME     @endlink<td>@b SUPER
<tr><td>@link KM_MENU       @endlink<td>@link KM_MENU_NAME      @endlink<td>@b MENU
<tr><td>@link KM_HYPER      @endlink<td>@link KM_HYPER_NAME     @endlink<td>@b HYPER
<tr><td>@link KM_GROUP      @endlink<td>@link KM_GROUP_NAME     @endlink<td>@b GROUP
<tr><td>@link KM_NUMLOCK    @endlink<td>@link KM_NUMLOCK_NAME   @endlink<td>@b NUMLOCK
<tr><td>@link KM_SCROLL     @endlink<td>@link KM_SCROLL_NAME    @endlink<td>@b SCROLL
<tr><td>@link KM_CAPS       @endlink<td>@link KM_CAPS_NAME      @endlink<td>@b CAPS
</table>

@section kc_sect Key code textual representation
Unlike key modifiers, the key code representation is same whithin specification or human readable text.
For Unicode key codes, the textual representation is a character itself (except "Space" character, which
textual representation is "Space"). For example, key specification of pair U'A':KM_CONTROL returned by
key_spec_to_string() is <b>"\<CONTROL\>A" </b> and human readable text returned by key_spec_to_label()
is <b>"Ctrl+A" </b>. If key code does not belong to Unicode range, the textual representation depend
on that, is that code listed in #Key_codes enumeration or not. If it is, the predefined string will be
produced, otherwise the string like "U+xxxx" will be produced.
#### Related functions and methods using textual key code representation

- key_modifiers_to_string()
- key_modifiers_from_string()
- key_spec_to_string()
- key_spec_to_label()
- key_spec_from_string()
- Accel::Accel(const ustring & spec)
- Accel::Accel(const ustring & spec, slot<bool()>)
- void Accel::assign(const ustring & spec)
- Accel::spec()
- Accel::label()
- Master_action::Master_action(const ustring & accels)
- Master_action::Master_action(const ustring & accels, const ustring &)
- Master_action::Master_action(const ustring & accels, const ustring &, const ustring &)
- Master_action::Master_action(const ustring & accels, const ustring &, const ustring &, const ustring &)
- Master_action::add_accels()
- Master_action::remove_accels()
- Action_base::Action_base(const ustring & accels)
- Action_base::Action_base(const ustring & accels, const ustring &)
- Action_base::Action_base(const ustring & accels, const ustring &, const ustring &)
- Action_base::Action_base(const ustring & accels, const ustring &, const ustring &, const ustring &)
- Action_base::add_accels()
- Action_base::remove_accels()
- Action::Action(const ustring & accels, slot<void()>)
- Action::Action(const ustring & accels, const ustring &, slot<void()>)
- Action::Action(const ustring & accels, const ustring &, const ustring &, slot<void()>)
- Action::Action(const ustring & accels, const ustring &, const ustring &, const ustring &, slot<void()>)
- Toggle_action::Toggle_action(const ustring & accels, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, const ustring &, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, const ustring &, const ustring &, slot<void(bool)>)
- Toggle_action::Toggle_action(const ustring & accels, const ustring &, const ustring &, const ustring &, slot<void(bool)>)

Library provide constant access string variables containing actual key code names. You can use these strings
to prevent typos while source text edit and strict compile time key code name check. Of course, you can also use
the actual values. The variable name consist of key code enumerator name plus "_NAME" suffix. Here is summary
of all possible at the moment values. All of them (except "Space", U+0020) are non-Unicode values.
<table>
<tr><th><b>KC_ constant (#Key_codes enum)</b>       <th>String variable providing key name              <th>Actual value                <th>Comment
<tr><td>@link KC_NONE                       @endlink<td>@link KC_NONE_NAME                      @endlink<td>@b None                     <td>None pressed or unknown
<tr><td>@link KC_BACKSPACE                  @endlink<td>@link KC_BACKSPACE_NAME                 @endlink<td>@b BackSpace                <td>BackSpace
<tr><td>@link KC_TAB                        @endlink<td>@link KC_TAB_NAME                       @endlink<td>@b Tab                      <td>Tab
<tr><td>@link KC_LINEFEED                   @endlink<td>@link KC_LINEFEED_NAME                  @endlink<td>@b LineFeed                 <td>LineFeed
<tr><td>@link KC_ENTER                      @endlink<td>@link KC_ENTER_NAME                     @endlink<td>@b Enter                    <td>Enter
<tr><td>@link KC_RETURN                     @endlink<td>@link KC_RETURN_NAME                    @endlink<td>@b Return                   <td>Synonymous Enter
<tr><td>@link KC_ESCAPE                     @endlink<td>@link KC_ESCAPE_NAME                    @endlink<td>@b Escape                   <td>Escape
<tr><td>@link KC_SPACE                      @endlink<td>@link KC_SPACE_NAME                     @endlink<td>@b Space                    <td>Space
<tr><td>@link KC_DELETE                     @endlink<td>@link KC_DELETE_NAME                    @endlink<td>@b Delete                   <td>Delete
<tr><td>@link KC_F1                         @endlink<td>@link KC_F1_NAME                        @endlink<td>@b F1                       <td>Functional, F1
<tr><td>@link KC_F2                         @endlink<td>@link KC_F2_NAME                        @endlink<td>@b F2                       <td>Functional, F2
<tr><td>@link KC_F3                         @endlink<td>@link KC_F3_NAME                        @endlink<td>@b F3                       <td>Functional, F3
<tr><td>@link KC_F4                         @endlink<td>@link KC_F4_NAME                        @endlink<td>@b F4                       <td>Functional, F4
<tr><td>@link KC_F5                         @endlink<td>@link KC_F5_NAME                        @endlink<td>@b F5                       <td>Functional, F5
<tr><td>@link KC_F6                         @endlink<td>@link KC_F6_NAME                        @endlink<td>@b F6                       <td>Functional, F6
<tr><td>@link KC_F7                         @endlink<td>@link KC_F7_NAME                        @endlink<td>@b F7                       <td>Functional, F7
<tr><td>@link KC_F8                         @endlink<td>@link KC_F8_NAME                        @endlink<td>@b F8                       <td>Functional, F8
<tr><td>@link KC_F9                         @endlink<td>@link KC_F9_NAME                        @endlink<td>@b F9                       <td>Functional, F9
<tr><td>@link KC_F10                        @endlink<td>@link KC_F10_NAME                       @endlink<td>@b F10                      <td>Functional, F10
<tr><td>@link KC_F11                        @endlink<td>@link KC_F11_NAME                       @endlink<td>@b F11                      <td>Functional, F11
<tr><td>@link KC_F12                        @endlink<td>@link KC_F12_NAME                       @endlink<td>@b F12                      <td>Functional, F12
<tr><td>@link KC_F13                        @endlink<td>@link KC_F13_NAME                       @endlink<td>@b F13                      <td>Functional, F13
<tr><td>@link KC_F14                        @endlink<td>@link KC_F14_NAME                       @endlink<td>@b F14                      <td>Functional, F14
<tr><td>@link KC_F15                        @endlink<td>@link KC_F15_NAME                       @endlink<td>@b F15                      <td>Functional, F15
<tr><td>@link KC_F16                        @endlink<td>@link KC_F16_NAME                       @endlink<td>@b F16                      <td>Functional, F16
<tr><td>@link KC_F17                        @endlink<td>@link KC_F17_NAME                       @endlink<td>@b F17                      <td>Functional, F17
<tr><td>@link KC_F18                        @endlink<td>@link KC_F18_NAME                       @endlink<td>@b F18                      <td>Functional, F18
<tr><td>@link KC_F19                        @endlink<td>@link KC_F19_NAME                       @endlink<td>@b F19                      <td>Functional, F19
<tr><td>@link KC_F20                        @endlink<td>@link KC_F20_NAME                       @endlink<td>@b F20                      <td>Functional, F20
<tr><td>@link KC_F21                        @endlink<td>@link KC_F21_NAME                       @endlink<td>@b F21                      <td>Functional, F21
<tr><td>@link KC_F22                        @endlink<td>@link KC_F22_NAME                       @endlink<td>@b F22                      <td>Functional, F22
<tr><td>@link KC_F23                        @endlink<td>@link KC_F23_NAME                       @endlink<td>@b F23                      <td>Functional, F23
<tr><td>@link KC_F24                        @endlink<td>@link KC_F24_NAME                       @endlink<td>@b F24                      <td>Functional, F24
<tr><td>@link KC_F25                        @endlink<td>@link KC_F25_NAME                       @endlink<td>@b F25                      <td>Functional, F25
<tr><td>@link KC_F26                        @endlink<td>@link KC_F26_NAME                       @endlink<td>@b F26                      <td>Functional, F26
<tr><td>@link KC_F27                        @endlink<td>@link KC_F27_NAME                       @endlink<td>@b F27                      <td>Functional, F27
<tr><td>@link KC_F28                        @endlink<td>@link KC_F28_NAME                       @endlink<td>@b F28                      <td>Functional, F28
<tr><td>@link KC_F29                        @endlink<td>@link KC_F29_NAME                       @endlink<td>@b F29                      <td>Functional, F29
<tr><td>@link KC_F30                        @endlink<td>@link KC_F30_NAME                       @endlink<td>@b F30                      <td>Functional, F30
<tr><td>@link KC_F31                        @endlink<td>@link KC_F31_NAME                       @endlink<td>@b F31                      <td>Functional, F31
<tr><td>@link KC_F32                        @endlink<td>@link KC_F32_NAME                       @endlink<td>@b F32                      <td>Functional, F32
<tr><td>@link KC_F33                        @endlink<td>@link KC_F33_NAME                       @endlink<td>@b F33                      <td>Functional, F33
<tr><td>@link KC_F34                        @endlink<td>@link KC_F34_NAME                       @endlink<td>@b F34                      <td>Functional, F34
<tr><td>@link KC_F35                        @endlink<td>@link KC_F35_NAME                       @endlink<td>@b F35                      <td>Functional, F35
<tr><td>@link KC_LEFT                       @endlink<td>@link KC_LEFT_NAME                      @endlink<td>@b Left                     <td>Left
<tr><td>@link KC_RIGHT                      @endlink<td>@link KC_RIGHT_NAME                     @endlink<td>@b Right                    <td>Right
<tr><td>@link KC_UP                         @endlink<td>@link KC_UP_NAME                        @endlink<td>@b Up                       <td>Up
<tr><td>@link KC_DOWN                       @endlink<td>@link KC_DOWN_NAME                      @endlink<td>@b Down                     <td>Down
<tr><td>@link KC_HOME                       @endlink<td>@link KC_HOME_NAME                      @endlink<td>@b Home                     <td>Home
<tr><td>@link KC_END                        @endlink<td>@link KC_END_NAME                       @endlink<td>@b End                      <td>End
<tr><td>@link KC_PAGE_UP                    @endlink<td>@link KC_PAGE_UP_NAME                   @endlink<td>@b PageUp                   <td>Page Up
<tr><td>@link KC_PAGE_DOWN                  @endlink<td>@link KC_PAGE_DOWN_NAME                 @endlink<td>@b PageDown                 <td>Page Down
<tr><td>@link KC_SCROLL_UP                  @endlink<td>@link KC_SCROLL_UP_NAME                 @endlink<td>@b ScrollUp                 <td>Scroll Up
<tr><td>@link KC_SCROLL_DOWN                @endlink<td>@link KC_SCROLL_DOWN_NAME               @endlink<td>@b ScrollDown               <td>Scroll Down
<tr><td>@link KC_INSERT                     @endlink<td>@link KC_INSERT_NAME                    @endlink<td>@b Insert                   <td>Insert
<tr><td>@link KC_BEGIN                      @endlink<td>@link KC_BEGIN_NAME                     @endlink<td>@b Begin                    <td>Begin
<tr><td>@link KC_CANCEL                     @endlink<td>@link KC_CANCEL_NAME                    @endlink<td>@b Cancel                   <td>Cancel
<tr><td>@link KC_BREAK                      @endlink<td>@link KC_BREAK_NAME                     @endlink<td>@b Break                    <td>Break
<tr><td>@link KC_CLEAR                      @endlink<td>@link KC_CLEAR_NAME                     @endlink<td>@b Clear                    <td>Clear
<tr><td>@link KC_PAUSE                      @endlink<td>@link KC_PAUSE_NAME                     @endlink<td>@b Pause                    <td>Pause
<tr><td>@link KC_PRINT                      @endlink<td>@link KC_PRINT_NAME                     @endlink<td>@b Print                    <td>Print
<tr><td>@link KC_SYSREQ                     @endlink<td>@link KC_SYSREQ_NAME                    @endlink<td>@b SysReq                   <td>Sys Request
<tr><td>@link KC_HELP                       @endlink<td>@link KC_HELP_NAME                      @endlink<td>@b Help                     <td>Help
<tr><td>@link KC_UNDO                       @endlink<td>@link KC_UNDO_NAME                      @endlink<td>@b Undo                     <td>Undo
<tr><td>@link KC_REDO                       @endlink<td>@link KC_REDO_NAME                      @endlink<td>@b Redo                     <td>Redo
<tr><td>@link KC_FIND                       @endlink<td>@link KC_FIND_NAME                      @endlink<td>@b Find                     <td>Find
<tr><td>@link KC_SELECT                     @endlink<td>@link KC_SELECT_NAME                    @endlink<td>@b Select                   <td>Select
<tr><td>@link KC_NO                         @endlink<td>@link KC_NO_NAME                        @endlink<td>@b No                       <td>No
<tr><td>@link KC_YES                        @endlink<td>@link KC_YES_NAME                       @endlink<td>@b Yes                      <td>Yes
<tr><td>@link KC_COPY                       @endlink<td>@link KC_COPY_NAME                      @endlink<td>@b Copy                     <td>Copy
<tr><td>@link KC_CUT                        @endlink<td>@link KC_CUT_NAME                       @endlink<td>@b Cut                      <td>Cut
<tr><td>@link KC_PASTE                      @endlink<td>@link KC_PASTE_NAME                     @endlink<td>@b Paste                    <td>Paste
<tr><td>@link KC_LDIR                       @endlink<td>@link KC_LDIR_NAME                      @endlink<td>@b DirectionL               <td>Left Direction
<tr><td>@link KC_RDIR                       @endlink<td>@link KC_RDIR_NAME                      @endlink<td>@b DirectionR               <td>Right Direction
<tr><td>@link KC_MULTI                      @endlink<td>@link KC_MULTI_NAME                     @endlink<td>@b Multi                    <td>Multi
<tr><td>@link KC_CODE_INPUT                 @endlink<td>@link KC_CODE_INPUT_NAME                @endlink<td>@b CodeInput                <td>Code Input
<tr><td>@link KC_SINGLE_CANDIDATE           @endlink<td>@link KC_SINGLE_CANDIDATE_NAME          @endlink<td>@b SingleCandidate          <td>Single Candidate
<tr><td>@link KC_MULTIPLE_CANDIDATE         @endlink<td>@link KC_MULTIPLE_CANDIDATE_NAME        @endlink<td>@b MultipleCandidate        <td>Multiple Candidate
<tr><td>@link KC_PREVIOUS_CANDIDATE         @endlink<td>@link KC_PREVIOUS_CANDIDATE_NAME        @endlink<td>@b PreviousCandidate        <td>Previous Candidate
<tr><td>@link KC_MODE_SWITCH                @endlink<td>@link KC_MODE_SWITCH_NAME               @endlink<td>@b ModeSwitch               <td>Mode Switch
<tr><td>@link KC_SEPARATOR                  @endlink<td>@link KC_SEPARATOR_NAME                 @endlink<td>@b Separator                <td>Often comma.
<tr><td>@link KC_DECIMAL                    @endlink<td>@link KC_DECIMAL_NAME                   @endlink<td>@b Decimal                  <td>Decimal
<tr><td>@link KC_ISO_LOCK                   @endlink<td>@link KC_ISO_LOCK_NAME                  @endlink<td>@b ISO_Lock                 <td>ISO Lock
<tr><td>@link KC_LEVEL2_LATCH               @endlink<td>@link KC_LEVEL2_LATCH_NAME              @endlink<td>@b Level2_Latch             <td>Level2 Latch
<tr><td>@link KC_LEVEL3_SHIFT               @endlink<td>@link KC_LEVEL3_SHIFT_NAME              @endlink<td>@b Level3_Shift             <td>Level3 Shift
<tr><td>@link KC_LEVEL3_LATCH               @endlink<td>@link KC_LEVEL3_LATCH_NAME              @endlink<td>@b Level3_Latch             <td>Level3 Latch
<tr><td>@link KC_LEVEL3_LOCK                @endlink<td>@link KC_LEVEL3_LOCK_NAME               @endlink<td>@b Level3_Lock              <td>Level3 Lock
<tr><td>@link KC_LEVEL5_SHIFT               @endlink<td>@link KC_LEVEL5_SHIFT_NAME              @endlink<td>@b Level5_Shift             <td>Level5 Shift
<tr><td>@link KC_LEVEL5_LATCH               @endlink<td>@link KC_LEVEL5_LATCH_NAME              @endlink<td>@b Level5_Latch             <td>Level5 Latch
<tr><td>@link KC_LEVEL5_LOCK                @endlink<td>@link KC_LEVEL5_LOCK_NAME               @endlink<td>@b Level5_Lock              <td>Level5 Lock
<tr><td>@link KC_GROUP_SHIFT                @endlink<td>@link KC_GROUP_SHIFT_NAME               @endlink<td>@b GroupShift               <td>Group Shift
<tr><td>@link KC_GROUP_LATCH                @endlink<td>@link KC_GROUP_LATCH_NAME               @endlink<td>@b GroupLatch               <td>Group Latch
<tr><td>@link KC_GROUP_LOCK                 @endlink<td>@link KC_GROUP_LOCK_NAME                @endlink<td>@b GroupLock                <td>Group Lock
<tr><td>@link KC_NEXT_GROUP                 @endlink<td>@link KC_NEXT_GROUP_NAME                @endlink<td>@b NextGroup                <td>Next Group
<tr><td>@link KC_NEXT_GROUP_LOCK            @endlink<td>@link KC_NEXT_GROUP_LOCK_NAME           @endlink<td>@b NextGroupLock            <td>Next Group Lock
<tr><td>@link KC_PREV_GROUP                 @endlink<td>@link KC_PREV_GROUP_NAME                @endlink<td>@b PrevGroup                <td>Prev Group
<tr><td>@link KC_PREV_GROUP_LOCK            @endlink<td>@link KC_PREV_GROUP_LOCK_NAME           @endlink<td>@b PrevGroupLock            <td>Prev Group Lock
<tr><td>@link KC_FIRST_GROUP                @endlink<td>@link KC_FIRST_GROUP_NAME               @endlink<td>@b FirstGroup               <td>First Group
<tr><td>@link KC_FIRST_GROUP_LOCK           @endlink<td>@link KC_FIRST_GROUP_LOCK_NAME          @endlink<td>@b FirstGroupLock           <td>First Group Lock
<tr><td>@link KC_LAST_GROUP                 @endlink<td>@link KC_LAST_GROUP_NAME                @endlink<td>@b LastGroup                <td>Last Group
<tr><td>@link KC_LAST_GROUP_LOCK            @endlink<td>@link KC_LAST_GROUP_LOCK_NAME           @endlink<td>@b LastGroupLock            <td>Last Group Lock
<tr><td>@link KC_LEFT_TAB                   @endlink<td>@link KC_LEFT_TAB_NAME                  @endlink<td>@b LeftTab                  <td>Left Tab
<tr><td>@link KC_MOVE_LINE_UP               @endlink<td>@link KC_MOVE_LINE_UP_NAME              @endlink<td>@b MoveLineUp               <td>Move Line Up
<tr><td>@link KC_MOVE_LINE_DOWN             @endlink<td>@link KC_MOVE_LINE_DOWN_NAME            @endlink<td>@b MoveLineDown             <td>Move Line Down
<tr><td>@link KC_PARTIAL_SPACE_LEFT         @endlink<td>@link KC_PARTIAL_SPACE_LEFT_NAME        @endlink<td>@b PartialSpaceLeft         <td>Partial Space Left
<tr><td>@link KC_PARTIAL_SPACE_RIGHT        @endlink<td>@link KC_PARTIAL_SPACE_RIGHT_NAME       @endlink<td>@b PartialSpaceRight        <td>Partial Space Right
<tr><td>@link KC_PARTIAL_LINE_UP            @endlink<td>@link KC_PARTIAL_LINE_UP_NAME           @endlink<td>@b PartialLineUp            <td>Partial Line Up
<tr><td>@link KC_PARTIAL_LINE_DOWN          @endlink<td>@link KC_PARTIAL_LINE_DOWN_NAME         @endlink<td>@b PartialLineDown          <td>Partial Line Down
<tr><td>@link KC_SET_MARGIN_LEFT            @endlink<td>@link KC_SET_MARGIN_LEFT_NAME           @endlink<td>@b SetMarginLeft            <td>Set Margin Left
<tr><td>@link KC_SET_MARGIN_RIGHT           @endlink<td>@link KC_SET_MARGIN_RIGHT_NAME          @endlink<td>@b SetMarginRight           <td>Set Margin Right
<tr><td>@link KC_RELEASE_MARGIN_LEFT        @endlink<td>@link KC_RELEASE_MARGIN_LEFT_NAME       @endlink<td>@b ReleaseMarginLeft        <td>Release Margin Left
<tr><td>@link KC_RELEASE_MARGIN_RIGHT       @endlink<td>@link KC_RELEASE_MARGIN_RIGHT_NAME      @endlink<td>@b ReleaseMarginRight       <td>Release Margin Right
<tr><td>@link KC_RELEASE_BOTH_MARGINS       @endlink<td>@link KC_RELEASE_BOTH_MARGINS_NAME      @endlink<td>@b ReleaseBothMargins       <td>Release Both Margins
<tr><td>@link KC_FAST_CURSOR_LEFT           @endlink<td>@link KC_FAST_CURSOR_LEFT_NAME          @endlink<td>@b FastCursorLeft           <td>Fast %Cursor Left
<tr><td>@link KC_FAST_CURSOR_RIGHT          @endlink<td>@link KC_FAST_CURSOR_RIGHT_NAME         @endlink<td>@b FastCursorRight          <td>Fast %Cursor Right
<tr><td>@link KC_FAST_CURSOR_UP             @endlink<td>@link KC_FAST_CURSOR_UP_NAME            @endlink<td>@b FastCursorUp             <td>Fast %Cursor Up
<tr><td>@link KC_FAST_CURSOR_DOWN           @endlink<td>@link KC_FAST_CURSOR_DOWN_NAME          @endlink<td>@b FastCursorDown           <td>Fast %Cursor Down
<tr><td>@link KC_CONTINUOUS_UNDERLINE       @endlink<td>@link KC_CONTINUOUS_UNDERLINE_NAME      @endlink<td>@b ContinuousUnderline      <td>Continuous Underline
<tr><td>@link KC_DISCONTINUOUS_UNDERLINE    @endlink<td>@link KC_DISCONTINUOUS_UNDERLINE_NAME   @endlink<td>@b DiscontinuousUnderline   <td>Discontinuous Underline
<tr><td>@link KC_EMPHASIZE                  @endlink<td>@link KC_EMPHASIZE_NAME                 @endlink<td>@b Emphasize                <td>Emphasize
<tr><td>@link KC_CENTER_OBJECT              @endlink<td>@link KC_CENTER_OBJECT_NAME             @endlink<td>@b CenterObject             <td>Center Object
<tr><td>@link KC_SEND                       @endlink<td>@link KC_SEND_NAME                      @endlink<td>@b Send                     <td>Send mail, file, object
<tr><td>@link KC_REPLY                      @endlink<td>@link KC_REPLY_NAME                     @endlink<td>@b Reply                    <td>Reply e.g., mail
<tr><td>@link KC_ZOOM_IN                    @endlink<td>@link KC_ZOOM_IN_NAME                   @endlink<td>@b ZoomIn                   <td>Zoom in view, map, etc.
<tr><td>@link KC_ZOOM_OUT                   @endlink<td>@link KC_ZOOM_OUT_NAME                  @endlink<td>@b ZoomOut                  <td>Zoom out view, map, etc.
<tr><td>@link KC_FORWARD                    @endlink<td>@link KC_FORWARD_NAME                   @endlink<td>@b Forward                  <td>Forward
<tr><td>@link KC_BACK                       @endlink<td>@link KC_BACK_NAME                      @endlink<td>@b Back                     <td>Back
<tr><td>@link KC_STOP                       @endlink<td>@link KC_STOP_NAME                      @endlink<td>@b Stop                     <td>Stop
<tr><td>@link KC_REFRESH                    @endlink<td>@link KC_REFRESH_NAME                   @endlink<td>@b Refresh                  <td>Refresh
<tr><td>@link KC_HOME_PAGE                  @endlink<td>@link KC_HOME_PAGE_NAME                 @endlink<td>@b HomePage                 <td>Home Page
<tr><td>@link KC_FAVORITES                  @endlink<td>@link KC_FAVORITES_NAME                 @endlink<td>@b Favorites                <td>Favorites
<tr><td>@link KC_SEARCH                     @endlink<td>@link KC_SEARCH_NAME                    @endlink<td>@b Search                   <td>Search
<tr><td>@link KC_STANDBY                    @endlink<td>@link KC_STANDBY_NAME                   @endlink<td>@b Standby                  <td>Standby
<tr><td>@link KC_OPEN_URL                   @endlink<td>@link KC_OPEN_URL_NAME                  @endlink<td>@b OpenURL                  <td>Open URL
<tr><td>@link KC_LAUNCH_MAIL                @endlink<td>@link KC_LAUNCH_MAIL_NAME               @endlink<td>@b LaunchMail               <td>Launch Mail
<tr><td>@link KC_LAUNCH_MEDIA               @endlink<td>@link KC_LAUNCH_MEDIA_NAME              @endlink<td>@b LaunchMedia              <td>Launch Media Player
<tr><td>@link KC_LAUNCH0                    @endlink<td>@link KC_LAUNCH0_NAME                   @endlink<td>@b Launch0                  <td>Launch0
<tr><td>@link KC_LAUNCH1                    @endlink<td>@link KC_LAUNCH1_NAME                   @endlink<td>@b Launch1                  <td>Launch1
<tr><td>@link KC_LAUNCH2                    @endlink<td>@link KC_LAUNCH2_NAME                   @endlink<td>@b Launch2                  <td>Launch2
<tr><td>@link KC_LAUNCH3                    @endlink<td>@link KC_LAUNCH3_NAME                   @endlink<td>@b Launch3                  <td>Launch3
<tr><td>@link KC_LAUNCH4                    @endlink<td>@link KC_LAUNCH4_NAME                   @endlink<td>@b Launch4                  <td>Launch4
<tr><td>@link KC_LAUNCH5                    @endlink<td>@link KC_LAUNCH5_NAME                   @endlink<td>@b Launch5                  <td>Launch5
<tr><td>@link KC_LAUNCH6                    @endlink<td>@link KC_LAUNCH6_NAME                   @endlink<td>@b Launch6                  <td>Launch6
<tr><td>@link KC_LAUNCH7                    @endlink<td>@link KC_LAUNCH7_NAME                   @endlink<td>@b Launch7                  <td>Launch7
<tr><td>@link KC_LAUNCH8                    @endlink<td>@link KC_LAUNCH8_NAME                   @endlink<td>@b Launch8                  <td>Launch8
<tr><td>@link KC_LAUNCH9                    @endlink<td>@link KC_LAUNCH9_NAME                   @endlink<td>@b Launch9                  <td>Launch9
<tr><td>@link KC_LAUNCHA                    @endlink<td>@link KC_LAUNCHA_NAME                   @endlink<td>@b LaunchA                  <td>LaunchA
<tr><td>@link KC_LAUNCHB                    @endlink<td>@link KC_LAUNCHB_NAME                   @endlink<td>@b LaunchB                  <td>LaunchB
<tr><td>@link KC_LAUNCHC                    @endlink<td>@link KC_LAUNCHC_NAME                   @endlink<td>@b LaunchC                  <td>LaunchC
<tr><td>@link KC_LAUNCHD                    @endlink<td>@link KC_LAUNCHD_NAME                   @endlink<td>@b LaunchD                  <td>LaunchD
<tr><td>@link KC_LAUNCHE                    @endlink<td>@link KC_LAUNCHE_NAME                   @endlink<td>@b LaunchE                  <td>LaunchE
<tr><td>@link KC_LAUNCHF                    @endlink<td>@link KC_LAUNCHF_NAME                   @endlink<td>@b LaunchF                  <td>LaunchF
<tr><td>@link KC_EXECUTE                    @endlink<td>@link KC_EXECUTE_NAME                   @endlink<td>@b Execute                  <td>Execute
<tr><td>@link KC_TERMINAL                   @endlink<td>@link KC_TERMINAL_NAME                  @endlink<td>@b Terminal                 <td>Terminal
<tr><td>@link KC_POWER_OFF                  @endlink<td>@link KC_POWER_OFF_NAME                 @endlink<td>@b PowerOff                 <td>Power Off
<tr><td>@link KC_SLEEP                      @endlink<td>@link KC_SLEEP_NAME                     @endlink<td>@b Sleep                    <td>Sleep
<tr><td>@link KC_WAKE_UP                    @endlink<td>@link KC_WAKE_UP_NAME                   @endlink<td>@b WakeUp                   <td>Wake Up
<tr><td>@link KC_TERMINATE_SERVER           @endlink<td>@link KC_TERMINATE_SERVER_NAME          @endlink<td>@b TerminateServer          <td>Terminate Server
<tr><td>@link KC_CLOSE                      @endlink<td>@link KC_CLOSE_NAME                     @endlink<td>@b Close                    <td>Close
<tr><td>@link KC_PHONE                      @endlink<td>@link KC_PHONE_NAME                     @endlink<td>@b Phone                    <td>Launch phone; dial number
<tr><td>@link KC_RELOAD                     @endlink<td>@link KC_RELOAD_NAME                    @endlink<td>@b Reload                   <td>Reload web page, file, etc.
<tr><td>@link KC_SAVE                       @endlink<td>@link KC_SAVE_NAME                      @endlink<td>@b Save                     <td>Save (file, document, state
<tr><td>@link KC_VIDEO                      @endlink<td>@link KC_VIDEO_NAME                     @endlink<td>@b Video                    <td>Launch video player
<tr><td>@link KC_MUSIC                      @endlink<td>@link KC_MUSIC_NAME                     @endlink<td>@b Music                    <td>Launch music applicaton
<tr><td>@link KC_BATTERY                    @endlink<td>@link KC_BATTERY_NAME                   @endlink<td>@b Battery                  <td>%Display battery information
<tr><td>@link KC_BLUETOOTH                  @endlink<td>@link KC_BLUETOOTH_NAME                 @endlink<td>@b Bluetooth                <td>Enable/disable Bluetooth
<tr><td>@link KC_WLAN                       @endlink<td>@link KC_WLAN_NAME                      @endlink<td>@b WLAN                     <td>Enable/disable WLAN
<tr><td>@link KC_VOLUME_UP                  @endlink<td>@link KC_VOLUME_UP_NAME                 @endlink<td>@b VolumeUp                 <td>Volume Up
<tr><td>@link KC_VOLUME_DOWN                @endlink<td>@link KC_VOLUME_DOWN_NAME               @endlink<td>@b VolumeDown               <td>Volume Down
<tr><td>@link KC_VOLUME_MUTE                @endlink<td>@link KC_VOLUME_MUTE_NAME               @endlink<td>@b VolumeMute               <td>Volume Mute
<tr><td>@link KC_BASS_BOOST                 @endlink<td>@link KC_BASS_BOOST_NAME                @endlink<td>@b BassBoost                <td>Bass Boost
<tr><td>@link KC_BASS_UP                    @endlink<td>@link KC_BASS_UP_NAME                   @endlink<td>@b BassUp                   <td>Bass Up
<tr><td>@link KC_BASS_DOWN                  @endlink<td>@link KC_BASS_DOWN_NAME                 @endlink<td>@b BassDown                 <td>Bass Down
<tr><td>@link KC_TREBLE_UP                  @endlink<td>@link KC_TREBLE_UP_NAME                 @endlink<td>@b TrebleUp                 <td>Treble Up
<tr><td>@link KC_TREBLE_DOWN                @endlink<td>@link KC_TREBLE_DOWN_NAME               @endlink<td>@b TrebleDown               <td>Treble Down
<tr><td>@link KC_MEDIA_PLAY                 @endlink<td>@link KC_MEDIA_PLAY_NAME                @endlink<td>@b MediaPlay                <td>Media Play
<tr><td>@link KC_MEDIA_PAUSE                @endlink<td>@link KC_MEDIA_PAUSE_NAME               @endlink<td>@b MediaPause               <td>Media Pause
<tr><td>@link KC_MEDIA_STOP                 @endlink<td>@link KC_MEDIA_STOP_NAME                @endlink<td>@b MediaStop                <td>Media Stop
<tr><td>@link KC_MEDIA_PREVIOUS             @endlink<td>@link KC_MEDIA_PREVIOUS_NAME            @endlink<td>@b MediaPrevious            <td>Media Previous
<tr><td>@link KC_MEDIA_NEXT                 @endlink<td>@link KC_MEDIA_NEXT_NAME                @endlink<td>@b MediaNext                <td>Media Next
<tr><td>@link KC_MEDIA_RECORD               @endlink<td>@link KC_MEDIA_RECORD_NAME              @endlink<td>@b MediaRecord              <td>Media Record
<tr><td>@link KC_KANJI                      @endlink<td>@link KC_KANJI_NAME                     @endlink<td>@b Kanji                    <td>Kanji, Kanji convert
<tr><td>@link KC_MUHENKAN                   @endlink<td>@link KC_MUHENKAN_NAME                  @endlink<td>@b Muhenkan                 <td>Cancel Conversion
<tr><td>@link KC_HENKAN                     @endlink<td>@link KC_HENKAN_NAME                    @endlink<td>@b Henkan                   <td>Alias for Henkan_Mode
<tr><td>@link KC_ROMAJI                     @endlink<td>@link KC_ROMAJI_NAME                    @endlink<td>@b Romaji                   <td>to Romaji
<tr><td>@link KC_HIRAGANA                   @endlink<td>@link KC_HIRAGANA_NAME                  @endlink<td>@b Hiragana                 <td>to Hiragana
<tr><td>@link KC_KATAKANA                   @endlink<td>@link KC_KATAKANA_NAME                  @endlink<td>@b Katakana                 <td>to Katakana
<tr><td>@link KC_HIRAGANA_KATAKANA          @endlink<td>@link KC_HIRAGANA_KATAKANA_NAME         @endlink<td>@b HiraganaKatakana         <td>Hiragana/Katakana toggle
<tr><td>@link KC_ZENKAKU                    @endlink<td>@link KC_ZENKAKU_NAME                   @endlink<td>@b Zenkaku                  <td>to Zenkaku
<tr><td>@link KC_HANKAKU                    @endlink<td>@link KC_HANKAKU_NAME                   @endlink<td>@b Hankaku                  <td>to Hankaku
<tr><td>@link KC_ZENKAKU_HANKAKU            @endlink<td>@link KC_ZENKAKU_HANKAKU_NAME           @endlink<td>@b ZenkakuHankaku           <td>Zenkaku/Hankaku toggle
<tr><td>@link KC_TOUROKU                    @endlink<td>@link KC_TOUROKU_NAME                   @endlink<td>@b Touroku                  <td>Add to Dictionary
<tr><td>@link KC_MASSYO                     @endlink<td>@link KC_MASSYO_NAME                    @endlink<td>@b Massyo                   <td>Delete from Dictionary
<tr><td>@link KC_KANA_LOCK                  @endlink<td>@link KC_KANA_LOCK_NAME                 @endlink<td>@b KanaLock                 <td>Kana Lock
<tr><td>@link KC_KANA_SHIFT                 @endlink<td>@link KC_KANA_SHIFT_NAME                @endlink<td>@b KanaShift                <td>Kana Shift
<tr><td>@link KC_EISU_SHIFT                 @endlink<td>@link KC_EISU_SHIFT_NAME                @endlink<td>@b EisuShift                <td>Alphanumeric Shift
<tr><td>@link KC_EISU_TOGGLE                @endlink<td>@link KC_EISU_TOGGLE_NAME               @endlink<td>@b EisuToggle               <td>Alphanumeric toggle
<tr><td>@link KC_HANGUL                     @endlink<td>@link KC_HANGUL_NAME                    @endlink<td>@b Hangul                   <td>Hangul start/stop(toggle)
<tr><td>@link KC_HANGUL_START               @endlink<td>@link KC_HANGUL_START_NAME              @endlink<td>@b HangulStart              <td>Hangul start
<tr><td>@link KC_HANGUL_END                 @endlink<td>@link KC_HANGUL_END_NAME                @endlink<td>@b HangulEnd                <td>Hangul end, English start
<tr><td>@link KC_HANGUL_HANJA               @endlink<td>@link KC_HANGUL_HANJA_NAME              @endlink<td>@b HangulHanja              <td>Start Hangul->Hanja Conversion
<tr><td>@link KC_HANGUL_JAMO                @endlink<td>@link KC_HANGUL_JAMO_NAME               @endlink<td>@b HangulJamo               <td>Hangul Jamo mode
<tr><td>@link KC_HANGUL_ROMAJA              @endlink<td>@link KC_HANGUL_ROMAJA_NAME             @endlink<td>@b HangulRomaja             <td>Hangul Romaja mode
<tr><td>@link KC_HANGUL_JEONJA              @endlink<td>@link KC_HANGUL_JEONJA_NAME             @endlink<td>@b HangulJeonja             <td>Jeonja mode
<tr><td>@link KC_HANGUL_BANJA               @endlink<td>@link KC_HANGUL_BANJA_NAME              @endlink<td>@b HangulBanja              <td>Banja mode
<tr><td>@link KC_HANGUL_PREHANJA            @endlink<td>@link KC_HANGUL_PREHANJA_NAME           @endlink<td>@b HangulPrehanja           <td>Pre Hanja conversion
<tr><td>@link KC_HANGUL_POSTHANJA           @endlink<td>@link KC_HANGUL_POSTHANJA_NAME          @endlink<td>@b HangulPosthanja          <td>Post Hanja conversion
<tr><td>@link KC_HANGUL_SPECIAL             @endlink<td>@link KC_HANGUL_SPECIAL_NAME            @endlink<td>@b HangulSpecial            <td>Special symbols
<tr><td>@link KC_BRAILLE_DOT_1              @endlink<td>@link KC_BRAILLE_DOT_1_NAME             @endlink<td>@b BrailleDot_1             <td>Braille Dot 1
<tr><td>@link KC_BRAILLE_DOT_2              @endlink<td>@link KC_BRAILLE_DOT_2_NAME             @endlink<td>@b BrailleDot_2             <td>Braille Dot 2
<tr><td>@link KC_BRAILLE_DOT_3              @endlink<td>@link KC_BRAILLE_DOT_3_NAME             @endlink<td>@b BrailleDot_3             <td>Braille Dot 3
<tr><td>@link KC_BRAILLE_DOT_4              @endlink<td>@link KC_BRAILLE_DOT_4_NAME             @endlink<td>@b BrailleDot_4             <td>Braille Dot 4
<tr><td>@link KC_BRAILLE_DOT_5              @endlink<td>@link KC_BRAILLE_DOT_5_NAME             @endlink<td>@b BrailleDot_5             <td>Braille Dot 5
<tr><td>@link KC_BRAILLE_DOT_6              @endlink<td>@link KC_BRAILLE_DOT_6_NAME             @endlink<td>@b BrailleDot_6             <td>Braille Dot 6
<tr><td>@link KC_BRAILLE_DOT_7              @endlink<td>@link KC_BRAILLE_DOT_7_NAME             @endlink<td>@b BrailleDot_7             <td>Braille Dot 7
<tr><td>@link KC_BRAILLE_DOT_8              @endlink<td>@link KC_BRAILLE_DOT_8_NAME             @endlink<td>@b BrailleDot_8             <td>Braille Dot 8
<tr><td>@link KC_BRAILLE_DOT_9              @endlink<td>@link KC_BRAILLE_DOT_9_NAME             @endlink<td>@b BrailleDot_9             <td>Braille Dot 9
<tr><td>@link KC_BRAILLE_DOT_10             @endlink<td>@link KC_BRAILLE_DOT_10_NAME            @endlink<td>@b BrailleDot_10            <td>Braille Dot 10
<tr><td>@link KC_LSHIFT                     @endlink<td>@link KC_LSHIFT_NAME                    @endlink<td>@b ShiftL                   <td>Left Shift
<tr><td>@link KC_RSHIFT                     @endlink<td>@link KC_RSHIFT_NAME                    @endlink<td>@b ShiftR                   <td>Right Shift
<tr><td>@link KC_SHIFT                      @endlink<td>@link KC_SHIFT_NAME                     @endlink<td>@b Shift                    <td>Shift
<tr><td>@link KC_LCONTROL                   @endlink<td>@link KC_LCONTROL_NAME                  @endlink<td>@b ControlL                 <td>Left Control
<tr><td>@link KC_RCONTROL                   @endlink<td>@link KC_RCONTROL_NAME                  @endlink<td>@b ControlR                 <td>Right Control
<tr><td>@link KC_CONTROL                    @endlink<td>@link KC_CONTROL_NAME                   @endlink<td>@b Control                  <td>Control
<tr><td>@link KC_LALT                       @endlink<td>@link KC_LALT_NAME                      @endlink<td>@b AltL                     <td>Left Alt
<tr><td>@link KC_RALT                       @endlink<td>@link KC_RALT_NAME                      @endlink<td>@b AltR                     <td>Right Alt
<tr><td>@link KC_ALT                        @endlink<td>@link KC_ALT_NAME                       @endlink<td>@b Alt                      <td>Alt
<tr><td>@link KC_LMETA                      @endlink<td>@link KC_LMETA_NAME                     @endlink<td>@b MetaL                    <td>Left Meta
<tr><td>@link KC_RMETA                      @endlink<td>@link KC_RMETA_NAME                     @endlink<td>@b MetaR                    <td>Right Meta
<tr><td>@link KC_META                       @endlink<td>@link KC_META_NAME                      @endlink<td>@b Meta                     <td>Meta
<tr><td>@link KC_LWIN                       @endlink<td>@link KC_LWIN_NAME                      @endlink<td>@b WinL                     <td>Left Windows
<tr><td>@link KC_RWIN                       @endlink<td>@link KC_RWIN_NAME                      @endlink<td>@b WinR                     <td>Right Windows
<tr><td>@link KC_WIN                        @endlink<td>@link KC_WIN_NAME                       @endlink<td>@b Win                      <td>Windows
<tr><td>@link KC_LSUPER                     @endlink<td>@link KC_LSUPER_NAME                    @endlink<td>@b SuperL                   <td>Left Super
<tr><td>@link KC_RSUPER                     @endlink<td>@link KC_RSUPER_NAME                    @endlink<td>@b SuperR                   <td>Right Super
<tr><td>@link KC_SUPER                      @endlink<td>@link KC_SUPER_NAME                     @endlink<td>@b Super                    <td>Super
<tr><td>@link KC_LMENU                      @endlink<td>@link KC_LMENU_NAME                     @endlink<td>@b MenuL                    <td>Left %Menu
<tr><td>@link KC_RMENU                      @endlink<td>@link KC_RMENU_NAME                     @endlink<td>@b MenuR                    <td>Right %Menu
<tr><td>@link KC_MENU                       @endlink<td>@link KC_MENU_NAME                      @endlink<td>@b %Menu                     <td>%Menu
<tr><td>@link KC_LHYPER                     @endlink<td>@link KC_LHYPER_NAME                    @endlink<td>@b HyperL                   <td>Left Hyper
<tr><td>@link KC_RHYPER                     @endlink<td>@link KC_RHYPER_NAME                    @endlink<td>@b HyperR                   <td>Right Hyper
<tr><td>@link KC_HYPER                      @endlink<td>@link KC_HYPER_NAME                     @endlink<td>@b Hyper                    <td>Hyper
<tr><td>@link KC_GROUP                      @endlink<td>@link KC_GROUP_NAME                     @endlink<td>@b Group                    <td>Group
<tr><td>@link KC_NUM_LOCK                   @endlink<td>@link KC_NUM_LOCK_NAME                  @endlink<td>@b NumLock                  <td>Num Lock
<tr><td>@link KC_SCROLL_LOCK                @endlink<td>@link KC_SCROLL_LOCK_NAME               @endlink<td>@b ScrollLock               <td>Scroll Lock
<tr><td>@link KC_CAPS_LOCK                  @endlink<td>@link KC_CAPS_LOCK_NAME                 @endlink<td>@b CapsLock                 <td>Caps Lock
<tr><td>@link KC_SHIFT_LOCK                 @endlink<td>@link KC_SHIFT_LOCK_NAME                @endlink<td>@b ShiftLock                <td>Shift Lock
</table>

**/
}
