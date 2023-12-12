#include <d2d1.h>
#include <dwrite.h>
#include <strsafe.h>
#include <iostream>
#include <vector>

#include "note.h"
#include "scale.h"

ID2D1HwndRenderTarget *renderTgt = NULL;
ID2D1Factory *winFactory = NULL;

ID2D1SolidColorBrush **brushList = NULL;
int brushColors[] = {
    D2D1::ColorF::Black, //text
    D2D1::ColorF::Cyan,  //play button
    D2D1::ColorF::Green, //check button
    D2D1::ColorF::Black, //lines
    D2D1::ColorF::Blue,  //root notes
    D2D1::ColorF::Green, //fifth notes
    D2D1::ColorF::Magenta, //dissonant notes
    D2D1::ColorF::Black, //other notes
    D2D1::ColorF::Red    //incorrect notes
};
int numBrushes = sizeof(brushColors) / sizeof(int);

IDWriteFactory *writeFactory = NULL;
IDWriteTextFormat *textFormat = NULL;

float textSize = 20.0f;
float padding = 10.0f;
float buttonTop = 50.0f;
float buttonBottom = 100.0f;
float buttonWidth = 100.0f;
int topNote = 120;
int lineSpace = 18;
int windWidth = 2000;

template <class T>
void SafeRelease(T **ppt) {
    if (*ppt) {
        (*ppt)->Release();
        *ppt = NULL;
    }
}

bool createFactoryPaintObjs(HWND hwnd) {
    if (D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &winFactory) != S_OK) {
        std::cout << "Unable to create window factory!" << std::endl;
        return false;
    }
    if (DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                reinterpret_cast<IUnknown**>(&writeFactory)) != S_OK) {
        std::cout << "Unable to create write factory!" << std::endl;
        return false;
    }
    return true;
}

bool createStaticPaintObjs(HWND hwnd) {
    HRESULT hr;

    if (renderTgt == NULL) {
        RECT r;
        GetClientRect(hwnd, &r);
        D2D1_SIZE_U size = D2D1::SizeU(r.right, r.bottom);

        hr = winFactory->CreateHwndRenderTarget(
                D2D1::RenderTargetProperties(),
                D2D1::HwndRenderTargetProperties(hwnd, size),
                &renderTgt);
        if (hr != S_OK) {
            std::cout << "Failed to create render target!" << std::endl;
            return false;
        }
    }

    if (brushList == NULL) {
        brushList = (ID2D1SolidColorBrush **)malloc(sizeof(ID2D1SolidColorBrush *) * numBrushes);
        if (brushList == NULL) {
            std::cout << "Unable to create brush list" << std::endl;
            return false;
        }

        for (int i = 0; i < numBrushes; i++) {
            brushList[i] = NULL;

            D2D1_COLOR_F brushColor = D2D1::ColorF(brushColors[i]);
            hr = renderTgt->CreateSolidColorBrush(brushColor, &(brushList[i]));
            if (hr != S_OK) {
                std::cout << "Unable to create brush " << i << std::endl;
                return false;
            }
        }
    }

    if (textFormat == NULL) {
        hr = writeFactory->CreateTextFormat(L"Ariel", NULL, DWRITE_FONT_WEIGHT_REGULAR,
                DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, textSize, L"en-us",
                &textFormat);
        if (hr != S_OK) {
            std::cout << "Unable to create text format!" << std::endl;
            return false;
        }
    }

    return true;
}

void paint(HWND hwnd, int score, std::vector<std::vector<Note *>> notes) {
    PAINTSTRUCT ps;

    if (!createStaticPaintObjs(hwnd)) {
        return;
    }

    BeginPaint(hwnd, &ps);
    renderTgt->BeginDraw();

    //Clear the window before redrawing
    renderTgt->Clear(D2D1::ColorF(D2D1::ColorF::White));

    int strlen = 15;
    wchar_t *score_text = (wchar_t *)malloc(sizeof(wchar_t) * strlen);
    strlen = swprintf(score_text, strlen, L"Score: %d\n", score);
    if (strlen > 0) {
        D2D1_RECT_F textRect = D2D1::RectF(padding, padding, 1000.0f, textSize + (2.0f * padding));
        renderTgt->DrawText(score_text, wcslen(score_text), textFormat, textRect, brushList[0]);
    }
    free(score_text);

    D2D1_RECT_F playRect = D2D1::RectF(padding, buttonTop, buttonWidth + padding, buttonBottom);
    D2D1_RECT_F playTextRect = D2D1::RectF(padding * 2.0f, buttonTop + padding,
            buttonWidth, buttonBottom - padding);
    renderTgt->FillRectangle(playRect, brushList[1]);
    renderTgt->DrawText(L"Play", wcslen(L"Play"), textFormat, playTextRect, brushList[0]);

    D2D1_RECT_F checkRect = D2D1::RectF(buttonWidth + (2.0f * padding), buttonTop,
        2.0f * (buttonWidth + padding), buttonBottom);
    D2D1_RECT_F checkTextRect = D2D1::RectF(buttonWidth + (3.0f * padding), buttonTop + padding,
        (2.0f * buttonWidth) + padding, buttonBottom - padding);
    renderTgt->FillRectangle(checkRect, brushList[2]);
    renderTgt->DrawText(L"Check", wcslen(L"Check"), textFormat, checkTextRect, brushList[0]);

    //Draw the lines
    for (int i = 0; i <= MAX_NOTE; i++) {
        float width = ((i % 12) == 0) ? 2.0f : 1.0f;
        int mod = i % 12;
        for (int j = 0; j < Scale::NOTES_PER_SCALE; j++) {
            if (mod == Scale::majorScaleMap[j]) {
                D2D1_POINT_2F start = {padding, topNote + (float)(lineSpace * (MAX_NOTE - i))};
                D2D1_POINT_2F end = {windWidth - padding, topNote + (float)(lineSpace * (MAX_NOTE - i))};
                renderTgt->DrawLine(start, end, brushList[3], width, NULL);
            }
        }
    }

    //Draw the notes
    int x = padding + lineSpace;
    for (int i = 0; i < notes.size(); i++) {
        std::vector<Note *>ch = notes.at(i);
        for (Note *n : ch) {
            int y = topNote + (MAX_NOTE - n->value) * lineSpace;
            D2D1_POINT_2F center = D2D1::Point2F(x, y);
            D2D1_ELLIPSE ellipse = D2D1::Ellipse(center, lineSpace, lineSpace / 2);
            int brushIdx = 7;
            if (n->correct == Note::INCORRECT) {
                brushIdx = 8;
            } else if (n->pos == Note::ROOT) {
                brushIdx = 4;
            } else if (n->pos == Note::FIFTH) {
                brushIdx = 5;
            } else if (n->pos == Note::SEVENTH) {
                brushIdx = 6;
            }
            renderTgt->FillEllipse(ellipse, brushList[brushIdx]);
        }
        x += lineSpace * 2 + padding;
    }
    renderTgt->EndDraw();
    EndPaint(hwnd, &ps);
}

void resize(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    D2D1_SIZE_U sizeU = D2D1::SizeU(rc.right, rc.bottom);
    if (renderTgt != NULL) {
        renderTgt->Resize(sizeU);
    }
    lineSpace = (rc.bottom - topNote) / MAX_NOTE;
    windWidth = rc.right;
    InvalidateRect(hwnd, NULL, FALSE);
}

void destroy() {
    SafeRelease(&winFactory);
    SafeRelease(&writeFactory);
    PostQuitMessage(0);
}
