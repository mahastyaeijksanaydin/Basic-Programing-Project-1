#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/choice.h>
#include "rental_system.h"
#include "rental.h"
#include "borrower.h"
#include "umbrella.h"
#include "umbrella_status.h"

using namespace std;

static wxString W(const string& s) { return wxString::FromUTF8(s); }
static string S(const wxString& s) { return string(s.utf8_string()); }

class MainFrame : public wxFrame
{
public:
    MainFrame() : wxFrame(nullptr, wxID_ANY, "Umbrella Rental", wxDefaultPosition, wxSize(980, 620))
    {
        auto* root = new wxPanel(this);
        auto* vbox = new wxBoxSizer(wxVERTICAL);

        notebook = new wxNotebook(root, wxID_ANY);

        BuildStockTab();
        BuildManageTab();
        BuildRentalsTab();

        vbox->Add(notebook, 1, wxEXPAND | wxALL, 8);
        root->SetSizer(vbox);

        string msg;
        sys.AddUmbrella("P1", msg);
        sys.AddUmbrella("P2", msg);

        RefreshAll();
        Centre();
    }

private:
    RentalSystem sys;

    wxNotebook* notebook{};

    wxListCtrl* stockList{};
    wxListCtrl* availableList{};
    wxButton* btnRefreshStock{};

    wxTextCtrl* txtAddId{};
    wxButton* btnAdd{};

    wxTextCtrl* txtDelId{};
    wxButton* btnDel{};

    wxTextCtrl* txtOldId{};
    wxTextCtrl* txtNewId{};
    wxButton* btnRename{};

    wxTextCtrl* txtResetId{};
    wxChoice* choiceStatus{};
    wxButton* btnReset{};

    wxTextCtrl* txtFirst{};
    wxTextCtrl* txtLast{};
    wxTextCtrl* txtNiu{};
    wxChoice* choiceUmbrella{};
    wxTextCtrl* txtStartDate{};
    wxSpinCtrl* spinStartHour{};
    wxSpinCtrl* spinDuration{};
    wxButton* btnRegister{};

    wxListCtrl* rentalsList{};
    wxButton* btnReturn{};

private:
    void Info(const wxString& title, const wxString& msg)
    {
        wxMessageBox(msg, title, wxOK | wxICON_INFORMATION, this);
    }

    void Error(const wxString& title, const wxString& msg)
    {
        wxMessageBox(msg, title, wxOK | wxICON_ERROR, this);
    }

    void BuildStockTab()
    {
        auto* panel = new wxPanel(notebook);
        auto* hbox = new wxBoxSizer(wxHORIZONTAL);

        auto* left = new wxBoxSizer(wxVERTICAL);
        left->Add(new wxStaticText(panel, wxID_ANY, "Stock Payung (Semua)"), 0, wxBOTTOM, 6);

        stockList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
        stockList->InsertColumn(0, "ID", wxLIST_FORMAT_LEFT, 200);
        stockList->InsertColumn(1, "Status", wxLIST_FORMAT_LEFT, 150);

        btnRefreshStock = new wxButton(panel, wxID_ANY, "Refresh");
        btnRefreshStock->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { RefreshAll(); });

        left->Add(stockList, 1, wxEXPAND | wxBOTTOM, 8);
        left->Add(btnRefreshStock, 0, wxALIGN_LEFT);

        auto* right = new wxBoxSizer(wxVERTICAL);
        right->Add(new wxStaticText(panel, wxID_ANY, "Payung AVAILABLE"), 0, wxBOTTOM, 6);

        availableList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
        availableList->InsertColumn(0, "ID", wxLIST_FORMAT_LEFT, 200);

        right->Add(availableList, 1, wxEXPAND);

        hbox->Add(left, 1, wxEXPAND | wxRIGHT, 10);
        hbox->Add(right, 1, wxEXPAND);

