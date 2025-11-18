# Wittche Operating System

Wittche, x86 mimarisi için geliştirilmiş eğitim amaçlı bir işletim sistemidir. Sıfırdan yazılmış, temiz ve anlaşılır kod yapısıyla OS geliştirme öğrenmek isteyenler için mükemmel bir başlangıç noktasıdır.

## Özellikler

### 🚀 Core Features
- ✅ **Custom Bootloader**: BIOS boot, GDT setup, protected mode geçişi
- ✅ **32-bit Protected Mode Kernel**: Tamamen C ve Assembly ile yazılmış
- ✅ **Interrupt Management**: Tam IDT ve ISR/IRQ desteği
- ✅ **Hardware Drivers**: PS/2 klavye, VGA text mode

### 🎨 Display
- ✅ **VGA Text Mode Driver**: 80x25 renkli metin çıktısı
- ✅ **Hardware Cursor**: VGA donanım cursor desteği
- ✅ **Proper Scrolling**: Otomatik ekran kaydırma
- ✅ **16 Color Support**: Tam VGA renk paleti

### ⌨️ Input
- ✅ **PS/2 Keyboard Driver**: Donanım interrupt tabanlı
- ✅ **Scancode to ASCII**: US QWERTY layout
- ✅ **Special Keys**: Shift, Caps Lock, Backspace, Enter desteği
- ✅ **Input Buffering**: Circular queue ile klavye buffer'ı

### 💻 Shell
- ✅ **Interactive Shell**: Tam özellikli komut satırı arayüzü
- ✅ **Command Parser**: Argüman ayrıştırma ve tokenization
- ✅ **Command History**: Son 10 komut geçmişi
- ✅ **Built-in Commands**: help, clear, about, echo, color, history, banner
- ✅ **Colorful Output**: Renkli komut çıktıları

### 📚 Libraries
- ✅ **String Library**: strlen, strcmp, strcpy, strcat, split, trim, vb.
- ✅ **Memory Functions**: memset, memcpy, memcmp
- ✅ **Conversion Functions**: atoi, itoa (çeşitli tabanlar)
- ✅ **Modular Design**: Temiz, ayrılmış modül yapısı

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

