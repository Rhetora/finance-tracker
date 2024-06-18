#include "../include/Account.h"
#include "wx/wx.h"
#include <wx/spinctrl.h>
#include "wx/grid.h"
#include "wx/mathplot.h"
#include <locale.h>

// Application class
class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
};

// HOME: Frame class
class HomeFrame : public wxFrame
{
public:
    HomeFrame(const wxString &title);

    // Event handlers
    void OnQuit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnAddAccount(wxCommandEvent &event);
    void OnVisualise(wxCommandEvent &event);
    void OnSaveSummary(wxCommandEvent &event);
    void OnGridCellChange(wxGridEvent &event);

    // Public methods to update frame contents
    void LoadData();

    

    // Store program data
    SavedData savedData;

private:
    // Helper functions to set up frame contents
    void CreateMenu();
    wxGrid *CreateGrid();
    void CreateSummaryBoxes();
    void InitializeGrid();
    // void SetSizerAndFit();

    // wx Components for the frame
    wxGrid *grid;
    wxStaticText *summaryBoxes[8];
    wxButton *saveSummaryButton;

    wxDECLARE_EVENT_TABLE();
};

// ACCOUNTADD: Frame class
class AccountAddFrame : public wxFrame
{
public:
    AccountAddFrame(wxWindow *parent);

private:
    // Event handlers and variables to store input values
    void OnSubmit(wxCommandEvent &event);
    wxTextCtrl *nameCtrl;
    wxTextCtrl *bankCtrl;
    wxSpinCtrlDouble *balanceCtrl;
    wxSpinCtrlDouble *interestCtrl;
    wxTextCtrl *typeCtrl;

    wxDECLARE_EVENT_TABLE();
};

// VISUALISE Frame class
class VisualiseFrame : public wxFrame
{
public:
    VisualiseFrame(wxWindow *parent);

private:
    void CreatePlot();

    wxDECLARE_EVENT_TABLE();
};

// Event table
enum
{
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT,
    Add_Account = 1,
    Submit_Account = 2,
    Save_Summary = 3,
    Visualise = 4
};

wxBEGIN_EVENT_TABLE(HomeFrame, wxFrame)
    EVT_MENU(Minimal_Quit, HomeFrame::OnQuit)
        EVT_MENU(Minimal_About, HomeFrame::OnAbout)
            EVT_MENU(Add_Account, HomeFrame::OnAddAccount)
                EVT_MENU(Visualise, HomeFrame::OnVisualise)
                    EVT_BUTTON(Save_Summary, HomeFrame::OnSaveSummary)
                        wxEND_EVENT_TABLE()

                            wxBEGIN_EVENT_TABLE(AccountAddFrame, wxFrame)
                                EVT_BUTTON(Submit_Account, AccountAddFrame::OnSubmit)
                                    wxEND_EVENT_TABLE()
                                        wxBEGIN_EVENT_TABLE(VisualiseFrame, wxFrame)

                                            wxEND_EVENT_TABLE()

                                                wxIMPLEMENT_APP(MyApp);

// Application initialization
bool MyApp::OnInit()
{
    setlocale(LC_NUMERIC, "");
    if (!wxApp::OnInit())
        return false;

    HomeFrame *frame = new HomeFrame("Personal Finance Tracker");
    frame->Show(true);

    return true;
}

// HOME: Frame constructor
HomeFrame::HomeFrame(const wxString &title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) // Set the size here
{
    // Create frame elements
    CreateMenu();
    grid = CreateGrid();
    CreateSummaryBoxes();

    // Load initial data
    savedData = SavedData();
    LoadData();

    // Bind event handler for cell value changes
    grid->Bind(wxEVT_GRID_CELL_CHANGED, &HomeFrame::OnGridCellChange, this);
}

// HOME: Create UI elements
void HomeFrame::CreateMenu()
{
#if wxUSE_MENUBAR
    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(Add_Account, "Add Account", "Add a financial account");
    fileMenu->Append(Visualise, "Visualise", "Visualise financial history");
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);
#else
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton *aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &HomeFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
    SetSizer(sizer);
#endif

#if wxUSE_STATUSBAR
    CreateStatusBar(1);
    SetStatusText("Welcome to Personal Finance Tracker!");
#endif
}

