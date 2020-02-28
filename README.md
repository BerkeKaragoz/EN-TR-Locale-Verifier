# Linux Dil Kontrolü
- Dil Sistemi Nasıl İşler?
  - Komutlar
  - Dosyalar
  - Özet
- Yerel Dil Kontrolcüsü
## Dil Sistemi Nasıl İşler?
Bilinmesi gereken ana **3 komut** ve **2 dosya** vardır:
##### Komutlar
1. `locale`: Sistemin bölümlerinin hangi dil ayarlarında bulunduklarını tutan değişkenleri gösterir.
2. `locale -a`: Sistemde hazırda yüklü olan dil paketlerini listeler.
3. `sudo locale-gen`: **`/etc/locale.gen`** dosyasındaki listelenmiş diller yüklü değil ise yükler.
##### Dosyalar
1. `/etc/default/locale`: üstte bulunan **`locale`** komutunun referans ettiği dosyadır. İçinde, POSIX ve GNU C kütüphanesi tarafından kategorize edilmiş kısımların dil değerlerini içerir. Bu değerlere göre, okunduğu zaman, eğer dil paketleri yüklü ise dil ayarlarını değerlere göre değiştirir. **`LANG`** değeri sistem ana yerel dilini belirlemekte kullanılır.
İlk dört satırı aşağıdaki gibidir:
```
LANG=tr_TR.UTF-8
LANGUAGE=
LC_CTYPE="en_US.UTF-8"
LC_NUMERIC="tr_TR.UTF-8"
...
```
2. `/etc/locale.gen`: İçinde yüklenmesi için bulunan dil paketleri listesini bulundurur. **`sudo locale-gen`** komutu çalıştırıldığında, yorumlama içine (`#`) alınmamış olan dil değerlerini bu dosyadan okur ve hangi dilleri yükleyeceğine bu dosyadan karar verir. Varsayılan olarak `/usr/share/i18n/SUPPORTED` dosyasındaki dil listesini alır ve yüklü olmayanları yorum içinde barındırır.
Dosyanın içeriği aşağıdaki gibidir:
```
...
# tr_TR ISO-8859-9
tr_TR.UTF-8 UTF-8
# ts_ZA UTF-8
# tt_RU UTF-8
...
```
##### Özet
Sistem açılırken `locale` dosyasından sistem bölmelerinin dil değerlerini okur ve yüklü dil paketlerinden yazıları değiştirir. Buradaki dil değerlerini değiştirip, bilgisayarı yeniden başlatarak, sistem bölmelerinin de dilleri değiştirilebilir. Yüklü dil paketleri `locale -a` ile kontrol edilebilir. Eğer yüklü olmayan bir dil paketi kullanılmak isteniyorsa, `locale.gen` dosyasına yüklenmek istenen dili ekledikten sonra `sudo locale-gen` ile o listedeki diller yüklenebilir.
## Yerel Dil Kontrolcüsü
Bu yazılım sistem diline bakarak, İngilizce veya Türkçe olmak üzere dil paketlerini doğrular. Dil paketleri yüklü değilse yükler, değerler hata vermeye yatkın ise düzeltir. **C** ve **Shell Scripting** ile yazılmıştır.
Yazılımın yeterli yetkilerin olup olmadığını kontrol ettikten sonra `locale`'den  `LANG` değerine bakarak sistem dilini öğrenir.
```c
// Get the locale LANG
char *lang_value = run_command("cat /etc/default/locale 2> /dev/null | grep '^LANG=' | cut -d '=' -f2 | tr '\\n' '\\0'");
```
``char* run_command(char* command)`` özel fonksiyonu ise **Shell** komutlarını çalıştırır ve çıktılarını geri döndürür.
`lang_value`'ye `LANG` değerini atadıktan sonra hangi dil paketlerinin yüklü olması gerektiğine karar verir. Eğer `LANG` değerini tanıyamazsa İngilizce'ye çevirir.
```c
#define LANG_TR_TR "tr_TR.UTF-8" // Dil kodu
#define LOCALE_TURKISH // Doğrulanmış olan 'locale' değerleri
...
// Extract the LANGs needed to be installed after verifying
size_t ln_count; // ln == locales_needed
char **locales_needed = !strcmp(lang_value, LANG_TR_TR) ?
	extract_charmaps(LOCALE_TURKISH, &ln_count) : extract_charmaps(LOCALE_ENGLISH, &ln_count);
```
Gerekli dil paketleri `locales_needed` değişkenine atandıktan sonra tek tek dil paketlerinin yüklü olup olmadığı kontrol edilir. Eğer yüklü değilse komut satırına bildirimi yazılır ve `locale.gen` listesine `void add_lang_to_localegen(char* lang)` özel fonksiyonu ile eklenir.
```c
// Check if the LANG is installed -> locale -a
char *locale_installed;
bool is_locale_gen_needed = false;
for(int ln_i = 0; ln_i < ln_count-1; ln_i++){ //-1 for null
  // $ locale -a 2> /dev/null | grep -i LANG
  locale_installed = search_locale_installed(*(locales_needed+ln_i));
  if (*locale_installed == '\0'){
    printf("Locale '%s' is not installed.\n", *(locales_needed+ln_i));
    is_locale_gen_needed = true;
    add_lang_to_localegen(*(locales_needed+ln_i));
  }
  free(locale_installed);
}// for
```
Yukarıdaki `void add_lang_to_localegen(char* lang)` özel fonksiyonu aşağıdakine denk bir **Shell** komutu çalıştır. Bu komut, `/usr/share/i18n/SUPPORTED` listesinden dil kodlarına bakar. Alınan dil kodu değerini `locale.gen` listesine kendisinin kopyası olmadan ekler.
```sh
$ search="^lang\.";
result=$(grep $search /usr/share/i18n/SUPPORTED);
grep -v "$result" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null;
echo $result | sudo tee -a /etc/locale.gen > /dev/null
```
Gerekli paketler `locale.gen` listesine eklendikten sonra `run_command("sudo locale-gen")` ile gerekli dil paketleri yüklenir. Onaylandıktan sonra, `lang_value` değerine göre hangi dilin doğrulanmış değerlerinin (örn: `LOCALE_TURKISH`) `locale` dosyasına yazılacağı karar verilerek yazılır.
*Son*