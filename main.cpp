// developer by ludociel
// github page: https://github.com/ludociel61

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <termios.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

using namespace std;

struct Hesap {
    string servisAdi;
    string kullaniciAdi;
    string sifre;
};

string gizliSifreGirisiAl(const string& mesaj) {
    cout << mesaj;

    termios eskiAyar, yeniAyar;
    tcgetattr(STDIN_FILENO, &eskiAyar);
    yeniAyar = eskiAyar;
    yeniAyar.c_lflag &= ~ECHO; 
    tcsetattr(STDIN_FILENO, TCSANOW, &yeniAyar);

    string girilenSifre;
    getline(cin, girilenSifre);

    tcsetattr(STDIN_FILENO, TCSANOW, &eskiAyar); 
    cout << "\n";
    return girilenSifre;
}

//  veriyi hex formatina ceivr
string toHex(const unsigned char* data, size_t len) {
    stringstream ss;
    ss << hex << setfill('0');
    for (size_t i = 0; i < len; ++i)
        ss << setw(2) << (int)data[i];
    return ss.str();
}

// hexi baytlara donusturme
vector<unsigned char> fromHex(const string& hexStr) {
    vector<unsigned char> data;
    for (size_t i = 0; i < hexStr.length(); i += 2) {
        string byteString = hexStr.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteString.c_str(), NULL, 16);
        data.push_back(byte);
    }
    return data;
}