void HomeFrame::CreateSummaryBoxes()
{
    // Create a box sizer for the left side with 8 boxes
    wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);

    summaryBoxes[0] = new wxStaticText(this, wxID_ANY, "Total Balance: ");
    summaryBoxes[1] = new wxStaticText(this, wxID_ANY, "Current Balance: ");
    summaryBoxes[2] = new wxStaticText(this, wxID_ANY, "Savings Balance: ");
    summaryBoxes[3] = new wxStaticText(this, wxID_ANY, "Credit Balance: ");
    summaryBoxes[4] = new wxStaticText(this, wxID_ANY, "ISA Balance: ");
    summaryBoxes[5] = new wxStaticText(this, wxID_ANY, "GIA Balance: ");
    summaryBoxes[6] = new wxStaticText(this, wxID_ANY, "Crypto Balance: ");
    summaryBoxes[7] = new wxStaticText(this, wxID_ANY, "Total Interest: ");
    for (int i = 0; i < 8; ++i)
    {
        leftSizer->Add(summaryBoxes[i], 0, wxEXPAND | wxALL, 5);
    }

    // Create a horizontal sizer to hold the left boxes and the grid
    wxBoxSizer *mainSizer = new wxBoxSizer(wxHORIZONTAL);
    mainSizer->Add(leftSizer, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(grid, 1, wxEXPAND | wxALL, 5);

    // Create the action button and add it below the grid
    wxBoxSizer *bottomSizer = new wxBoxSizer(wxHORIZONTAL);
    saveSummaryButton = new wxButton(this, Save_Summary, "Save Summary");
    bottomSizer->Add(saveSummaryButton, 0, wxALIGN_LEFT | wxALL, 5);

    // Create a vertical sizer to combine the main sizer and the bottom button
    wxBoxSizer *verticalSizer = new wxBoxSizer(wxVERTICAL);
    verticalSizer->Add(mainSizer, 1, wxEXPAND | wxALL, 5);
    verticalSizer->Add(bottomSizer, 0, wxALIGN_LEFT | wxALL, 5);

    // Set the sizer for the frame
    SetSizerAndFit(verticalSizer);
}

wxGrid *HomeFrame::CreateGrid()
{
    grid = new wxGrid(this, wxID_ANY);
    grid->CreateGrid(0, 5); // Initially 0 rows, 5 columns
    grid->SetColLabelValue(0, "Bank");
    grid->SetColLabelValue(1, "Name");
    grid->SetColLabelValue(2, "Balance");
    grid->SetColLabelValue(3, "Interest");
    grid->SetColLabelValue(4, "Type");
    grid->AutoSizeColumns(); // Automatically size columns to fit content
    grid->HideRowLabels();   // Hide row numbers
    return grid;
}

// HOME: Update data in UI
void HomeFrame::LoadData()
{
    savedData.currentSummary_ = FinanceSummary(savedData.accountList_);
    FinanceSummary summary = savedData.currentSummary_;

    // Update grid with account data
    if (grid->GetNumberRows() > 0)
        grid->DeleteRows(0, grid->GetNumberRows());
    for (const Account &account : savedData.accountList_)
    {
        int newRow = grid->GetNumberRows();
        grid->AppendRows(1);
        grid->SetCellValue(newRow, 0, account.bank_);
        grid->SetCellValue(newRow, 1, account.name_);
        grid->SetCellValue(newRow, 2, wxString::Format("%#'.2f", account.balance()));
        grid->SetCellValue(newRow, 3, wxString::Format("%.2f", account.interest()));
        grid->SetCellValue(newRow, 4, account.type_);
    }
    grid->AutoSizeColumns(); // Automatically size columns to fit content
    grid->Refresh();

    // Update summary boxes
    summaryBoxes[0]->SetLabel("Total Balance: " + wxString::Format("%#'.2f", summary.totalBalance_));
    summaryBoxes[1]->SetLabel("Current Balance: " + wxString::Format("%#'.2f", summary.currentBalance_));
    summaryBoxes[2]->SetLabel("Savings Balance: " + wxString::Format("%#'.2f", summary.savingsBalance_));
    summaryBoxes[3]->SetLabel("Credit Balance: " + wxString::Format("%#'.2f", summary.creditBalance_));
    summaryBoxes[4]->SetLabel("ISA Balance: " + wxString::Format("%#'.2f", summary.isaBalance_));
    summaryBoxes[5]->SetLabel("GIA Balance: " + wxString::Format("%#'.2f", summary.giaBalance_));
    summaryBoxes[6]->SetLabel("Crypto Balance: " + wxString::Format("%#'.2f", summary.cryptoBalance_));
    summaryBoxes[7]->SetLabel("Total Interest: " + wxString::Format("%#'.2f", summary.totalInterest_));
}

// HOME: Event handlers
void HomeFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
    Close(true);
}

void HomeFrame::OnAbout(wxCommandEvent &WXUNUSED(event))
{
    wxMessageBox(wxString::Format(
                     "Welcome to the Personal Finance Tracker!\n"
                     "\n"
                     "This application helps you track your finances.\n"
                     "Running under %s.",
                     wxGetOsDescription()),
                 "About Personal Finance Tracker",
                 wxOK | wxICON_INFORMATION,
                 this);
}

void HomeFrame::OnAddAccount(wxCommandEvent &WXUNUSED(event))
{
    (new AccountAddFrame(this))->Show();
}

