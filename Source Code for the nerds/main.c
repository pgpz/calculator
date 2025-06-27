#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>  // 
#include <ctype.h>
#include <string.h>

#define INPUT_SIZE 100


WNDPROC oldProc = NULL;


HANDLE hConsole;


void setColorGreen() {
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
}

void setColorRed() {
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
}

void resetColor() {
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}


LRESULT CALLBACK CustomWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_SETTEXT: {
        LRESULT resSetText = CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
        RedrawWindow(hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
        return resSetText;
    }
    case WM_NCPAINT: {
        LRESULT res = CallWindowProc(oldProc, hwnd, msg, wParam, lParam);

        HDC hdc = GetWindowDC(hwnd);
        if (hdc) {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            OffsetRect(&rect, -rect.left, -rect.top);
            rect.bottom = 30;

            wchar_t title[256];
            GetWindowTextW(hwnd, title, sizeof(title) / sizeof(title[0]));

            HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 128));
            FillRect(hdc, &rect, hBrush);
            DeleteObject(hBrush);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));

            NONCLIENTMETRICS ncm = { sizeof(ncm) };
            SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
            HFONT hFont = CreateFontIndirect(&ncm.lfCaptionFont);
            HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

            DrawTextW(hdc, title, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(hdc, oldFont);
            DeleteObject(hFont);
            ReleaseDC(hwnd, hdc);
        }
        return res;
    }
    case WM_NCACTIVATE: {
        LRESULT resNCA = CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
        RedrawWindow(hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
        return resNCA;
    }
    default:
        return CallWindowProc(oldProc, hwnd, msg, wParam, lParam);
    }
}

int main() {
    HWND hwnd = GetConsoleWindow();

    
    SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_APPWINDOW);

    
    HICON hIcon = (HICON)LoadImage(NULL, "icon.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    if (hIcon) {
        
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
        SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

       
        SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)hIcon);
        SetClassLongPtr(hwnd, GCLP_HICONSM, (LONG_PTR)hIcon);
    }

    
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    
    oldProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)CustomWndProc);

    SetConsoleTitle("pgp's shitty calculator");

    double num1 = 0, num2, result = 0;
    char op;
    int have_result = 0;

    char input[INPUT_SIZE];

    printf("pgp's shitty calculator\n");
    printf("written in C\n");
    printf("equation format: [number] [+,-,*,/] [number]\n");
    printf("equation example: 10.5 / 3\n");
    printf("You can also chain calculations by entering: [operator] [number]\n\n");

    while (1) {
        printf("enter equation: ");

        if (!fgets(input, sizeof(input), stdin)) {
            printf("\nInput error.\n");
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strlen(input) == 0) continue;

        char* p = input;
        while (isspace(*p)) p++;

        if (have_result) {
            if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
                op = *p;
                p++;
                while (isspace(*p)) p++;
                if (sscanf(p, "%lf", &num2) != 1) {
                    setColorRed();
                    printf("Invalid number for chaining.\n");
                    resetColor();
                    continue;
                }
                num1 = result;
            }
            else {
                if (sscanf(p, "%lf %c %lf", &num1, &op, &num2) != 3) {
                    setColorRed();
                    printf("Invalid input format.\n");
                    resetColor();
                    continue;
                }
            }
        }
        else {
            if (sscanf(p, "%lf %c %lf", &num1, &op, &num2) != 3) {
                setColorRed();
                printf("Invalid input format.\n");
                resetColor();
                continue;
            }
            have_result = 1;
        }

        switch (op) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/':
            if (num2 == 0) {
                setColorRed();
                printf("Error: division by zero.\n");
                resetColor();
                continue;
            }
            result = num1 / num2;
            break;
        default:
            setColorRed();
            printf("Unsupported operator: %c\n", op);
            resetColor();
            continue;
        }

        char titlebuf[100];
        snprintf(titlebuf, sizeof(titlebuf), "pgp's shitty calculator - Result: %.10g", result);
        SetConsoleTitle(titlebuf);

        setColorGreen();
        printf("Result: %.10g\n\n", result);
        resetColor();

        printf("Press Ctrl+L to clear console, or any other key + Enter to continue...\n");

        while (!_kbhit()) {}
        int ch = _getch();

        if (ch == 12) {
            system("cls");
        }
    }

    
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oldProc);

    return 0;
}
