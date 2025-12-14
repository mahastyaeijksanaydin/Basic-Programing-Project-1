#pragma once
#include <string>
#include <vector>
#include "rental.h"
#include "borrower.h"
#include "umbrella.h"
#include "umbrella_status.h"
using namespace std; 

class RentalSystem
{
private:
    vector<Umbrella> umbrellas;
    vector<Rental> rentals;
    int nextRentalId = 1;

    int findUmbrellaIdx(const string& id) const
    {
        for (int i = 0; i < (int)umbrellas.size(); ++i)
            if (umbrellas[i].id == id) return i;
        return -1;
    }

    int findRentalIdx(int rentalId) const
    {
        for (int i = 0; i < (int)rentals.size(); ++i)
            if (rentals[i].rentalId == rentalId) return i;
        return -1;
    }

    bool validateTime(int startHour, int durationHours) const
    {
        if (startHour < 0 || startHour > 23) return false;
        if (durationHours <= 0) return false;
        if (startHour + durationHours > 24) return false;
        return true;
    }

public:
    const vector<Umbrella>& GetUmbrellas() const { return umbrellas; }
    const vector<Rental>& GetRentals() const { return rentals; }

    bool AddUmbrella(const string& id, string& msg)
    {
        if (id.empty()) { msg = "ID payung tidak boleh kosong"; return false; }
        if (findUmbrellaIdx(id) != -1) { msg = "ID payung sudah ada"; return false; }
        umbrellas.emplace_back(id);
        msg = "Payung " + id + " ditambahkan";
        return true;
    }

    bool DeleteUmbrella(const string& id, string& msg)
    {
        int idx = findUmbrellaIdx(id);
        if (idx == -1) { msg = "Payung tidak ditemukan"; return false; }
        if (umbrellas[idx].status == UmbrellaStatus::BORROWED) { msg = "Payung sedang dipinjam"; return false; }
        umbrellas.erase(umbrellas.begin() + idx);
        msg = "Payung " + id + " dihapus";
        return true;
    }

    bool RenameUmbrella(const string& oldId, const string& newId, string& msg)
    {
        if (newId.empty()) { msg = "ID baru tidak boleh kosong"; return false; }
        if (findUmbrellaIdx(newId) != -1) { msg = "ID sudah dipakai"; return false; }

        int idx = findUmbrellaIdx(oldId);
        if (idx == -1) { msg = "ID tidak ditemukan"; return false; }

        umbrellas[idx].id = newId;
        for (auto& r : rentals)
            if (r.umbrellaId == oldId) r.umbrellaId = newId;

        msg = "Payung " + oldId + " diganti menjadi " + newId + ".";
        return true;
    }

    bool ResetUmbrellaStatus(const string& id, UmbrellaStatus st, string& msg)
    {
        int idx = findUmbrellaIdx(id);
        if (idx == -1) { msg = "Payung tidak ditemukan"; return false; }
        umbrellas[idx].status = st;
        msg = "Status payung " + id + " jadi " + statusToString(st) + ".";
        return true;
    }

    std::vector<string> GetAvailableUmbrellaIds() const
    {
        vector<string> out;
        for (auto& u : umbrellas)
            if (u.status == UmbrellaStatus::AVAILABLE) out.push_back(u.id);
        return out;
    }

    bool RegisterRental(const Borrower& b, const string& umbrellaId, const string& startDate, int startHour, int durationHours, string& msg)
    {
        if (b.firstName.empty() || b.lastName.empty() || b.niu.empty()) {
            msg = "Data belum lengkap";
            return false;
        }

        if (!validateTime(startHour, durationHours)) {
            msg = "Waktu tidak sesuai, Pastikan:\n"
                "- jam mulai 0–23\n"
                "- durasi > 0\n"
                "- jam mulai + durasi <= 24";
            return false;
        }

        int umbIdx = findUmbrellaIdx(umbrellaId);
        if (umbIdx == -1) { msg = "Payung tidak ditemukan"; return false; }
        if (umbrellas[umbIdx].status != UmbrellaStatus::AVAILABLE) { msg = "Payung tidak AVAILABLE"; return false; }

        int endHour = startHour + durationHours;

        Rental r;
        r.rentalId = nextRentalId++;
        r.borrower = b;
        r.umbrellaId = umbrellaId;
        r.startDate = startDate;
        r.startHour = startHour;
        r.durationHours = durationHours;
        r.endHour = endHour;

        rentals.push_back(r);
        umbrellas[umbIdx].status = UmbrellaStatus::BORROWED;

        msg = "Registrasi berhasil.\n"
            "Nomer Registrasi: " + std::to_string(r.rentalId) +
            "\nNomer Payung: " + umbrellaId +
            "\nKembali Pukul: " + std::to_string(endHour) + ":00";
        return true;
    }

    bool ReturnUmbrella(int rentalId, string& msg)
    {
        int rentIdx = findRentalIdx(rentalId);
        if (rentIdx == -1) { msg = "Nomer Registrasi tidak ditemukan."; return false; }

        string umbId = rentals[rentIdx].umbrellaId;
        int umbIdx = findUmbrellaIdx(umbId);
        if (umbIdx != -1) umbrellas[umbIdx].status = UmbrellaStatus::AVAILABLE;

        rentals.erase(rentals.begin() + rentIdx);
        msg = "Payung dikembalikan, registrasi dihapus.";
        return true;
    }
};
#pragma once