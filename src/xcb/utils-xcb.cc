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

#include "types-xcb.hh"
#include <tau/color.hh>
#include <tau/geometry.hh>
#include <tau/string.hh>
#include <map>

namespace tau {

xcb_render_color_t x11_render_color(const Color & color) {
    xcb_render_color_t rc;
    rc.red = 65535.0*color.red();
    rc.green = 65535.0*color.green();
    rc.blue = 65535.0*color.blue();
    rc.alpha = 65535.0*color.alpha();
    return rc;
}

ustring x11_error_msg(int code) {
    static const std::map<int, ustring> errors = {
        { XCB_CONN_ERROR, "connection error" },
        { XCB_CONN_CLOSED_EXT_NOTSUPPORTED, "extension not supported" },
        { XCB_CONN_CLOSED_MEM_INSUFFICIENT, "memory not available" },
        { XCB_CONN_CLOSED_REQ_LEN_EXCEED, "exceeding request length that server accepts" },
        { XCB_CONN_CLOSED_PARSE_ERR, "error parsing display string" },
        { XCB_CONN_CLOSED_INVALID_SCREEN, "server does not have a screen matching the display" }
    };

    auto iter = errors.find(code);
    return iter == errors.end() ? str_format("#", code) : iter->second;
}

xcb_point_t to_xcb_point(const Point & pt) {
    xcb_point_t xpt;
    xpt.x = pt.x();
    xpt.y = pt.y();
    return xpt;
}

xcb_rectangle_t to_xcb_rectangle(const Rect & r) {
    xcb_rectangle_t xr;
    xr.x = r.left();
    xr.y = r.top();
    xr.width = r.width();
    xr.height = r.height();
    return xr;
}

xcb_gx_t gx_oper(Oper op) {
    static const struct { Oper op; xcb_gx_t gx; } opers[] = {
        { OPER_CLEAR,       XCB_GX_CLEAR    },
        { OPER_SOURCE,      XCB_GX_COPY     },
        { OPER_XOR,         XCB_GX_XOR      },
        { OPER_NOT,         XCB_GX_INVERT   },
        { OPER_SET,         XCB_GX_SET      },
        { OPER_COPY,        XCB_GX_COPY     }
    };

    for (int i = 0; OPER_COPY != opers[i].op; ++i) {
        if (op == opers[i].op) {
            return opers[i].gx;
        }
    }

    return XCB_GX_COPY;
}

xcb_render_pict_op_t xrender_oper(Oper op) {
    static const struct { Oper op; xcb_render_pict_op_t pict_op; } opers[] = {
        { OPER_CLEAR,       XCB_RENDER_PICT_OP_CLEAR        },
        { OPER_SOURCE,      XCB_RENDER_PICT_OP_CLEAR        },
        { OPER_XOR,         XCB_RENDER_PICT_OP_XOR          },
        { OPER_NOT,         XCB_RENDER_PICT_OP_OVER_REVERSE },
        { OPER_COPY,        XCB_RENDER_PICT_OP_OVER         }
    };

    for (int i = 0; OPER_COPY != opers[i].op; ++i) {
        if (op == opers[i].op) {
            return opers[i].pict_op;
        }
    }

    return XCB_RENDER_PICT_OP_OVER;
}

xcb_cap_style_t xcb_cap_style(Cap_style cs) {
    switch (cs) {
        case FLAT_CAP:
            return XCB_CAP_STYLE_PROJECTING;

        case ROUND_CAP:
            return XCB_CAP_STYLE_ROUND;

        default:
            return XCB_CAP_STYLE_NOT_LAST;
    }
}

xcb_line_style_t xcb_line_style(Line_style ls) {
    switch (ls) {
        case DASH_LINE:
            return XCB_LINE_STYLE_ON_OFF_DASH;

        case DASH_DOT_DOT_LINE:
            return XCB_LINE_STYLE_DOUBLE_DASH;

        default:
            return XCB_LINE_STYLE_SOLID;
    }
}

xcb_join_style_t xcb_join_style(Join_style js) {
    switch (js) {
        case BEVEL_JOIN:
            return XCB_JOIN_STYLE_BEVEL;

        case ROUND_JOIN:
            return XCB_JOIN_STYLE_ROUND;

        default:
            return XCB_JOIN_STYLE_MITER;
    }
}

} // namespace tau

//END
