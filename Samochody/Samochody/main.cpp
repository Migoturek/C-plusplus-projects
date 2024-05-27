#include <iostream>
#include <locale.h>
#include <string>
#include <vector>
#include <sqlite3.h>

using namespace std;

class Auto
{
public:

	string marka;
	string nr_rej;
	string model;
	int rok_prod;
	double przebieg;

	void Wczytaj() {
		cout << "Dodaj samochód do Bazy Danych" << endl;
		cout << "Podaj Markê : ";
		cin >> marka;

		cout << "Podaj Model: ";
		cin >> model;

		cout << "Podaj Rok Produkcji: ";
		cin >> rok_prod;

		cout << "Podaj Numer rejestracyjny: ";
		cin >> nr_rej;

		cout << "Podaj Przebieg: ";
		cin >> przebieg;
		
	};


	void Wypisz() {
		cout << "Informacje o PojeŸdzie: " << marka << "O tablicach: " << nr_rej << endl;
		cout << "Markê : " << marka << endl;
		cout << "Model: " << model << endl;
		cout << "Rok Produkcji: " << rok_prod << endl;
		cout << "Numer rejestracyjny: " << nr_rej << endl;
		cout << "Przebieg: "<< przebieg << "KM" << endl;




	};


	void Zapisz_do_BD(sqlite3* db) const {
		const char* sql = "INSERT INTO Auto (marka, model, rok_prod, nr_rej, przebieg) VALUES (?, ?, ?, ?, ?);";
		sqlite3_stmt* stmt;
		int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
		if (rc != SQLITE_OK) {
			cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
			return;
		}

		sqlite3_bind_text(stmt, 1, marka.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_text(stmt, 2, model.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_int(stmt, 3, rok_prod);
		sqlite3_bind_text(stmt, 4, nr_rej.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_double(stmt, 5, przebieg);

		rc = sqlite3_step(stmt);
		if (rc != SQLITE_DONE) {
			cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
		}

		sqlite3_finalize(stmt);
	}
};

void createTable(sqlite3* db) {
	const char* sql = "CREATE TABLE IF NOT EXISTS Auto ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT, "
		"marka TEXT, "
		"model TEXT, "
		"rok_prod INTEGER, "
		"nr_rej TEXT, "
		"przebieg REAL);";

	char* errmsg = nullptr;
	int rc = sqlite3_exec(db, sql, nullptr, nullptr, &errmsg);
	if (rc != SQLITE_OK) {
		cerr << "SQL error: " << errmsg << endl;
		sqlite3_free(errmsg);
	}




};




int main() {
	setlocale(LC_CTYPE, "Polish");
	vector<Auto> objects;

	sqlite3* db;
	sqlite3_stmt* stmt;

	int rc = sqlite3_open("mydb.db", &db);
	if (rc) {
		cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
		return 1;
	}

	createTable(db);

	int wybierz_czynnosc;

	while (true) {
		cout << "1: Dodaj auto, 2: Poka¿ dane wpisanego auta, 3: Zapisz dane do bazy, 4: Zakoñcz 5: Odczytaj z Bazy Danych" << endl;
		cin >> wybierz_czynnosc;

		switch (wybierz_czynnosc) {
		case 1: {
			Auto nowe_auto;
			nowe_auto.Wczytaj();
			objects.push_back(nowe_auto);
			break;
		}

		case 2: {
			for (size_t i = 0; i < objects.size(); ++i) {
				cout << "\nSamochód " << i + 1 << endl;
				objects[i].Wypisz();
			}
			break;
		}

		case 3: {
			for (const auto& car : objects) {
				car.Zapisz_do_BD(db);
			}
			cout << "Dane zapisane do bazy danych." << endl;
			break;
		}

		case 4:
			sqlite3_close(db);
			cout << "Koniec programu." << endl;
			return 0;

		case 5: {
			cout << "Odczyt z bazy danych" << endl;
			const char* sql = "SELECT id, marka, model, rok_prod, nr_rej, przebieg FROM Auto";
			sqlite3_stmt* stmt;
			rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
			if (rc != SQLITE_OK) {
				cerr << "Cannot prepare statement: " << sqlite3_errmsg(db) << endl;
				break;
			}

			while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
				int id = sqlite3_column_int(stmt, 0);
				const unsigned char* marka = sqlite3_column_text(stmt, 1);
				const unsigned char* model = sqlite3_column_text(stmt, 2);
				int rok_prod = sqlite3_column_int(stmt, 3);
				const unsigned char* nr_rej = sqlite3_column_text(stmt, 4);
				double przebieg = sqlite3_column_double(stmt, 5);

				cout << "ID: " << id
					<< ", Marka: " << marka
					<< ", Model: " << model
					<< ", Rok produkcji: " << rok_prod
					<< ", Numer rejestracyjny: " << nr_rej
					<< ", Przebieg: " << przebieg << " KM" << endl;
			}
			sqlite3_finalize(stmt);
			break;
		}
		default:
			cout << "Nieprawid³owy wybór. Spróbuj ponownie." << endl;
			break;
		}
	}

	sqlite3_close(db);
	return 0;
}







