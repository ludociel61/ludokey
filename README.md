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
