#include "screen.h"

void Screen::drawGraphics(wxDC &dc, Chip8 chip8)
{
    printf("Drawing graphics\n");
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        int x = (i % SCREEN_WIDTH) * 20 + 10;
        int y = (i / SCREEN_WIDTH) * 20 + 10;

        if (chip8.gfx[i] == 1)
            dc.SetPen(wxPen(wxColor(0, 0, 0), 10));

        if (chip8.gfx[i] == 0)
            dc.SetPen(wxPen(wxColor(255, 255, 255), 10));

        dc.DrawRectangle(x, y, 10, 10);
    }
}

void Screen::setupGraphics()
{
    printf("Setting up graphics\n");
}

void Screen::setupInput()
{
    printf("Setting up input\n");
}

bool MyApp::OnInit()
{
    wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    frame = new wxFrame((wxFrame *)NULL, -1, wxT("CHIP-8 Emulator"), wxPoint(50, 50), wxSize(1300, 700));

    drawPane = new BasicDrawPane((wxFrame *)frame);
    sizer->Add(drawPane, 1, wxEXPAND);

    frame->SetSizer(sizer);
    frame->SetAutoLayout(true);

    frame->Show();

    // Initialize the Chip8 system and load the game into the memory
    drawPane->myChip8.initialize();                 // Clear the memory, registers and screen
    drawPane->myChip8.loadGame("../roms/pong.ch8"); // Copy the program into the memory

    return true;
}

BEGIN_EVENT_TABLE(BasicDrawPane, wxPanel)
// some useful events
/*
 EVT_MOTION(BasicDrawPane::mouseMoved)
 EVT_LEFT_DOWN(BasicDrawPane::mouseDown)
 EVT_LEFT_UP(BasicDrawPane::mouseReleased)
 EVT_RIGHT_DOWN(BasicDrawPane::rightClick)
 EVT_LEAVE_WINDOW(BasicDrawPane::mouseLeftWindow)
 EVT_MOUSEWHEEL(BasicDrawPane::mouseWheelMoved)
 */

// catch key events
EVT_KEY_DOWN(BasicDrawPane::keyPressed)
EVT_KEY_UP(BasicDrawPane::keyReleased)

// catch paint events
EVT_PAINT(BasicDrawPane::paintEvent)

END_EVENT_TABLE()

// some useful events
/*
 void BasicDrawPane::mouseMoved(wxMouseEvent& event) {}
 void BasicDrawPane::mouseDown(wxMouseEvent& event) {}
 void BasicDrawPane::mouseWheelMoved(wxMouseEvent& event) {}
 void BasicDrawPane::mouseReleased(wxMouseEvent& event) {}
 void BasicDrawPane::rightClick(wxMouseEvent& event) {}
 void BasicDrawPane::mouseLeftWindow(wxMouseEvent& event) {}
 */

void BasicDrawPane::keyPressed(wxKeyEvent &event)
{
    myChip8.setKeyDown((int)event.GetKeyCode());

    printf("Key pressed... %d\n", (int)event.GetKeyCode());
}

void BasicDrawPane::keyReleased(wxKeyEvent &event)
{
    myChip8.setKeyUp((int)event.GetKeyCode());

    printf("Key released... %d\n", (int)event.GetKeyCode());
}

BasicDrawPane::BasicDrawPane(wxFrame *parent) : wxPanel(parent)
{
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void BasicDrawPane::paintEvent(wxPaintEvent &evt)
{
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 *
 * In most cases, this will not be needed at all; simply handling
 * paint events and calling Refresh() when a refresh is needed
 * will do the job.
 */
void BasicDrawPane::paintNow()
{
    wxClientDC dc(this);
    render(dc);
}

/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void BasicDrawPane::render(wxDC &dc)
{
    // Emulate one cycle
    myChip8.emulateCycle();

    // If the draw flag is set, update the screen
    if (myChip8.draw_flag) // Only two opcodes should set this flag: 0x00E0 (Clears the screen) and 0xDXYN (Draws a sprite on the screen)
    {
        myChip8.draw_flag = false;
    }

    myScreen.drawGraphics(dc, myChip8);
    Update();
    Refresh();
    usleep(10000);
}