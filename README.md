[+] Developed By: @ludo
[+] Programin Ismi: LudoKey
[+] Programin Surumu: 1.0
[+] Kullanilan Programlama Dilleri: C++

[https://i.pinimg.com/originals/34/47/47/3447473838ac39c35bbb5f6b8f2921e3.gif]

Terminalle ic ice oldugum donemlerde boyle bir araca ihtiyac duymustum; kendimi daha cok terminale bagimli kilmak ve bulut tabanli parola yoneticilerine guvenmedigim icin bu cozum ortaya cikti. Sade, hizli ve tamamen kontrolumde olan islevsel bir arac oldu.

LudoKey, terminal ortaminda calisan, verilerinizi uctan uca sifreleyerek tamamen sizin kontrolunuzde tutan hafif ve guvenli bir parola yoneticisidir. Hassas bilgilerinizi not defterlerinde veya bulut servislerinde saklamak yerine, komut satiri uzerinde endustri standardi kriptografik metotlarla korunmasi amaciyla tarafimca gelistirilmistir.


## Nasil Calisir?

Uygulamaya belirlediginiz master sifre, tum bilgilerinizin anahtaridir. Girdiginiz her hesap bilgisi (servis adi, kullanici adi ve parola), AES-256-CBC algoritmasiyla sifrelenerek diske yazilir. Bu sifreleme icin gereken anahtar, master sifrenizden PBKDF2 (SHA-256, 10.000 iterasyon) yontemiyle tuyretilir.

Her kayit islemi sirasinda rastgele salt ve IV degerleri uretilir; boylece ayni parolayi farkli servislerde kullansaniz bile diske yazilan sifreli ciktilar tamamen farkli gorunur. Dosyaniz ele gecirilse dahi master sifreniz bilinmedigi surece hicbir veriye ulasilamaz.


## Kurulum ve Calistirma

Projeyi sisteminize kurmak ve calistirmak icin terminal uzerinde su adimlari takip edebilirsiniz:

1. Gereksinimleri yukleyin (OpenSSL gelistirme kutuphanesi):

sudo apt-get install libssl-dev

2. Depoyu klonlayin ve dizine girin:

bash
git clone https://github.com/ludociel61/ludokey.git
cd ludokey

3. Kaynak kodunu derleyin:

bash
g++ main.cpp -o ludokey -lssl -lcrypto

4. Programi calistirin:

bash
./ludokey


## Temel Yetenekler

* Yeni hesap ekleme ve aninda sifreli olarak diske kaydetme
* Kayitli tum hesaplari guvenli bir sekilde listeleme
* Servis adina gore hizli arama ve filtreleme yapma
* Program kapatilirken verilerin otomatik olarak muhurlenmesi


## Guvenlik Mimarisi

* Disk uzerindeki tum veriler duz metin yerine hex-encoded sifreli bloklar halinde saklanir.
* Yanlis master sifre girilmesi durumunda program veriyi cozmeyi reddeder ve erisimi engeller.
* Bulut bagimliligi olmayan, tamamen yerel ve bagimsiz bir mimari sunar.
