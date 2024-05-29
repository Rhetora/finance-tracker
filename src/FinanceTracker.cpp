#include "../include/Account.h"
#include "wx/wx.h"
#include <wx/spinctrl.h>
#include "wx/grid.h"
#include <locale.h>

// Application class
class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
};

// Frame class for the main application window
class InfoFrame : public wxFrame
{
public:
    std::vector<Account> accountList = {};
    FinanceSummary summary = FinanceSummary(accountList);

    InfoFrame(const wxString &title);

    void OnQuit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnAddAccount(wxCommandEvent &event);
    void OnSaveSummary(wxCommandEvent &event);

    void LoadAccounts();
    void LoadSummary();

private:
    wxGrid *grid;
    void InitializeGrid();

    wxStaticText *summaryBoxes[8]; // Array to hold 8 value boxes
    wxButton *actionButton;        // Button below the grid

    wxDECLARE_EVENT_TABLE();
};

// Frame class for adding a new account
class AccountAddFrame : public wxFrame
{
public:
    AccountAddFrame(wxWindow *parent);

private:
    void OnSubmit(wxCommandEvent &event);
    wxTextCtrl *nameCtrl;
    wxTextCtrl *bankCtrl;
    wxSpinCtrlDouble *balanceCtrl;
    wxSpinCtrlDouble *interestCtrl;
    wxTextCtrl *typeCtrl;

    wxDECLARE_EVENT_TABLE();
};

// Event table for the main frame
enum
{
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT,
    Add_Account = 1,
    Submit_Account = 2,
    Save_Summary = 3
};

wxBEGIN_EVENT_TABLE(InfoFrame, wxFrame)
    EVT_MENU(Minimal_Quit, InfoFrame::OnQuit)
        EVT_MENU(Minimal_About, InfoFrame::OnAbout)
            EVT_MENU(Add_Account, InfoFrame::OnAddAccount)
                EVT_BUTTON(Save_Summary, InfoFrame::OnSaveSummary)
                    wxEND_EVENT_TABLE()

                        wxBEGIN_EVENT_TABLE(AccountAddFrame, wxFrame)
                            EVT_BUTTON(Submit_Account, AccountAddFrame::OnSubmit)
                                wxEND_EVENT_TABLE()

                                    wxIMPLEMENT_APP(MyApp);

// Application initialization
bool MyApp::OnInit()
{
    setlocale(LC_NUMERIC, "");
    if (!wxApp::OnInit())
        return false;

    InfoFrame *frame = new InfoFrame("Personal Finance Tracker");
    frame->Show(true);

    return true;
}

// Frame constructor
InfoFrame::InfoFrame(const wxString &title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(800, 600)) // Set the size here
{
#if wxUSE_MENUBAR
    wxMenu *fileMenu = new wxMenu;
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Minimal_About, "&About\tF1", "Show about dialog");

    fileMenu->Append(Add_Account, "Add Account", "Add a financial account");
    fileMenu->Append(Minimal_Quit, "E&xit\tAlt-X", "Quit this program");

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    SetMenuBar(menuBar);
#else
    wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton *aboutBtn = new wxButton(this, wxID_ANY, "About...");
    aboutBtn->Bind(wxEVT_BUTTON, &InfoFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
    SetSizer(sizer);
#endif

#if wxUSE_STATUSBAR
    CreateStatusBar(1);
    SetStatusText("Welcome to Personal Finance Tracker!");
#endif

    accountList = LoadAccountsFromCSV();
    summary = FinanceSummary(accountList);
    // Create and initialize the grid
    grid = new wxGrid(this, wxID_ANY);
    InitializeGrid();

    // Create a box sizer for the left side with 8 boxes
    wxBoxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);

    summaryBoxes[0] = new wxStaticText(this, wxID_ANY, "Total Balance: " + wxString::Format("%#'.2f", summary.totalBalance_));
    summaryBoxes[1] = new wxStaticText(this, wxID_ANY, "Current Balance: " + wxString::Format("%#'.2f", summary.currentBalance_));
    summaryBoxes[2] = new wxStaticText(this, wxID_ANY, "Savings Balance: " + wxString::Format("%#'.2f", summary.savingsBalance_));
    summaryBoxes[3] = new wxStaticText(this, wxID_ANY, "Credit Balance: " + wxString::Format("%#'.2f", summary.creditBalance_));
    summaryBoxes[4] = new wxStaticText(this, wxID_ANY, "ISA Balance: " + wxString::Format("%#'.2f", summary.isaBalance_));
    summaryBoxes[5] = new wxStaticText(this, wxID_ANY, "GIA Balance: " + wxString::Format("%#'.2f", summary.giaBalance_));
    summaryBoxes[6] = new wxStaticText(this, wxID_ANY, "Crypto Balance: " + wxString::Format("%#'.2f", summary.cryptoBalance_));
    summaryBoxes[7] = new wxStaticText(this, wxID_ANY, "Total Interest: " + wxString::Format("%#'.2f", summary.totalInterest_));
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
    actionButton = new wxButton(this, Save_Summary, "Save Summary");
    bottomSizer->Add(actionButton, 0, wxALIGN_LEFT | wxALL, 5);

    // Create a vertical sizer to combine the main sizer and the bottom button
    wxBoxSizer *verticalSizer = new wxBoxSizer(wxVERTICAL);
    verticalSizer->Add(mainSizer, 1, wxEXPAND | wxALL, 5);
    verticalSizer->Add(bottomSizer, 0, wxALIGN_LEFT | wxALL, 5);

    // Set the sizer for the frame
    SetSizerAndFit(verticalSizer);
    LoadAccounts();
    LoadSummary();
}

