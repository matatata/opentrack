#include "app.h"



INT_PTR CALLBACK DialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

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
                case ID_CLOSE:
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
                    if(HIWORD(wParam)==CBN_CLOSEUP){
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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    HINSTANCE hInstance = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

    switch (uMsg) {
        case WM_DESTROY: {
            DeleteObject(app.hBitmap);
            app.hBitmap = NULL;
            if(bridge) {
                bridge->stop();
                delete bridge;
            }
            if(logger) {
                delete logger;
            }

            PostQuitMessage(0);
        }
        break;
        case WM_CREATE: {
            app.InitWindow(hwnd,hInstance);

            logger = new Log(hwnd,ID_TEXTFIELD);
            logger->logln(string("Opentrack Wine Bridge ") + VERSION );

            settings.GetCurrentSettings();
            bridge = new Bridge(settings.fps,logger,BridgeStatusChangedProc);
            bridge->stop();
            if(settings.proto > 0) {
                bridge->runAsync();
            }

        }
        break;

        case WM_CTLCOLORSTATIC:
        {
            if( (HWND)lParam == GetDlgItem(hwnd, ID_TEXTFIELD) )
            {
                SetBkMode( (HDC)wParam, TRANSPARENT );
                SetTextColor((HDC)wParam, RGB(0, 0, 0));
                return (LRESULT)( (HBRUSH)GetStockObject(WHITE_BRUSH) );
            }
            else
            {
                return DefWindowProc( hwnd, uMsg, wParam, lParam );
            }
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == IDB_CONFIGURE)
            {
                DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), hwnd, DialogProc);
            }
        }
        break;


        case WM_PAINT: {
            app.Paint();
        }
        break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return FALSE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

    setVerbose(getenv("OTR_VERBOSE") != NULL);


    WNDCLASS wc = {0};

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
    wc.lpszClassName = WineBridgeApp::CLASS_NAME;
    wc.lpszMenuName  = NULL;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;


    if (!RegisterClass(&wc)) {
        return 1;
    }

    if(!app.MakeWindow(hInstance)){
        return 1;
    }

    ShowWindow(app.mainHwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if(settings.disable_on_close) {
        clearBothProtocols();
    }
    return 0;
}
