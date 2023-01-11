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

/// @page actions_page Standard Actions
/// Standard action names used by library.
/// Actions provide key bindings, standard icon names, labels and tooltips.
///
/// Some actions provide more than one keyboard accelerator, in such cases
/// key bindings shown here separated by spaces.
///
/// To obtain a standard action, use Theme::find_action() method.
/// <table>
/// <tr><th>%Action name    <th>Meaning                                 <th>Key bindings                    <th>%Icon sample
/// <tr><td>focus-next      <td>Focus next child within container       <td><b>Tab                      </b><td> @image html go-next.png
/// <tr><td>focus-previous  <td>Focus previous child within container   <td><b>Shift+Tab Shift+LeftTab  </b><td> @image html go-previous.png
/// <tr><td>pan-left        <td>Pan left                                <td><b>Ctrl+Alt+Left            </b><td>
/// <tr><td>pan-right       <td>Pan right                               <td><b>Ctrl+Alt+Right           </b><td>
/// <tr><td>pan-up          <td>Pan up                                  <td><b>Ctrl+Up                  </b><td>
/// <tr><td>pan-down        <td>Pan down                                <td><b>Ctrl+Down                </b><td>
/// </table>

}
