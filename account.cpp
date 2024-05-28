#include "account.h"
#include "wx/wx.h"
#include "wx/grid.h"


// Application class
class MyApp : public wxApp
{
public:
    virtual bool OnInit() wxOVERRIDE;
};

// Frame class for the main application window
class MyFrame : public wxFrame
{
public:
    std::vector<Account> accountList = {};

    MyFrame(const wxString &title);

    void OnQuit(wxCommandEvent &event);
    void OnAbout(wxCommandEvent &event);
    void OnAddAccount(wxCommandEvent &event);

    void LoadAccounts();

private:
    wxGrid *grid;
    void InitializeGrid();

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
    wxTextCtrl *balanceCtrl;
    wxTextCtrl *interestCtrl;
    wxTextCtrl *typeCtrl;

    wxDECLARE_EVENT_TABLE();
};

enum
{
    Minimal_Quit = wxID_EXIT,
    Minimal_About = wxID_ABOUT,
    Add_Account = 1,
    Submit_Account = 2
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(Minimal_Quit, MyFrame::OnQuit)
        EVT_MENU(Minimal_About, MyFrame::OnAbout)
            EVT_MENU(Add_Account, MyFrame::OnAddAccount)
                wxEND_EVENT_TABLE()

                    wxBEGIN_EVENT_TABLE(AccountAddFrame, wxFrame)
                        EVT_BUTTON(Submit_Account, AccountAddFrame::OnSubmit)
                            wxEND_EVENT_TABLE()

                                wxIMPLEMENT_APP(MyApp);

// Application initialization
bool MyApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    MyFrame *frame = new MyFrame("Personal Finance Tracker");
    frame->Show(true);

    return true;
}

// Frame constructor
MyFrame::MyFrame(const wxString &title)
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
    aboutBtn->Bind(wxEVT_BUTTON, &MyFrame::OnAbout, this);
    sizer->Add(aboutBtn, wxSizerFlags().Center());
    SetSizer(sizer);
#endif

#if wxUSE_STATUSBAR
    CreateStatusBar(1);
    SetStatusText("Welcome to Personal Finance Tracker!");
#endif

    accountList = LoadAccountsFromCSV();
    // Create and initialize the grid
    grid = new wxGrid(this, wxID_ANY);
    InitializeGrid();

    // Set the sizer
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(grid, 1, wxEXPAND | wxALL, 5);

    // Load accounts after the grid has been initialized
    LoadAccounts();
    SetSizerAndFit(sizer);
}

// Initialize the grid with columns for account details
void MyFrame::InitializeGrid()
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
void MyFrame::LoadAccounts()
{
    if (grid->GetNumberRows() > 0)
        grid->DeleteRows(0, grid->GetNumberRows());
    for (const Account &account : accountList)
    {
        int newRow = grid->GetNumberRows();
        grid->AppendRows(1);
        grid->SetCellValue(newRow, 0, account.name);
        grid->SetCellValue(newRow, 1, account.bank);
        grid->SetCellValue(newRow, 2, wxString::Format("%.2f", account.getBalance()));
        grid->SetCellValue(newRow, 3, wxString::Format("%.2f", account.getInterest()));
        grid->SetCellValue(newRow, 4, account.type);
    }
    grid->AutoSizeColumns(); // Automatically size columns to fit content
    grid->Refresh();
}

// Event handler for quitting the application
void MyFrame::OnQuit(wxCommandEvent &WXUNUSED(event))
{
    Close(true);
}

// Event handler for showing the "About" dialog
void MyFrame::OnAbout(wxCommandEvent &WXUNUSED(event))
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
void MyFrame::OnAddAccount(wxCommandEvent &WXUNUSED(event))
{
    (new AccountAddFrame(this))->Show();
}

// Constructor for the AccountAddFrame
AccountAddFrame::AccountAddFrame(wxWindow *parent)
    : wxFrame(parent, wxID_ANY, "New Account", wxDefaultPosition, wxSize(400, 500)) // Adjusted size here
{
    wxPanel *panel = new wxPanel(this, wxID_ANY);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);

    // Creating input fields for account details
    nameCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    bankCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    balanceCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    interestCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));
    typeCtrl = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(200, -1));

    vbox->Add(new wxStaticText(panel, wxID_ANY, "Name"), 0, wxALL, 5);
    vbox->Add(nameCtrl, 0, wxALL | wxEXPAND, 5);
    vbox->Add(new wxStaticText(panel, wxID_ANY, "Bank"), 0, wxALL, 5);
    vbox->Add(bankCtrl, 0, wxALL | wxEXPAND, 5);
    vbox->Add(new wxStaticText(panel, wxID_ANY, "Balance"), 0, wxALL, 5);
    vbox->Add(balanceCtrl, 0, wxALL | wxEXPAND, 5);
    vbox->Add(new wxStaticText(panel, wxID_ANY, "Interest"), 0, wxALL, 5);
    vbox->Add(interestCtrl, 0, wxALL | wxEXPAND, 5);
    vbox->Add(new wxStaticText(panel, wxID_ANY, "Type"), 0, wxALL, 5);
    vbox->Add(typeCtrl, 0, wxALL | wxEXPAND, 5);

    // Submit button
    wxButton *submitBtn = new wxButton(panel, Submit_Account, "Submit");
    vbox->Add(submitBtn, 0, wxALL | wxALIGN_CENTER, 10);

    panel->SetSizer(vbox);
    vbox->SetSizeHints(this);
    // Adjust the frame size to ensure all controls are visible
}

// Event handler for the submit button
void AccountAddFrame::OnSubmit(wxCommandEvent &WXUNUSED(event))
{
    // Handle form submission, add validation and data processing here
    wxString name = nameCtrl->GetValue();
    wxString bank = bankCtrl->GetValue();
    double balance, interest;
    balanceCtrl->GetValue().ToDouble(&balance);
    interestCtrl->GetValue().ToDouble(&interest);
    wxString type = typeCtrl->GetValue();

    Account newAccount(name.ToStdString(), bank.ToStdString(), balance, interest, type.ToStdString());

    MyFrame *parentFrame = dynamic_cast<MyFrame *>(GetParent());
    if (parentFrame)
    {
        parentFrame->accountList.push_back(newAccount);
        newAccount.AddAccountToCSV();
        FinanceSummary summary(parentFrame->accountList);
        summary.SaveFinanceSummary();
        parentFrame->LoadAccounts();
    }

    // Close the frame after submission
    Close(true);
}