Bu komut tüm kaynak dosyaları derleyip `build/wittche.img` dosyasını oluşturur.

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
│   └── boot.asm         # Bootloader (BIOS -> Protected Mode)
│
├── kernel/              # Kernel source files
│   ├── kernel_entry.asm # Assembly entry point
│   ├── kernel.c         # Main kernel initialization
│   ├── screen.c         # VGA text mode driver
│   ├── idt.c            # Interrupt Descriptor Table
│   ├── isr.c            # Interrupt Service Routines
│   ├── interrupt.asm    # ISR/IRQ assembly stubs
│   ├── keyboard.c       # PS/2 keyboard driver
│   ├── string.c         # String utility functions
│   └── shell.c          # Interactive shell
│
├── include/             # Header files
│   ├── kernel.h         # Kernel main header
│   ├── screen.h         # Screen driver interface
│   ├── idt.h            # IDT structures & functions
│   ├── keyboard.h       # Keyboard driver interface
│   ├── shell.h          # Shell interface
│   ├── string.h         # String utilities
│   ├── ports.h          # I/O port operations
│   └── types.h          # Type definitions
│
├── build/               # Build output (gitignore'd)
├── linker.ld            # Linker script
├── Makefile             # Build system
└── README.md            # This file
```

## Nasıl Çalışır?

### Boot Süreci
1. **BIOS Boot**:
   - BIOS, boot sektörü (512 byte) 0x7C00 adresine yüklenir
   - Boot signature (0xAA55) kontrol edilir

2. **Bootloader** (`boot/boot.asm`):
   - Kernel'i diskten 0x1000:0x0000'e yükler (18 sektör)
   - GDT (Global Descriptor Table) kurar
   - Protected mode'a geçer (CR0.PE = 1)
   - Kernel'e (0x10000) far jump yapar

3. **Kernel Entry** (`kernel/kernel_entry.asm`):
   - Protected mode segment register'larını ayarlar
   - Stack pointer'ı kurar (ESP = 0x90000)
   - `kernel_main()` C fonksiyonunu çağırır

4. **Kernel Initialization** (`kernel/kernel.c`):
   - Screen driver başlatılır (VGA 80x25)
   - IDT kurar ve PIC yeniden eşlenir
   - Keyboard driver başlatılır (IRQ1)
   - Interrupt'lar etkinleştirilir (`sti`)
   - Shell başlatılır

5. **Shell Loop** (`kernel/shell.c`):
   - Prompt gösterir: `wittche> `
   - Klavye input'u bekler (interrupt-driven)
   - Komutları parse eder ve çalıştırır
   - Sonuçları renkli olarak gösterir

### Shell Komutları
- **help**: Tüm komutları listeler
- **clear**: Ekranı temizler
- **about**: Sistem bilgileri ve özellikler
- **echo <text>**: Metni ekrana yazar
- **color**: Renk paletini gösterir
- **history**: Komut geçmişini gösterir
- **banner**: Hoş geldin mesajını gösterir

## Kod Kalitesi ve Best Practices

### ✨ Yapılan İyileştirmeler
- **Modular Architecture**: Her özellik ayrı modülde
- **Clean Code**: İyi isimlendirme, açıklayıcı yorumlar
- **Proper Scrolling**: Gerçek ekran kaydırma (satır kopyalama)
- **Hardware Cursor**: VGA cursor register'ları kullanımı
- **Error Handling**: Detaylı exception mesajları
- **Color Coding**: Mesaj tipine göre renkli çıktı
- **Input Validation**: Güvenli string işlemleri
- **Buffer Management**: Circular queue keyboard buffer

### 🔧 Teknik Detaylar
- **Memory Layout**:
  - Bootloader: 0x7C00
  - Kernel: 0x10000
  - Stack: 0x90000 (grows down)
  - VGA Text: 0xB8000

- **Interrupt Mapping**:
  - ISR 0-31: CPU Exceptions
  - IRQ 32-47: Hardware Interrupts (PIC remapped)
  - IRQ 33 (IRQ1): Keyboard

- **VGA Text Mode**:
  - 80x25 characters
  - 16 foreground + 16 background colors
  - Character format: [bg:4][fg:4][char:8]

## Geliştirme Yol Haritası

### ✅ Tamamlananlar (v0.3):
- [x] Custom bootloader with GDT
- [x] Protected mode kernel
- [x] IDT with full ISR/IRQ support
- [x] PIC configuration and remapping
- [x] VGA text mode driver
- [x] Hardware cursor support
- [x] Proper screen scrolling
- [x] PS/2 keyboard driver
- [x] Input buffering
- [x] Interactive shell
- [x] Command parser
- [x] Command history
- [x] String library
- [x] Color support
- [x] Modular code architecture

### 🎯 Kısa Vadeli (v0.4):
- [ ] Timer interrupt (PIT)
- [ ] System uptime tracking
- [ ] Better line editing (cursor keys, delete)
- [ ] Tab completion
- [ ] Printf-style formatting

### 🚀 Orta Vadeli (v0.5-v1.0):
- [ ] Memory management (paging)
- [ ] Heap allocator (kmalloc/kfree)
- [ ] Physical memory manager
- [ ] ATA/IDE disk driver
- [ ] FAT12/16 file system
- [ ] VFS layer

### 🌟 Uzun Vadeli (v2.0+):
- [ ] Multitasking (cooperative/preemptive)
- [ ] Process management
- [ ] User mode
- [ ] System calls
- [ ] ELF binary loader
- [ ] More drivers (serial, network, etc.)

## Kaynaklar

### Dokumentasyon
- [OSDev.org](https://wiki.osdev.org/) - OS geliştirme wiki (en kapsamlı kaynak)
- [Intel x86 Manual](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html) - Resmi Intel dökümanları
- [NASM Documentation](https://www.nasm.us/docs.php) - NASM assembler referansı

### Kitaplar
- "Operating Systems: Design and Implementation" - Andrew S. Tanenbaum
- "Modern Operating Systems" - Andrew S. Tanenbaum
- "Operating System Concepts" - Silberschatz, Galvin, Gagne

### Tutorials
- [Bona Fide OS Developer](http://www.osdever.net/)
- [James Molloy's Kernel Tutorials](http://www.jamesmolloy.co.uk/tutorial_html/)
- [Bran's Kernel Development](http://www.osdever.net/bkerndev/index.php)

## Lisans

Bu proje eğitim amaçlıdır ve özgürce kullanılabilir.

## Katkıda Bulunma

Bu bir öğrenme projesidir. Fork'layıp kendi özelliklerinizi eklemekten çekinmeyin!

### Önerilen Geliştirmeler
1. Daha fazla shell komutu ekleyin
2. Timer interrupt implementasyonu
3. Daha iyi hata mesajları
4. Memory management
5. Dosya sistemi desteği

---

**Wittche OS v0.3** - Education amaçlı x86 operating system