// Initialize the grid with columns for account details
void InfoFrame::InitializeGrid()
{
    grid->CreateGrid(0, 5); // Initially 0 rows, 5 columns
    grid->SetColLabelValue(0, "Name");
    grid->SetColLabelValue(1, "Bank");
    grid->SetColLabelValue(2, "Balance");
    grid->SetColLabelValue(3, "Interest");
    grid->SetColLabelValue(4, "Type");
    grid->AutoSizeColumns(); // Automatically size columns to fit content
}

// Load existing accounts into the grid
void InfoFrame::LoadAccounts()
{
    if (grid->GetNumberRows() > 0)
        grid->DeleteRows(0, grid->GetNumberRows());
    for (const Account &account : accountList)
    {
        int newRow = grid->GetNumberRows();
        grid->AppendRows(1);
        grid->SetCellValue(newRow, 0, account.name_);
        grid->SetCellValue(newRow, 1, account.bank_);
        grid->SetCellValue(newRow, 2, wxString::Format("%#'.2f", account.balance()));
        grid->SetCellValue(newRow, 3, wxString::Format("%.2f", account.interest()));
        grid->SetCellValue(newRow, 4, account.type_);
    }
    grid->AutoSizeColumns(); // Automatically size columns to fit content
    grid->Refresh();
}

// Function to load and update the summary values
void InfoFrame::LoadSummary()
{
    summary = FinanceSummary(accountList);
    summaryBoxes[0]->SetLabel("Total Balance: " + wxString::Format("%#'.2f", summary.totalBalance_));
    summaryBoxes[1]->SetLabel("Current Balance: " + wxString::Format("%#'.2f", summary.currentBalance_));
    summaryBoxes[2]->SetLabel("Savings Balance: " + wxString::Format("%#'.2f", summary.savingsBalance_));
    summaryBoxes[3]->SetLabel("Credit Balance: " + wxString::Format("%#'.2f", summary.creditBalance_));
    summaryBoxes[4]->SetLabel("ISA Balance: " + wxString::Format("%#'.2f", summary.isaBalance_));
    summaryBoxes[5]->SetLabel("GIA Balance: " + wxString::Format("%#'.2f", summary.giaBalance_));
    summaryBoxes[6]->SetLabel("Crypto Balance: " + wxString::Format("%#'.2f", summary.cryptoBalance_));
    summaryBoxes[7]->SetLabel("Total Interest: " + wxString::Format("%#'.2f", summary.totalInterest_));
}

// Event handler for quitting the application
void InfoFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
    Close(true);
}

// Event handler for showing the "About" dialog
void InfoFrame::OnAbout(wxCommandEvent &WXUNUSED(event))
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

// Event handler for adding a new account
void InfoFrame::OnAddAccount(wxCommandEvent &WXUNUSED(event))
{
    (new AccountAddFrame(this))->Show();
}
void InfoFrame::OnSaveSummary(wxCommandEvent &WXUNUSED(event))
{
    summary.SaveFinanceSummary();
}

// Frame constructor
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
    balanceCtrl->SetIncrement(0.1);
    vbox->Add(balanceLabel, 0, wxALL, borderSize);
    vbox->Add(balanceCtrl, 0, wxALL | wxEXPAND, borderSize);

    wxStaticText *interestLabel = new wxStaticText(panel, wxID_ANY, "Interest");
    interestCtrl = new wxSpinCtrlDouble(panel, wxID_ANY, "", wxDefaultPosition, wxSize(controlWidth, controlHeight));
    interestCtrl->SetRange(-10000, 10000);
    interestCtrl->SetIncrement(0.1);
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

        InfoFrame *parentFrame = dynamic_cast<InfoFrame *>(GetParent());
        if (parentFrame)
        {
            parentFrame->accountList.push_back(newAccount);
            newAccount.AddAccountToCSV();
            parentFrame->LoadAccounts();
            parentFrame->LoadSummary();
        }
        // Close the frame after submission
        Close(true);
    }
    catch (const std::exception &e)
    {
        wxMessageBox(e.what(), "Error", wxOK | wxICON_ERROR, this);
    }
}