void HomeFrame::OnVisualise(wxCommandEvent &WXUNUSED(event))
{
    (new VisualiseFrame(this))->Show();
}

void HomeFrame::OnSaveSummary(wxCommandEvent &WXUNUSED(event))
{
    savedData.currentSummary_.SaveFinanceSummary();
    savedData.savedSummaryList_.push_back(savedData.currentSummary_);
}

void HomeFrame::OnGridCellChange(wxGridEvent &event)
{
    int row = event.GetRow();
    int col = event.GetCol();
    wxString newValue = grid->GetCellValue(row, col);

    // Ensure the row index is within the bounds of the account list
    if (row >= 0 && row < static_cast<int>(savedData.accountList_.size()))
    {
        Account &account = savedData.accountList_[row];

        switch (col)
        {
        case 0:
            account.bank_ = newValue.ToStdString();
            break;
        case 1:
            account.name_ = newValue.ToStdString();
            break;
        case 2:
        {
            double balance;
            if (newValue.ToDouble(&balance))
            {
                account.setBalance(balance);
            }
            break;
        }
        case 3:
        {
            double interest;
            if (newValue.ToDouble(&interest))
            {
                account.setInterest(interest);
            }
            break;
        }
        case 4:
            account.type_ = newValue.ToStdString();
            break;
        default:
            break;
        }

        // Update accounts csv
        savedData.UpdateAccountsInCSV(savedData.accountList_);
        savedData.currentSummary_ = FinanceSummary(savedData.accountList_);

        // Optionally, you can refresh the grid or the entire UI to reflect other dependent changes
        LoadData();
    }
}

// ACCOUNTADD: Frame constructor
AccountAddFrame::AccountAddFrame(wxWindow *parent)
    : wxFrame(parent, wxID_ANY, "New Account", wxDefaultPosition, wxSize(400, 500))
{
    wxPanel *panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    const int controlWidth = 200;
    const int controlHeight = -1;
    const int borderSize = 5;

    wxStaticText *bankLabel = new wxStaticText(panel, wxID_ANY, "Bank");
    bankCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(controlWidth, controlHeight));
    vbox->Add(bankLabel, 0, wxALL, borderSize);
    vbox->Add(bankCtrl, 0, wxALL | wxEXPAND, borderSize);

    wxStaticText *nameLabel = new wxStaticText(panel, wxID_ANY, "Name");
    nameCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(controlWidth, controlHeight));
    vbox->Add(nameLabel, 0, wxALL, borderSize);
    vbox->Add(nameCtrl, 0, wxALL | wxEXPAND, borderSize);

    wxStaticText *balanceLabel = new wxStaticText(panel, wxID_ANY, "Balance");
    balanceCtrl = new wxSpinCtrlDouble(panel, wxID_ANY, "", wxDefaultPosition, wxSize(controlWidth, controlHeight));
    balanceCtrl->SetRange(-10000, 10000);
    balanceCtrl->SetIncrement(0.01);
    vbox->Add(balanceLabel, 0, wxALL, borderSize);
    vbox->Add(balanceCtrl, 0, wxALL | wxEXPAND, borderSize);

    wxStaticText *interestLabel = new wxStaticText(panel, wxID_ANY, "Interest");
    interestCtrl = new wxSpinCtrlDouble(panel, wxID_ANY, "", wxDefaultPosition, wxSize(controlWidth, controlHeight));
    interestCtrl->SetRange(-10000, 10000);
    interestCtrl->SetIncrement(0.01);
    vbox->Add(interestLabel, 0, wxALL, borderSize);
    vbox->Add(interestCtrl, 0, wxALL | wxEXPAND, borderSize);

    wxStaticText *typeLabel = new wxStaticText(panel, wxID_ANY, "Type");
    typeCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(controlWidth, controlHeight));
    vbox->Add(typeLabel, 0, wxALL, borderSize);
    vbox->Add(typeCtrl, 0, wxALL | wxEXPAND, borderSize);

    wxButton *submitBtn = new wxButton(panel, wxID_ANY, "Submit");
    vbox->Add(submitBtn, 0, wxALL | wxALIGN_CENTER, borderSize);

    panel->SetSizer(vbox);
    vbox->SetSizeHints(this);

    Bind(wxEVT_BUTTON, &AccountAddFrame::OnSubmit, this, submitBtn->GetId());
}

