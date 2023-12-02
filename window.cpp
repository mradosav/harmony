#include <windows.h>
#include <windowsx.h>

#include <vector>
#include <unordered_map>

#include "progression.h"
#include "note.h"

const char *className = "Harmony";

int guessCost = 2;
int correctAdd = 3;

extern bool createFactoryPaintObjs(HWND hwnd);
extern void paint(HWND hwnd, int score, std::vector<std::vector<Note *>> notes);
extern void destroy();
extern void resize(HWND hwnd);
extern void play(std::vector<std::vector<Note *>> notes);

extern float buttonTop;
extern float buttonBottom;
extern float padding;
extern float buttonWidth;
extern int topNote;
extern int lineSpace;

Progression progression;
std::unordered_map<int, Note::NoteStatus> guess;
int score;

void checkGuess() {
    for (auto iter = guess.begin(); iter != guess.end(); ++iter) {
        if (iter->second == Note::NOT_CHECKED) {
            iter->second = Note::INCORRECT;
            score -= guessCost;
        }
    }

    std::vector<Note *> compare = progression.getProgression().back();
    bool correct = true;
    for (Note *n : compare) {
        auto check = guess.find(n->value);
        if (check == guess.end()) {
            correct = false;
        } else {
            if (check->second == Note::INCORRECT) {
                check->second = Note::CORRECT;
                score += correctAdd;
            }
        }
    }

    if (correct) {
        progression.generateNextChord();
        guess.clear();
    }
}

LRESULT CALLBACK msgHndlr(HWND mHwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE:
            if (createFactoryPaintObjs(mHwnd)) {
                return 0;
            } else {
                return -1;
            }
        case WM_PAINT:
        {
            std::vector<std::vector<Note *>> drawNotes = progression.getProgression();
            drawNotes.pop_back();
            std::vector<Note *> drawGuess;
            for (std::pair<int, Note::NoteStatus> i : guess) {
                Note *add = new Note(i.first);
                add->correct = i.second;
                drawGuess.push_back(add);
            }
            drawNotes.push_back(drawGuess);
            paint(mHwnd, score, drawNotes);
            return 0;
        }
        case WM_DESTROY:
            destroy();
            return 0;
        case WM_SIZE:
            resize(mHwnd);
            return 0;
        case WM_LBUTTONDOWN:  //handle mouse click
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);

            if ((pt.y > buttonTop) && (pt.y < buttonBottom)) {
                if ((pt.x > padding) && (pt.x < (padding + buttonWidth))) {
                    play(progression.getProgression());
                } else if ((pt.x > (2*padding + buttonWidth)) && (pt.x < 2*(padding + buttonWidth))) {
                    checkGuess();
                }
                    
            } else if (pt.y > topNote) {
                int noteVal = MAX_NOTE - ((pt.y - topNote) / lineSpace);
                guess.emplace(noteVal, Note::NOT_CHECKED);
            }
            InvalidateRect(mHwnd, NULL, FALSE);
            return 0;
        }
        case WM_RBUTTONDOWN:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            if (pt.y > topNote) {
                int noteVal = MAX_NOTE - ((pt.y - topNote) / lineSpace);
                if ((guess.count(noteVal) == 1)  && (guess.at(noteVal) != Note::CORRECT)) {
                    guess.erase(noteVal);
                }
            }
            InvalidateRect(mHwnd, NULL, FALSE);
        }
        default:
            return DefWindowProc(mHwnd, msg, wParam, lParam);
    }

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
        int nCmdShow)
{
    WNDCLASSEX wcx = {0};
    HWND hwnd;
    MSG msg;

    score = 0;

    wcx.cbSize = sizeof(WNDCLASSEX);
    wcx.style = 0;
    wcx.lpfnWndProc = msgHndlr;
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
    wcx.hInstance = hInstance;
    wcx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszMenuName = NULL;
    wcx.lpszClassName = className;
    wcx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wcx)) {
        return 0;
    }

    hwnd = CreateWindowEx(
            WS_EX_CLIENTEDGE,
            className,
            "Harmony",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 2400, 1200,
            NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
