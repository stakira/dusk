
library dusk.window;

// different from `love`, `dusk` use zero based display index.

import 'dart:convert' show JSON;

List<double> fromPixels(num px, num py)
    native 'DuskNative_Window_FromPixels';

List<int> _getDesktopDimensions(int display)
    native 'DuskNative_Window_GetDesktopDimensions';

List<int> getDesktopDimensions([int display]) {
    if (display == null) {
        display = GetPosition()[2];
    }
    return _getDesktopDimensions(display);
}

int getDisplayCount()
    native 'DuskNative_Window_GetDisplayCount';

String _getDisplayName(int display)
    native 'DuskNative_Window_GetDisplayName';

String getDisplayName([int display]) {
    if (display == null) {
        display = GetPosition()[2];
    }
    return _getDisplayName(display);
}

List getFullscreen()
    native 'DuskNative_Window_GetFullscreen';

List<List<int>> _getFullscreenModes(int display)
    native 'DuskNative_Window_GetFullscreenModes';

List<List<int>> getFullscreenModes([int display]) {
    if (display == null) {
        display = GetPosition()[2];
    }
    return _getFullscreenModes(display);
}

void getIcon() // todo
    native 'DuskNative_Window_GetIcon';

String _getMode()
    native 'DuskNative_Window_GetMode';

Map getMode() {
    String json = _getMode();
    return JSON.decode(json);
}

double getPixelScale()
    native 'DuskNative_Window_GetPixelScale';

List<int> getPosition()
    native 'DuskNative_Window_GetPosition';

String getTitle()
    native 'DuskNative_Window_GetTitle';

bool hasFocus()
    native 'DuskNative_Window_HasFocus';

bool hasMouseFocus()
    native 'DuskNative_Window_HasMouseFocus';

bool isCreated()
    native 'DuskNative_Window_IsCreated';

bool isVisible()
    native 'DuskNative_Window_IsVisible';

void minimize()
    native 'DuskNative_Window_Minimize';

void maximize()
    native 'DuskNative_Window_Maximize';

bool setFullscreen(bool fullscreen, [bool exclusive = true])
    native 'DuskNative_Window_SetFullscreen';

void setIcon() // todo
    native 'DuskNative_Window_SetIcon';

bool setMode(int w, int h, [Map flags])
    native 'DuskNative_Window_SetMode';

void _setPosition(int x, int y, int display)
    native 'DuskNative_Window_SetPosition';

void setPosition(int x, int y, [int display]) {
    if (display == null) {
        display = GetPosition()[2];
    }
    _setPosition(x, y, display);
}

void setTitle(String title)
    native 'DuskNative_Window_SetTitle';

void showMessageBox() // todo
    native 'DuskNative_Window_ShowMessageBox';

List<double> toPixels(double x, double y)
    native 'DuskNative_Window_ToPixels';
