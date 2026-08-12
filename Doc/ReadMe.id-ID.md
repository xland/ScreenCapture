[简体中文](https://github.com/xland/ScreenCapture/) | [English](./ReadMe.en-US.md) | [Русский](./ReadMe.ru-RU.md) | Bahasa Indonesia

![banner](./banner.png)

**ScreenCapture** Alat penangkap layar Windows yang powerful dan ringan.

## Fitur

- Screenshot, anotasi, screenshot panjang (scroll), perekaman layar.
- Pipet warna dengan pintasan keyboard untuk menyalin warna RGB (`Ctrl+R`), HEX (`Ctrl+H`), dan CMYK (`Ctrl+K`).
- Menggambar elips dengan atau tanpa isi, lingkaran sempurna (tahan `Shift`), persegi panjang, persegi (tahan `Shift`), panah, label bernomor, dll.
- Menggambar kurva, garis lurus (tahan `Shift`), mosaik, penghapus, teks.
- Mengubah atau menghapus elemen yang telah digambar kapan saja (arahkan kursor mouse pada elemen).
- Urungkan (`Ctrl+Z`), ulangi (`Ctrl+Y`), simpan ke file (`Ctrl+S`), simpan ke clipboard (`Ctrl+C` atau klik dua kali).
- Screenshot panjang dengan scroll, perekaman layar (format output GIF/Mp4).
- Dibangun di atas kerangka GUI [Ling](https://github.com/xland/Ling).
- Mendukung plugin pengenalan teks OCR [ImageReader(25MB)](https://github.com/xland/ImageReader).
- Mendukung pengenalan kode QR.
- Performa cepat dengan penggunaan memori minimal.
- Ukuran kecil, hanya satu file eksekusi, tanpa instalasi, tidak memerlukan pustaka tautan dinamis.
- Mendukung menjalankan fungsi tertentu secara langsung melalui argumen baris perintah.
- Mendukung mode sekali pakai (proses tidak akan terus berjalan di sistem).
- Dukungan multibahasa.

## Unduh

[Release](https://github.com/xland/ScreenCapture/releases/) (1MB)

## Sistem Operasi yang Didukung

- Windows 10 1607 atau yang lebih baru

## Kompilasi

- Branch main bergantung pada kerangka GUI [Ling](https://github.com/xland/Ling).
- Proyek ini dapat dikompilasi dengan Visual Studio 2026 (dengan C++ Desktop Dev Kit terpasang).
- [2.4.25 (berbasis D2D)](https://github.com/xland/ScreenCapture/tree/2.4.25) atau [2.3.3 (berbasis Qt)](https://github.com/xland/ScreenCapture/tree/2.3.3_qt) adalah branch stabil sebelumnya.

## Baris Perintah

```
// Hentikan proses segera setelah pengambilan gambar selesai.
> ScreenCapture.exe --auto-quit=true
```

## Plugin Pengenalan Teks (OCR)

Unduh versi terbaru alat pengenalan teks [ImageReader.exe](https://github.com/xland/ImageReader/releases) (sekitar 25MB), lalu letakkan file tersebut di direktori yang sama dengan ScreenCapture.exe, atau di direktori `%appdata%\ScreenCapture\plugin`, kemudian jalankan ulang aplikasi untuk menggunakannya.

## Sponsor

<table>
  <tr>
    <td align="center">
      <img alt="Sponsor Alipay" src="./alipay.jpg" width="160" height="160">
      <p>Sponsor Alipay</p>
    </td>
    <td align="center">
      <img alt="Sponsor WeChat" src="./wechat.png" width="160" height="160">
      <p>Sponsor WeChat</p>
    </td>
    <td align="center">
      <img alt="WeChat Penulis" src="./author.jpg" width="160" height="160">
      <p>WeChat Penulis</p>
    </td>
    <td align="center">
      <img alt="QR Code Akun Resmi" src="./gongzhonghao.jpg" width="160" height="160">
      <p>Blog WeChat: Desktop Software</p>
    </td>
  </tr>
</table>
