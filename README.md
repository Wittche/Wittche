# Wittche Operating System

Wittche, x86 mimarisi için geliştirilmiş basit bir işletim sistemidir. Eğitim ve öğrenme amaçlı olarak tasarlanmıştır.

## Özellikler

- ✅ **Bootloader**: BIOS tarafından yüklenen, protected mode'a geçiş yapan bootloader
- ✅ **32-bit Kernel**: C ile yazılmış temel kernel
- ✅ **VGA Text Mode**: Ekrana metin yazdırma desteği
- ✅ **IDT (Interrupt Descriptor Table)**: Interrupt yönetimi
- ✅ **Keyboard Driver**: PS/2 klavye desteği, scancode to ASCII dönüşümü
- ✅ **Simple Shell**: Temel komut satırı arayüzü (help, clear, about, echo)
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
│   ├── kernel.c         # Ana kernel kodu ve shell
│   ├── idt.c            # Interrupt Descriptor Table
│   ├── isr.c            # Interrupt Service Routines
│   ├── interrupt.asm    # Interrupt handler stubs
│   └── keyboard.c       # Klavye sürücüsü
├── include/
│   ├── kernel.h         # Kernel header dosyası
│   ├── idt.h            # IDT header
│   ├── keyboard.h       # Klavye sürücüsü header
│   ├── ports.h          # I/O port fonksiyonları
│   └── types.h          # Tip tanımlamaları
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
3. **Kernel Entry**: `kernel/kernel_entry.asm` çalışır ve `kernel_main()` fonksiyonunu çağırır
4. **Kernel Main**: `kernel/kernel.c` içindeki `kernel_main()`:
   - Ekranı temizler
   - IDT (Interrupt Descriptor Table) kurar
   - PIC (Programmable Interrupt Controller) yapılandırır
   - Klavye sürücüsünü başlatır
   - Interrupt'ları etkinleştirir (sti)
   - Shell döngüsüne girer
5. **Shell**: Kullanıcıdan komut alır ve işler:
   - `help`: Kullanılabilir komutları gösterir
   - `clear`: Ekranı temizler
   - `about`: Sistem bilgilerini gösterir
   - `echo <mesaj>`: Mesajı ekrana yazar

## Geliştirme Yol Haritası

### Tamamlananlar:
- [x] Klavye girişi (Interrupt handler)
- [x] Interrupt Descriptor Table (IDT)
- [x] Basit komut satırı (shell)
- [x] PIC (Programmable Interrupt Controller) konfigürasyonu

### Kısa Vadeli:
- [ ] Daha gelişmiş ekran çıktısı (scroll, cursor, renkler)
- [ ] Timer interrupt (PIT - Programmable Interval Timer)
- [ ] Daha fazla shell komutu
- [ ] String parsing ve tokenization

### Orta Vadeli:
- [ ] Hafıza yönetimi (paging)
- [ ] Heap allocator (kmalloc/kfree)
- [ ] VFS (Virtual File System) katmanı
- [ ] Basit dosya sistemi (FAT12 veya custom)

### Uzun Vadeli:
- [ ] Çoklu görev (multitasking)
- [ ] Process/Thread yönetimi
- [ ] Kullanıcı modu
- [ ] Sistem çağrıları (syscalls)
- [ ] Daha fazla sürücü (ATA disk, serial port, vb.)

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
