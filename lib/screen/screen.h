#ifndef SCREEN_H
#define SCREEN_H

#include <cstdio>
#include <iostream>
#include <unistd.h>
#include "chip-8.h"

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/defs.h>

class Screen
{
public:
    void setupGraphics();
    void setupInput();
    void drawGraphics(wxDC &dc, Chip8 chip8);
};

class BasicDrawPane : public wxPanel
{

public:
    Screen myScreen;
    Chip8 myChip8;

    BasicDrawPane(wxFrame *parent);

    void paintEvent(wxPaintEvent &evt);
    void paintNow();

    void render(wxDC &dc);

    void keyPressed(wxKeyEvent &event);
    void keyReleased(wxKeyEvent &event);

    // some useful events
    /*
     void mouseMoved(wxMouseEvent& event);
     void mouseDown(wxMouseEvent& event);
     void mouseWheelMoved(wxMouseEvent& event);
     void mouseReleased(wxMouseEvent& event);
     void rightClick(wxMouseEvent& event);
     void mouseLeftWindow(wxMouseEvent& event);
     */

    DECLARE_EVENT_TABLE()
};

class MyApp : public wxApp
{
    bool OnInit();

    wxFrame *frame;
    BasicDrawPane *drawPane;

public:
};

#endif /* SCREEN_H */