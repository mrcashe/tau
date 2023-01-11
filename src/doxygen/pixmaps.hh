// ----------------------------------------------------------------------------
// Copyright © 2014-2022 Konstantin Shmelkov <mrcashe@gmail.com>.
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

/// @page pixmap_page Standard Pixmaps
/// The standard pixmap names used by libtau.
/// Icons used by the library.
/// The icon can be obtained by calling one of
/// Theme::find_icon(), Theme::get_icon() or Theme::find_pixmap() methods.
/// Here is in the table icon (image) names used by the library.
/// Most of icon names are compliant to XDG icon name standards
/// (https://standards.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html).
/// The presence of that icons is mandatory for normal library work.
///
/// <table>
/// <tr><th>%Icon name              <th>Classes using pixmap            <th>%Image sample
/// <tr><td>configure               <td>Fileman                         <td> @image html configure.png
/// <tr><td>dialog-cancel           <td>Fileman, Colorsel, Fontsel      <td> @image html dialog-cancel.png
/// <tr><td>dialog-ok               <td>Fontsel, Colorsel, Fileman      <td> @image html dialog-ok.png
/// <tr><td>drive-optical           <td>Fileman                         <td> @image html drive-optical.png
/// <tr><td>drive-harddisk          <td>Fileman                         <td> @image html drive-harddisk.png
/// <tr><td>drive-removable-media   <td>Fileman                         <td> @image html drive-removable-media.png
/// <tr><td>folder                  <td>Fileman, Navigator              <td> @image html folder.png
/// <tr><td>folder-new              <td>Fileman                         <td> @image html folder-new.png
/// <tr><td>folder-remote           <td>Fileman                         <td> @image html folder-remote.png
/// <tr><td>go-home                 <td>Fileman                         <td> @image html go-home.png
/// <tr><td>go-next                 <td>Fileman, Theme                  <td> @image html go-next.png
/// <tr><td>go-previous             <td>Fileman, Theme                  <td> @image html go-previous.png
/// <tr><td>go-up                   <td>Fileman                         <td> @image html go-up.png
/// <tr><td>picto-dec               <td>Cycle, Counter, Theme           <td> @image html picto-dec.png
/// <tr><td>picto-inc               <td>Cycle, Counter, Theme           <td> @image html picto-inc.png
/// <tr><td>picto-close             <td>Fileman, Theme                  <td> @image html picto-close.png
/// <tr><td>picto-down              <td>Roller, Slider, List, Theme     <td> @image html picto-down.png
/// <tr><td>picto-left              <td>Roller, Slider, Theme           <td> @image html picto-left.png
/// <tr><td>picto-right             <td>Roller, Slider, Menu_item, Theme<td> @image html picto-right.png
/// <tr><td>picto-up                <td>Roller, Slider, List, Theme     <td> @image html picto-up.png
/// <tr><td>unknown                 <td>Navigator                       <td> @image html unknown.png
/// <tr><td>view-refresh            <td>Fileman                         <td> @image html view-refresh.png
/// </table>

}
