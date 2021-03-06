
library dusk.graphics;

import 'dart:math' show max;

/**
 * Drawing
 */
void _arc(bool filled, num x, num y,
    num radius, num angle1, num angle2, int points)
    native 'DuskNative_Graphics_Arc';

void _ellipse(bool filled, num x, num y,
    num a, num b, int points)
    native 'DuskNative_Graphics_Ellipse';

void _circle(bool filled, num x, num y,
    num radius, int points)
    native 'DuskNative_Graphics_Circle';

void _print(String text, num x, num y,
    num angle, num sx, num sy,
    num ox, num oy, num kx, num ky)
    native 'DuskNative_Graphics_Print';

void _printf(String text, num x, num y,
    num wrap, int alignment,
    num angle, num sx, num sy,
    num ox, num oy, num kx, num ky)
    native 'DuskNative_Graphics_Printf';

void _rectangle(bool filled, num x, num y,
    num w, num h, num rx, num ry, int points)
    native 'DuskNative_Graphics_Rectangle';

void arc(bool filled, num x, num y,
    num radius, num angle1, num angle2, [int points]) {
    if (points == null) {
        points = radius > 10 ? radius.floor() : 10;
    }
    _arc(filled, x, y, radius, angle1, angle2, points);
}

void ellipse(bool filled, num x, num y,
    num a, num b, [int points]) {
    if (points == null) {
        points = a + b > 30 ? ((a + b) / 2).floor() : 15;
    }
    _ellipse(filled, x, y, a, b, points);
}

void circle(bool filled, num x, num y,
    num radius, [int points]) {
    if (points == null) {
        points = radius > 10 ? radius.floor() : 10;
    }
    _circle(filled, x, y, radius, points);
}

void clear()
    native 'DuskNative_Graphics_Clear';

void draw()
    native 'DuskNative_Graphics_Draw';

void line(num x1, num y1, num x2, num y2)
    native 'DuskNative_Graphics_Line';

void point(num x, num y)
    native 'DuskNative_Graphics_Point';

void polygon(bool filled, List<num> vertices)
    native 'DuskNative_Graphics_Polygon';

void polyline(List<num> vertices)
    native 'DuskNative_Graphics_Polyline';

void present()
    native 'DuskNative_Graphics_Present';

void print(String text, num x, num y,
    [num angle = 0, num sx = 1, num sy = null,
    num ox = 0, num oy = 0,
    num kx = 0, num ky = 0]) {
    if (sy == null) {
        sy = sx;
    }
    _print(text, x, y, angle, sx, sy, ox, oy, kx, ky);
}

void printf(String text, num x, num y,
    num limit, [String align = 'left',
    num angle = 0, num sx = 1, num sy = null,
    num ox = 0, num oy = 0,
    num kx = 0, num ky = 0]) {
    // handle scale
    if (sy == null) {
        sy = sx;
    }

    // handle alignment
    int _align = 0;
    switch (align) {
      case 'left':
        _align = 0; break;
      case 'center':
        _align = 1; break;
      case 'right':
        _align = 2; break;
      case 'justify':
        _align = 3; break;
      default;
        throw new ArgumentError("Incorrect alignment: $align");
    }

    _printf(text, x, y, limit, _align, angle, sx, sy, ox, oy, kx, ky);
}

void rectangle(bool filled, num x, num y,
    num w, num h, [num rx = 0, num ry = 0, int points]) {
    if (rx != 0 || ry != 0) {
        points = max(rx, ry) > 20.0 ? (max(rx, ry) / 2).floor() : 10;
    }
    _rectangle(filled, x, y, w, h, rx, ry, points);
}

/**
 * Coordinate system
 */
void origin()
    native 'DuskNative_Graphics_Origin';

void pop()
    native 'DuskNative_Graphics_Pop';

void push()
    native 'DuskNative_Graphics_Push';

void rotate(num angle)
    native 'DuskNative_Graphics_Rotate';

void _scale(num sx, num sy)
    native 'DuskNative_Graphics_Scale';

void scale(num sx, [num sy]) {
    if (sy == null) { sy = sx; }
    _scale(sx, sy);
}

void shear(num kx, num ky)
    native 'DuskNative_Graphics_Shear';

void translate(num dx, num dy)
    native 'DuskNative_Graphics_Translate';


/**
* Object creation
*/

/**
 * Font creation
 */
class Font {
    final String filename;
    final int size;
    int _ptr = 0;

    Font._dummy(this.filename, this.size);

    // private constructor
    factory Font._font(String filename, int size) {
        Font dummy = new Font._dummy(filename, size);
        dummy._ptr = _newFont(filename, size, dummy);
        return dummy;
    }

    num getAscent() { return 0; }
    num getBaseline() { return 0; }
    num getDescent() { return 0; }
    Map getFilter() { return {}; }
    num getHeight() { return 0; }
    num getLineHeight() { return 0; }
    num getWidth(String line) { return 0; }
    Map getWrap(String text, num width) { return {}; }
    bool hasGlyphs(String text) { return false; } // todo : overloaded?
    void setFilter(String min, String mag, int anisotropy) {}
    void setLineHeight(num height) {}
}

int _newFont(String filename, int size, Font dummy)
    native 'DuskNative_Graphics_NewFont';

Font newFont(var filenameOrSize, [var size = null]) {
    if (filenameOrSize is String && size != null && size is int) {
        return new Font._font(filenameOrSize, size);
    }
    else if (filenameOrSize is int && size == null) {
        return new Font._font(null, filenameOrSize);
    }
    else {
        throw new ArgumentError('Invalid arguments');
    }
}

void _setFont(int ptr)
    native 'DuskNative_Graphics_SetFont';

void setFont(Font font) {
    _setFont(font._ptr);
}

void setNewFont(var filenameOrSize, [var size = null]) {
    Font font = newFont(filenameOrSize, size);
    setFont(font);
}
