#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <stdio.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int getDistance(int x1 , int y1 , int x2 , int y2 ) {
    return (int)sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

void drawCircle(HDC hdc, int xc, int yc, int r, COLORREF color) {
    for (double t = 0 ; t < 6.28 ; t += 0.001) {
        int x = xc + (int)(r * cos(t));
        int y = yc + (int)(r * sin(t));
        SetPixel(hdc, x, y, color);
    }
}

void midpointLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (true)
    {
        SetPixel(hdc, x1, y1, color);

        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;

        if (e2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }

        if (e2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

int x[3], y[3];
int clickCount = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (uMsg)
    {
        case WM_LBUTTONDOWN:
        {
            if (clickCount < 3)
            {
                x[clickCount] = GET_X_LPARAM(lParam);
                y[clickCount] = GET_Y_LPARAM(lParam);

                hdc = GetDC(hwnd);
                SetPixel(hdc, x[clickCount], y[clickCount], RGB(255, 0, 0));
                ReleaseDC(hwnd, hdc);

                clickCount++;

                if (clickCount == 3)
                {
                    hdc = GetDC(hwnd);

                    int xc = x[0];
                    int yc = y[0];

                    int R1 = getDistance(x[0], y[0], x[1], y[1]);
                    int R2 = getDistance(x[0], y[0], x[2], y[2]);

                    int maxR = (R1 > R2) ? R1 : R2;
                    int minR = (R1 < R2) ? R1 : R2;

                    drawCircle(hdc, xc, yc, R1, RGB(0, 0, 255));
                    drawCircle(hdc, xc, yc, R2, RGB(0, 255, 0));

                    for (int r = maxR; r >= minR; r--)
                    {
                        drawCircle(hdc, xc, yc, r, RGB(200, 200, 0));
                    }


                    double angle1 = 0;
                    double angle2 = 2.0;

                    int x1 = xc + (int)(R1 * cos(angle1));
                    int y1 = yc + (int)(R1 * sin(angle1));

                    int x2 = xc + (int)(R2 * cos(angle2));
                    int y2 = yc + (int)(R2 * sin(angle2));

                    midpointLine(hdc, xc, yc, x1, y1, RGB(255, 0, 255));
                    midpointLine(hdc, xc, yc, x2, y2, RGB(0, 255, 255));

                    ReleaseDC(hwnd, hdc);

                    clickCount = 0;
                }
            }
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "MyWindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Circle Graphics App",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}