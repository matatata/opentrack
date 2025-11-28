#include "app.h"

INT_PTR CALLBACK SettingsDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static void BridgeStatusChangedProc(bridge_status_t newStatus){
    switch (newStatus)
    {
    case bridge_started:
        app.bridgeStatus = "Active";
        break;
    case bridge_stopped:
        app.bridgeStatus = "Inactive";
        break;
    case bridge_failed:
        app.bridgeStatus = "Error";
        break;
    default:
        break;
    }

    app.RedrawStatusText();
}

INT_PTR CALLBACK MainDialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam) {
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hDlg, GWLP_HINSTANCE);

    switch(msg) {
        case WM_DESTROY: {
            if(bridge) {
                bridge->stop();
                delete bridge;
            }
            if(logger) {
                delete logger;
            }
            cout << "Goodbye" << endl;
            if(settings.disable_on_close) {
                cout << "Clearing protocols" << endl;;
                clearBothProtocols();
            }
            PostQuitMessage(0);
        }
        break;

        case WM_INITDIALOG:
        {
            HBITMAP hBitmap = (HBITMAP)LoadImage(hInstance, MAKEINTRESOURCE(IDB_BITMAP1),
                                         IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);

            HWND hPictureCtrl = GetDlgItem(hDlg, ID_STATICPIC);
            RECT rect;
            GetClientRect(hPictureCtrl, &rect);

            // Create scaled bitmap
            HDC hdcScreen = GetDC(NULL);
            HDC hdcMem = CreateCompatibleDC(hdcScreen);
            HDC hdcMemScaled = CreateCompatibleDC(hdcScreen);

            HBITMAP hScaledBitmap = CreateCompatibleBitmap(hdcScreen,
                                                            rect.right, rect.bottom);

            SelectObject(hdcMem, hBitmap);
            SelectObject(hdcMemScaled, hScaledBitmap);

            // Get original bitmap size
            BITMAP bm;
            GetObject(hBitmap, sizeof(BITMAP), &bm);

            // Scale using StretchBlt
            SetStretchBltMode(hdcMemScaled, HALFTONE);
            StretchBlt(hdcMemScaled, 0, 0, rect.right, rect.bottom,
                    hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

            SendMessage(hPictureCtrl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hScaledBitmap);

            DeleteDC(hdcMem);
            DeleteDC(hdcMemScaled);
            ReleaseDC(NULL, hdcScreen);
            DeleteObject(hBitmap);


            app.InitWindow(hDlg,hInstance);

            logger = new Log(hDlg,ID_TEXTFIELD);
            logger->logln(string("Opentrack Wine Bridge ") + VERSION );

            settings.GetCurrentSettings();
            bridge = new Bridge(settings.fps,logger,BridgeStatusChangedProc);
            bridge->stop();
            if(settings.proto > 0) {
                bridge->runAsync();
            }

            return TRUE;
        }
        // case WM_CLOSE:
        //     EndDialog(hDlg, 0);
        // break;
        // prevent ESC from closing window:
        // ignore WM_CLOSE and
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_CLOSE) DestroyWindow(hDlg);
        break;
        case WM_COMMAND: {
            switch (wParam)
            {
                case MAKEWPARAM(IDOK, BN_CLICKED):
                case MAKEWPARAM(IDCANCEL, BN_CLICKED):
                    // ignore this
                    return TRUE;
                case MAKEWPARAM(IDB_CONFIGURE, BN_CLICKED):
                {
                    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hDlg, SettingsDialogProc);
                    return TRUE;
                }
            }
        }
        break;
        case WM_CTLCOLORDLG:
        {
            // Create a white brush
            HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
            return (INT_PTR)hBrush;
        }
        case WM_CTLCOLORSTATIC:
        {
            if( (HWND)lParam == GetDlgItem(hDlg, ID_TEXTFIELD)
                || (HWND)lParam == GetDlgItem(hDlg, ID_STATUS1)
                )
            {
                SetBkMode( (HDC)wParam, TRANSPARENT );
                SetTextColor((HDC)wParam, RGB(0, 0, 0));
                return (LRESULT)( (HBRUSH)GetStockObject(WHITE_BRUSH) );
            }
            else
            {
                return FALSE;
            }
        }

    }
    return FALSE;
}

INT_PTR CALLBACK SettingsDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

    switch (msg) {
        case WM_DESTROY: {

        }
        break;
        case WM_INITDIALOG:{
            settings.InitDialog(hwnd);
            return TRUE;
        }

        case WM_COMMAND:
            switch LOWORD(wParam) {
                case IDCANCEL:
                    settings.Revert();
                    EndDialog(hwnd, 0);
                break;
                case ID_WRITE: {
                    settings.WriteSettings();
                    EndDialog(hwnd, 0);
                }
                break;
                case ID_REVERT: {
                    settings.Revert();
                    settings.UpdateGui();
                }
                break;
                case ID_PROTO: {
                    if(HIWORD(wParam)==CBN_SELCHANGE){
                        settings.ApplyProtoSelection(settings.GetProtocols());
                    }
                }
                break;
                case ID_DISABLE_ON_CLOSE:{
                    settings.disable_on_close = IsDlgButtonChecked(hwnd, ID_DISABLE_ON_CLOSE);
                }
                break;
                case ID_FPS:{
                    if(HIWORD(wParam) == EN_CHANGE){
                        char buf[4]={};
                        GetWindowText(GetDlgItem(hwnd,ID_FPS), buf, sizeof(buf));
                        int newfps = atoi(buf);
                        if(bridge->setFPS(newfps)){
                            settings.fps = newfps;
                        }
                    }
                }
                break;
        }
        break; // WM_COMMAND

        case WM_CLOSE:
            settings.Revert();
            EndDialog(hwnd, 0);
            break;

    }
    return FALSE;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    setVerbose(getenv("OTR_VERBOSE") != NULL);
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), NULL, MainDialogProc);
}