// OpenSSL AES-256-CBC ve PBKDF2 sifreleme
bool aesSifrele(const string& temizMetin, const string& masterSifre, string& cikisHex) {
    unsigned char salt[16];
    unsigned char iv[16];
    if (!RAND_bytes(salt, sizeof(salt)) || !RAND_bytes(iv, sizeof(iv))) return false;

    unsigned char key[32];
    if (!PKCS5_PBKDF2_HMAC(masterSifre.c_str(), masterSifre.length(),
                           salt, sizeof(salt), 10000,
                           EVP_sha256(), sizeof(key), key)) {
        return false;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    vector<unsigned char> sifreliMetin(temizMetin.length() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, sifreliUzunluk = 0;

    if (1 != EVP_EncryptUpdate(ctx, sifreliMetin.data(), &len, 
                               (unsigned char*)temizMetin.c_str(), temizMetin.length())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    sifreliUzunluk = len;

    if (1 != EVP_EncryptFinal_ex(ctx, sifreliMetin.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    sifreliUzunluk += len;
    EVP_CIPHER_CTX_free(ctx);

    sifreliMetin.resize(sifreliUzunluk);

    cikisHex = toHex(salt, sizeof(salt)) + ":" + 
               toHex(iv, sizeof(iv)) + ":" + 
               toHex(sifreliMetin.data(), sifreliMetin.size());
    return true;
}

// OpenSSL AES-256-CBC ve PBKDF2
bool aesCoz(const string& girisHex, const string& masterSifre, string& temizMetin) {
    stringstream ss(girisHex);
    string saltHex, ivHex, sifreliHex;
    getline(ss, saltHex, ':');
    getline(ss, ivHex, ':');
    getline(ss, sifreliHex, ':');

    vector<unsigned char> salt = fromHex(saltHex);
    vector<unsigned char> iv = fromHex(ivHex);
    vector<unsigned char> sifreliVeri = fromHex(sifreliHex);

    if (salt.size() != 16 || iv.size() != 16) return false;

    unsigned char key[32];
    if (!PKCS5_PBKDF2_HMAC(masterSifre.c_str(), masterSifre.length(),
                           salt.data(), salt.size(), 10000,
                           EVP_sha256(), sizeof(key), key)) {
        return false;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    vector<unsigned char> cozulmusVeri(sifreliVeri.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0, cozulmusUzunluk = 0;

    if (1 != EVP_DecryptUpdate(ctx, cozulmusVeri.data(), &len, 
                               sifreliVeri.data(), sifreliVeri.size())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    cozulmusUzunluk = len;

    if (1 != EVP_DecryptFinal_ex(ctx, cozulmusVeri.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return false; // Yanlış master şifre girilirse burada hata yakalanır
    }
    cozulmusUzunluk += len;
    EVP_CIPHER_CTX_free(ctx);

    temizMetin = string((char*)cozulmusVeri.data(), cozulmusUzunluk);
    return true;
}

void ekraniTemizle() {
    cout << "\033[2J\033[1;1H";
}

void panelBasligiCiz(const string& mesaj = "") {
    ekraniTemizle();
    cout << "==================================================\n";
    cout << "    L U D O C I E L | A R - G E         \n";
    cout << "==================================================\n";
    if (!mesaj.empty()) {
        cout << " >> DURUM: " << mesaj << "\n";
        cout << "--------------------------------------------------\n";
    }
}

void kasayiDiskeKaydet(const string& dosyaAdi, const vector<Hesap>& hesaplar, const string& masterSifre) {
    ofstream dosya(dosyaAdi);
    if (!dosya.is_open()) return;

    for (const auto& hesap : hesaplar) {
        string satirVeri = hesap.servisAdi + "|" + hesap.kullaniciAdi + "|" + hesap.sifre;
        string sifreliSatir;
        if (aesSifrele(satirVeri, masterSifre, sifreliSatir)) {
            dosya << sifreliSatir << "\n";
        }
    }
    dosya.close();
}

bool kasayiDisktenOku(const string& dosyaAdi, vector<Hesap>& hesaplar, const string& masterSifre) {
    ifstream dosya(dosyaAdi);
    if (!dosya.is_open()) return false;

    hesaplar.clear();
    string sifreliSatir;
    while (getline(dosya, sifreliSatir)) {
        if (sifreliSatir.empty()) continue;
        string temizSatir;
        if (aesCoz(sifreliSatir, masterSifre, temizSatir)) {
            stringstream ss(temizSatir);
            string servis, kullanici, sifre;
            if (getline(ss, servis, '|') && getline(ss, kullanici, '|') && getline(ss, sifre)) {
                Hesap hesap{servis, kullanici, sifre};
                hesaplar.push_back(hesap);
            }
        } else {
            dosya.close();
            return false; // Şifre yanlış
        }
    }
    dosya.close();
    return true;
}

int main() {
    setlocale(LC_ALL, "Turkish");

    string kasaDosyasi = "naberUye";
    string masterSifre;

    panelBasligiCiz("Selam tüccar.");
    masterSifre = gizliSifreGirisiAl(" sifre belirleyin: ");

    vector<Hesap> kayitliHesaplar;
    string durumMesaji = "Yeni kasa olusturuldu.";
    
    ifstream kontrol(kasaDosyasi);
    if (kontrol.is_open()) {
        kontrol.close();
        if (kasayiDisktenOku(kasaDosyasi, kayitliHesaplar, masterSifre)) {
            durumMesaji = "Kasa basariyla cozuldu ve yuklendi.";
        } else {
            cout << "\n[HATA] Hatali Master Sifre veya bozuk kasa dosyasi!\n";
            return 1;
        }
    }

    int secim = 0;
    do {
        panelBasligiCiz(durumMesaji);
        durumMesaji = "";

        cout << " [1] Hesaplari Listele\n";
        cout << " [2] Yeni Hesap Ekle\n";
        cout << " [3] Hesap Ara\n";
        cout << " [4] Cikis ve Kaydet\n";
        cout << "--------------------------------------------------\n";
        cout << "Seciminiz [1-4]: ";
        cin >> secim;

        if (secim == 1) {
            panelBasligiCiz("Kayitli Hesaplar Listesi");
            if (kayitliHesaplar.empty()) {
                cout << " (Henuz kayitli bir hesap bulunmuyor.)\n";
            } else {
                for (size_t i = 0; i < kayitliHesaplar.size(); ++i) {
                    cout << " " << i + 1 << ". Servis   : " << kayitliHesaplar[i].servisAdi << "\n";
                    cout << "    Kullanici: " << kayitliHesaplar[i].kullaniciAdi << "\n";
                    cout << "    Sifre    : " << kayitliHesaplar[i].sifre << "\n";
                    cout << "--------------------------------------------------\n";
                }
            }
            cout << "\nDevam etmek icin Enter'a basin...";
            cin.ignore();
            cin.get();
        } 
        else if (secim == 2) {
            panelBasligiCiz("Yeni Hesap Ekleme");
            Hesap yeniHesap;
            cout << "Servis Adi (Orn: GitHub) : ";
            cin >> yeniHesap.servisAdi;
            cout << "Kullanici Adi / E-posta  : ";
            cin >> yeniHesap.kullaniciAdi;
            cout << "Sifre                    : ";
            cin >> yeniHesap.sifre;

            kayitliHesaplar.push_back(yeniHesap);
            kasayiDiskeKaydet(kasaDosyasi, kayitliHesaplar, masterSifre);
            durumMesaji = "Basarili: Hesap eklendi ve AES-256 ile şifrelenerek kaydedildi!";
        } 
        else if (secim == 3) {
            panelBasligiCiz("Hesap Arama");
            string arananServis;
            cout << "Aranacak Servis Adi: ";
            cin >> arananServis;

            cout << "\n--- Sonuclar ---\n";
            bool bulundu = false;
            for (const auto& h : kayitliHesaplar) {
                if (h.servisAdi.find(arananServis) != string::npos) {
                    cout << " Servis   : " << h.servisAdi << "\n";
                    cout << " Kullanici: " << h.kullaniciAdi << "\n";
                    cout << " Sifre    : " << h.sifre << "\n";
                    cout << "--------------------------------\n";
                    bulundu = true;
                }
            }
            if (!bulundu) {
                cout << " Bu isimde bir kayit bulunamadi.\n";
            }
            cout << "\nDevam etmek icin Enter'a basin...";
            cin.ignore();
            cin.get();
        } 
        else if (secim == 4) {
            kasayiDiskeKaydet(kasaDosyasi, kayitliHesaplar, masterSifre);
            panelBasligiCiz("Guvenli Cikis");
            cout << "Veriler şifrelenerek kaydedildi. Program kapatiliyor...\n";
        } 
        else {
            durumMesaji = "Hata: Gecersiz secim yaptiniz, tekrar deneyin.";
        }

    } while (secim != 4);

    return 0;
}