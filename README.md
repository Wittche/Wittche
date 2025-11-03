# Wittche Operating System

Wittche, x86 mimarisi için geliştirilmiş basit bir işletim sistemidir. Eğitim ve öğrenme amaçlı olarak tasarlanmıştır.

## Özellikler

- ✅ **Bootloader**: BIOS tarafından yüklenen, protected mode'a geçiş yapan bootloader
- ✅ **32-bit Kernel**: C ile yazılmış temel kernel
- ✅ **VGA Text Mode**: Ekrana metin yazdırma desteği
- ✅ **Temel Fonksiyonlar**: String işlemleri, hafıza işlemleri

## Gereksinimler

### Build için:
- `nasm` (Netwide Assembler)
- `gcc` (GNU C Compiler) - 32-bit desteği ile
- `ld` (GNU Linker)
- `make`

### Çalıştırmak için:
- `qemu-system-i386` (QEMU emülatörü)

### Ubuntu/Debian üzerinde kurulum:
```bash
sudo apt-get update
sudo apt-get install nasm gcc-multilib make qemu-system-x86
```

### Fedora/RHEL üzerinde kurulum:
```bash
sudo dnf install nasm gcc make qemu-system-x86
```

## Derleme

Projeyi derlemek için:

```bash
make
```

Bu komut şunları yapacaktır:
1. Bootloader'ı derler (`boot/boot.asm`)
2. Kernel entry point'i derler (`kernel/kernel_entry.asm`)
3. Kernel kodunu derler (`kernel/kernel.c`)
4. Tüm parçaları birleştirip `build/wittche.img` oluşturur

## Çalıştırma

QEMU ile çalıştırmak için:

```bash
make run
```

Debug modunda çalıştırmak için:

```bash
make debug
```

Debug modunda, QEMU 1234 portunda GDB bekleyecektir. Başka bir terminalde:

```bash
gdb
(gdb) target remote localhost:1234
(gdb) continue
```

## Temizleme

Build dosyalarını temizlemek için:

```bash
make clean
```

## Proje Yapısı

```
Wittche/
├── boot/
│   └── boot.asm         # Bootloader (16-bit -> 32-bit protected mode)
├── kernel/
│   ├── kernel_entry.asm # Kernel giriş noktası
│   └── kernel.c         # Ana kernel kodu
├── include/
│   └── kernel.h         # Kernel header dosyası
├── build/               # Build çıktıları (git'e eklenmez)
├── linker.ld            # Linker script
├── Makefile             # Build sistemi
└── README.md            # Bu dosya
```

## Nasıl Çalışır?

1. **BIOS Boot**: Bilgisayar açıldığında BIOS, ilk sektörü (boot sector) 0x7C00 adresine yükler
2. **Bootloader**: `boot/boot.asm` çalışır:
   - Diskten kernel'i okur
   - GDT (Global Descriptor Table) kurar
   - Protected mode'a geçer
   - Kernel'e (0x10000) atlar
3. **Kernel**: `kernel/kernel_entry.asm` çalışır ve `kernel_main()` fonksiyonunu çağırır
4. **Kernel Main**: `kernel/kernel.c` içindeki `kernel_main()`:
   - Ekranı temizler
   - Hoş geldin mesajı yazar
   - Sistem bilgilerini gösterir
   - Sonsuz döngüde bekler (HLT instruction ile)

## Geliştirme Yol Haritası

### Kısa Vadeli:
- [ ] Klavye girişi (Interrupt handler)
- [ ] Daha gelişmiş ekran çıktısı (scroll, renkler)
- [ ] Basit komut satırı (shell)

### Orta Vadeli:
- [ ] Interrupt Descriptor Table (IDT)
- [ ] Timer interrupt
- [ ] Hafıza yönetimi (paging)
- [ ] Basit dosya sistemi

### Uzun Vadeli:
- [ ] Çoklu görev (multitasking)
- [ ] Kullanıcı modu
- [ ] Sistem çağrıları (syscalls)
- [ ] Basit sürücüler (drivers)

## Kaynaklar

- [OSDev.org](https://wiki.osdev.org/) - OS geliştirme wiki
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
- [NASM Documentation](https://www.nasm.us/docs.php)

## Lisans

Bu proje eğitim amaçlıdır ve özgürce kullanılabilir.

## Katkıda Bulunma

Bu bir öğrenme projesidir. Fork'layıp kendi özelliklerinizi ekleyebilirsiniz!

---

**Not**: Bu işletim sistemi hala geliştirilme aşamasındadır ve production kullanımı için uygun değildir.