        panel->SetSizer(hbox);
        notebook->AddPage(panel, "Stock", true);
    }

    void BuildManageTab()
    {
        auto* panel = new wxPanel(notebook);
        auto* vbox = new wxBoxSizer(wxVERTICAL);

        // Add umbrella
        {
            auto* box = new wxStaticBoxSizer(wxVERTICAL, panel, "Tambah Payung");
            auto* row = new wxBoxSizer(wxHORIZONTAL);

            row->Add(new wxStaticText(panel, wxID_ANY, "ID:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
            txtAddId = new wxTextCtrl(panel, wxID_ANY);
            btnAdd = new wxButton(panel, wxID_ANY, "Tambah");

            btnAdd->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
                string msg;
                if (sys.AddUmbrella(S(txtAddId->GetValue()), msg)) {
                    Info("Berhasil", W(msg));
                    txtAddId->Clear();
                    RefreshAll();
                }
                else {
                    Error("Gagal", W(msg));
                }
                });

            row->Add(txtAddId, 1, wxRIGHT, 8);
            row->Add(btnAdd, 0);

            box->Add(row, 0, wxEXPAND | wxALL, 8);
            vbox->Add(box, 0, wxEXPAND | wxBOTTOM, 10);
        }

        // Delete umbrella
        {
            auto* box = new wxStaticBoxSizer(wxVERTICAL, panel, "Hapus Payung (harus AVAILABLE)");
            auto* row = new wxBoxSizer(wxHORIZONTAL);

            row->Add(new wxStaticText(panel, wxID_ANY, "ID:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
            txtDelId = new wxTextCtrl(panel, wxID_ANY);
            btnDel = new wxButton(panel, wxID_ANY, "Hapus");

            btnDel->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
                string msg;
                if (sys.DeleteUmbrella(S(txtDelId->GetValue()), msg)) {
                    Info("Berhasil", W(msg));
                    txtDelId->Clear();
                    RefreshAll();
                }
                else {
                    Error("Gagal", W(msg));
                }
                });

            row->Add(txtDelId, 1, wxRIGHT, 8);
            row->Add(btnDel, 0);

            box->Add(row, 0, wxEXPAND | wxALL, 8);
            vbox->Add(box, 0, wxEXPAND | wxBOTTOM, 10);
        }

        // Rename
        {
            auto* box = new wxStaticBoxSizer(wxVERTICAL, panel, "Rename ID Payung");
            auto* row = new wxBoxSizer(wxHORIZONTAL);

            row->Add(new wxStaticText(panel, wxID_ANY, "ID lama:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
            txtOldId = new wxTextCtrl(panel, wxID_ANY);
            row->Add(txtOldId, 1, wxRIGHT, 10);

            row->Add(new wxStaticText(panel, wxID_ANY, "ID baru:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
            txtNewId = new wxTextCtrl(panel, wxID_ANY);
            row->Add(txtNewId, 1, wxRIGHT, 10);

            btnRename = new wxButton(panel, wxID_ANY, "Rename");
            btnRename->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
                string msg;
                if (sys.RenameUmbrella(S(txtOldId->GetValue()), S(txtNewId->GetValue()), msg)) {
                    Info("Berhasil", W(msg));
                    txtOldId->Clear();
                    txtNewId->Clear();
                    RefreshAll();
                }
                else {
                    Error("Gagal", W(msg));
                }
                });
            row->Add(btnRename, 0);

            box->Add(row, 0, wxEXPAND | wxALL, 8);
            vbox->Add(box, 0, wxEXPAND | wxBOTTOM, 10);
        }

        // Reset status
        {
            auto* box = new wxStaticBoxSizer(wxVERTICAL, panel, "Reset Status Payung");
            auto* row = new wxBoxSizer(wxHORIZONTAL);

            row->Add(new wxStaticText(panel, wxID_ANY, "ID:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
            txtResetId = new wxTextCtrl(panel, wxID_ANY);
            row->Add(txtResetId, 1, wxRIGHT, 10);

            row->Add(new wxStaticText(panel, wxID_ANY, "Status:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
            choiceStatus = new wxChoice(panel, wxID_ANY);
            choiceStatus->Append("AVAILABLE");
            choiceStatus->Append("BORROWED");
            choiceStatus->SetSelection(0);
            row->Add(choiceStatus, 0, wxRIGHT, 10);

            btnReset = new wxButton(panel, wxID_ANY, "Reset");
            btnReset->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
                UmbrellaStatus st = (choiceStatus->GetSelection() == 0) ? UmbrellaStatus::AVAILABLE : UmbrellaStatus::BORROWED;
                string msg;
                if (sys.ResetUmbrellaStatus(S(txtResetId->GetValue()), st, msg)) {
                    Info("Berhasil", W(msg));
                    txtResetId->Clear();
                    RefreshAll();
                }
                else {
                    Error("Gagal", W(msg));
                }
                });
            row->Add(btnReset, 0);

            box->Add(row, 0, wxEXPAND | wxALL, 8);
            vbox->Add(box, 0, wxEXPAND);
        }

        panel->SetSizer(vbox);
        notebook->AddPage(panel, "Umbrella", false);
    }

    void BuildRentalsTab()
    {
        auto* panel = new wxPanel(notebook);
        auto* vbox = new wxBoxSizer(wxVERTICAL);

        // Register rental form
        {
            auto* box = new wxStaticBoxSizer(wxVERTICAL, panel, "Registrasi Peminjaman");
            auto* grid = new wxFlexGridSizer(0, 2, 8, 10);
            grid->AddGrowableCol(1, 1);

            auto addRow = [&](const wxString& label, wxWindow* ctrl) {
                grid->Add(new wxStaticText(panel, wxID_ANY, label), 0, wxALIGN_CENTER_VERTICAL);
                grid->Add(ctrl, 1, wxEXPAND);
                };

            txtFirst = new wxTextCtrl(panel, wxID_ANY);
            txtLast = new wxTextCtrl(panel, wxID_ANY);
            txtNiu = new wxTextCtrl(panel, wxID_ANY);

            choiceUmbrella = new wxChoice(panel, wxID_ANY);

            txtStartDate = new wxTextCtrl(panel, wxID_ANY, "YYYY-MM-DD");
            spinStartHour = new wxSpinCtrl(panel, wxID_ANY);
            spinStartHour->SetRange(0, 23);
            spinStartHour->SetValue(8);

            spinDuration = new wxSpinCtrl(panel, wxID_ANY);
            spinDuration->SetRange(1, 24);
            spinDuration->SetValue(2);

            addRow("Nama depan:", txtFirst);
            addRow("Nama belakang:", txtLast);
            addRow("NIU:", txtNiu);
            addRow("Payung AVAILABLE:", choiceUmbrella);
            addRow("Tanggal pinjam:", txtStartDate);
            addRow("Jam mulai (0-23):", spinStartHour);
            addRow("Durasi (jam):", spinDuration);

            btnRegister = new wxButton(panel, wxID_ANY, "Register");
            btnRegister->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
                Borrower b;
                b.firstName = S(txtFirst->GetValue());
                b.lastName = S(txtLast->GetValue());
                b.niu = S(txtNiu->GetValue());

                if (choiceUmbrella->GetSelection() == wxNOT_FOUND) {
                    Error("Gagal", "Tidak ada payung AVAILABLE yang dipilih.");
                    return;
                }

                string umbrellaId = S(choiceUmbrella->GetStringSelection());
                string startDate = S(txtStartDate->GetValue());
                int startHour = spinStartHour->GetValue();
                int duration = spinDuration->GetValue();

                string msg;
                if (sys.RegisterRental(b, umbrellaId, startDate, startHour, duration, msg)) {
                    Info("Sukses", W(msg));
                    RefreshAll();
                }
                else {
                    Error("Gagal", W(msg));
                }
                });

            box->Add(grid, 0, wxEXPAND | wxALL, 10);
            box->Add(btnRegister, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);
            vbox->Add(box, 0, wxEXPAND | wxBOTTOM, 10);
        }

        // Active rentals list and return
        {
            auto* box = new wxStaticBoxSizer(wxVERTICAL, panel, "Registrasi Aktif");

            rentalsList = new wxListCtrl(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL);
            rentalsList->InsertColumn(0, "Rental ID", wxLIST_FORMAT_LEFT, 90);
            rentalsList->InsertColumn(1, "Umbrella ID", wxLIST_FORMAT_LEFT, 110);
            rentalsList->InsertColumn(2, "Nama", wxLIST_FORMAT_LEFT, 200);
            rentalsList->InsertColumn(3, "NIU", wxLIST_FORMAT_LEFT, 140);
            rentalsList->InsertColumn(4, "Tanggal", wxLIST_FORMAT_LEFT, 120);
            rentalsList->InsertColumn(5, "Jam", wxLIST_FORMAT_LEFT, 140);
            rentalsList->InsertColumn(6, "Durasi", wxLIST_FORMAT_LEFT, 80);

            btnReturn = new wxButton(panel, wxID_ANY, "Return (pilih baris)");
            btnReturn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
                long sel = rentalsList->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                if (sel == -1) {
                    Error("Gagal", "Pilih salah satu registrasi terlebih dahulu");
                    return;
                }

                long rid = -1;
                rentalsList->GetItemText(sel, 0).ToLong(&rid);

                string msg;
                if (sys.ReturnUmbrella((int)rid, msg)) {
                    Info("Berhasil", W(msg));
                    RefreshAll();
                }
                else {
                    Error("Gagal", W(msg));
                }
                });

            box->Add(rentalsList, 1, wxEXPAND | wxALL, 10);
            box->Add(btnReturn, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);

            vbox->Add(box, 1, wxEXPAND);
        }

        panel->SetSizer(vbox);
        notebook->AddPage(panel, "Rentals", false);
    }

    void RefreshAll()
    {
        RefreshStock();
        RefreshAvailable();
        RefreshAvailableChoice();
        RefreshRentals();
    }

    void RefreshStock()
    {
        stockList->DeleteAllItems();
        const auto& us = sys.GetUmbrellas();
        for (long i = 0; i < (long)us.size(); ++i) {
            stockList->InsertItem(i, W(us[i].id));
            stockList->SetItem(i, 1, W(statusToString(us[i].status)));
        }
    }

    void RefreshAvailable()
    {
        availableList->DeleteAllItems();
        auto avail = sys.GetAvailableUmbrellaIds();
        for (long i = 0; i < (long)avail.size(); ++i) {
            availableList->InsertItem(i, W(avail[i]));
        }
    }

    void RefreshAvailableChoice()
    {
        auto cur = choiceUmbrella->GetStringSelection();

        choiceUmbrella->Clear();
        auto avail = sys.GetAvailableUmbrellaIds();
        for (auto& id : avail) choiceUmbrella->Append(W(id));

        if (!cur.empty()) {
            int idx = choiceUmbrella->FindString(cur);
            if (idx != wxNOT_FOUND) choiceUmbrella->SetSelection(idx);
        }
        if (choiceUmbrella->GetCount() > 0 && choiceUmbrella->GetSelection() == wxNOT_FOUND)
            choiceUmbrella->SetSelection(0);
    }

    void RefreshRentals()
    {
        rentalsList->DeleteAllItems();
        const auto& rs = sys.GetRentals();
        for (long i = 0; i < (long)rs.size(); ++i) {
            const auto& r = rs[i];

            rentalsList->InsertItem(i, wxString::Format("%d", r.rentalId));
            rentalsList->SetItem(i, 1, W(r.umbrellaId));
            rentalsList->SetItem(i, 2, W(r.borrower.firstName + " " + r.borrower.lastName));
            rentalsList->SetItem(i, 3, W(r.borrower.niu));
            rentalsList->SetItem(i, 4, W(r.startDate));
            rentalsList->SetItem(i, 5, wxString::Format("%02d:00-%02d:00", r.startHour, r.endHour));
            rentalsList->SetItem(i, 6, wxString::Format("%d", r.durationHours));
        }
    }
};

class MyApp : public wxApp
{
public:
    bool OnInit() override
    {
        auto* f = new MainFrame();
        f->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