// ACCOUNTADD: Event handlers
void AccountAddFrame::OnSubmit(wxCommandEvent &event)
{
    try
    {
        wxString name = nameCtrl->GetValue();
        wxString bank = bankCtrl->GetValue();
        wxString type = typeCtrl->GetValue();

        if (name.IsEmpty() || bank.IsEmpty() || type.IsEmpty())
        {
            throw std::invalid_argument("Fields are empty");
        }

        double balance = balanceCtrl->GetValue();
        double interest = interestCtrl->GetValue();

        // Process the data
        Account newAccount(name.ToStdString(), bank.ToStdString(), balance, interest, type.ToStdString());

        HomeFrame *parentFrame = dynamic_cast<HomeFrame *>(GetParent());
        if (parentFrame)
        {
            parentFrame->savedData.accountList_.push_back(newAccount);
            newAccount.AddAccountToCSV();
            parentFrame->LoadData();
        }
        // Close the frame after submission
        Close(true);
    }
    catch (const std::exception &e)
    {
        wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR, this);
    }
}

// VISUALISE: Frame constructor
VisualiseFrame::VisualiseFrame(wxWindow *parent)
    : wxFrame(parent, wxID_ANY, "Visualise Financial Summaries", wxDefaultPosition, wxSize(800, 600))
{
    CreatePlot();
}

void VisualiseFrame::CreatePlot()
{
    // Create a new mpWindow
    mpWindow *plotWindow = new mpWindow(this, wxID_ANY, wxDefaultPosition, wxSize(800, 600), wxSUNKEN_BORDER);

    // Create and add layer for the X and Y axes
    mpScaleX *xAxis = new mpScaleX(wxT("Date"), mpALIGN_BORDER_BOTTOM, true);
    mpScaleY *yAxis = new mpScaleY(wxT("Balance (£)"), mpALIGN_LEFT, true);
    xAxis->SetTicks(false);
    yAxis->SetTicks(false);
    plotWindow->AddLayer(xAxis);
    plotWindow->AddLayer(yAxis);

    // Define a custom line layer class
    class LineLayer : public mpFXYVector
    {
    public:
        LineLayer(const wxString &name, const std::vector<double> &xData, const std::vector<double> &yData, const wxColour &colour = *wxBLUE)
            : mpFXYVector(name)
        {
            SetData(xData, yData);
            SetContinuity(true);
            SetPen(wxPen(colour, 2, wxSOLID));
            SetDrawOutsideMargins(false);
        }
    };

    // Get the parent frame
    HomeFrame *parentFrame = dynamic_cast<HomeFrame *>(GetParent());

    // Retrieve the finance summary points from the parent frame
    auto financeSummaryPoints = parentFrame->savedData.getFinanceSummaryPoints();

    // Define line colors and names
    const std::vector<wxColour> lineColors = {
        wxColor(255, 0, 0),   // Red
        wxColor(0, 255, 0),   // Green
        wxColor(0, 0, 255),   // Blue
        wxColor(255, 255, 0), // Yellow
        wxColor(255, 0, 255), // Magenta
        wxColor(0, 255, 255), // Cyan
        wxColor(128, 0, 0),   // Maroon
        wxColor(0, 128, 0)    // Dark Green
    };

    const std::vector<wxString> lineNames = {
        wxT("Total"),
        wxT("Current"),
        wxT("Savings"),
        wxT("Credit"),
        wxT("ISA"),
        wxT("GIA"),
        wxT("Crypto"),
        wxT("Interest")};

    // Create a legend
    wxRect legendRect(100, 100, 200, 100);
    mpInfoLegend *legend = new mpInfoLegend(legendRect);
    plotWindow->AddLayer(legend);

    // Iterate over each finance summary point
    for (size_t i = 0; i < financeSummaryPoints.size(); ++i)
    {
        const auto &point = financeSummaryPoints[i];

        // Extract the labels (dates) and values from the tuple
        const std::vector<double> &labels = std::get<0>(point);
        const std::vector<double> &values = std::get<1>(point);

        // Create a new line layer for each finance summary point with the corresponding color and name
        wxString lineName = (i < lineNames.size()) ? lineNames[i] : wxString::Format(wxT("Line %d"), i + 1);
        wxColour lineColor = (i < lineColors.size()) ? lineColors[i] : *wxBLACK;
        LineLayer *lineLayer = new LineLayer(lineName, labels, values, lineColor);
        plotWindow->AddLayer(lineLayer);
    }

    // Disable mouse pan and zoom
    plotWindow->EnableMousePanZoom(false);

    // Find the minimum and maximum Y values
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();


    for (const auto &point : financeSummaryPoints)
    {
        const std::vector<double> &labels = std::get<0>(point);
        std::cout << labels[1];
        const std::vector<double> &values = std::get<1>(point);
        for (double value : values)
        {
            minY = std::min(minY, value);
            maxY = std::max(maxY, value);
        }
        for (double label : labels)
        {
            minX = std::min(minX, label);
            maxX = std::max(maxX, label);
        }
    }

    plotWindow->Fit(minX-1, maxX+1, minY - 1000, maxY + 1000);

    // Enable auto-scaling for the Y-axis based on the largest value plotted
    yAxis->SetLabelFormat(wxT("£%.2f"));
}
