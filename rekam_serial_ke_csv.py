#!/usr/bin/env python3
"""
Menangkap sesi rekam dari rekam_data.ino dan otomatis menyimpannya
sebagai file .csv yang siap di-upload ke Edge Impulse.

Instalasi sekali saja:
    pip install pyserial

Jalankan (contoh):
    python rekam_serial_ke_csv.py -p COM5 -l true1
    python rekam_serial_ke_csv.py -p /dev/ttyUSB0 -l unknown

Setiap kali Enter ditekan di sini, script ini mengirim '1' ke board
(sama seperti mengetik '1' di Serial Monitor), lalu menangkap data
di antara marker START...END dan menyimpannya sebagai:
    data/<label>.<nomor_sesi>.csv

Nama file berformat <label>.<id>.csv ini akan otomatis dikenali
Edge Impulse sebagai label kelas saat di-upload.

PENTING: tutup Serial Monitor Arduino IDE dulu sebelum menjalankan
script ini -- satu port serial hanya bisa dipakai satu program.
"""

import argparse
import os
import time

import serial


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-p', '--port', required=True,
                         help='Port serial, misal COM5 atau /dev/ttyUSB0')
    parser.add_argument('-b', '--baud', type=int, default=115200)
    parser.add_argument('-l', '--label', required=True,
                         help="Nama kelas: true1 / true2 / unknown")
    parser.add_argument('-o', '--outdir', default='data',
                         help='Folder tempat menyimpan file CSV')
    args = parser.parse_args()

    os.makedirs(args.outdir, exist_ok=True)

    ser = serial.Serial(args.port, args.baud, timeout=1)
    time.sleep(2)  # beri waktu board reset setelah port dibuka
    ser.reset_input_buffer()

    session_id = 0
    print(f"Terhubung ke {args.port}. Label sesi ini: '{args.label}'")

    while True:
        cmd = input("\nEnter untuk mulai sesi rekam berikutnya "
                     "(atau ketik 'q' lalu Enter untuk keluar): ")
        if cmd.strip().lower() == 'q':
            break

        ser.reset_input_buffer()
        ser.write(b'1')  # trigger sesi rekam di board

        lines = []
        recording = False
        print("Merekam...", end='', flush=True)

        while True:
            raw = ser.readline().decode(errors='ignore').strip()
            if not raw:
                continue
            if raw == 'START':
                recording = True
                continue
            if raw == 'END':
                break
            if recording:
                lines.append(raw)

        if len(lines) <= 1:  # cuma header, tidak ada data
            print(" tidak ada data diterima, coba lagi.")
            continue

        filename = os.path.join(args.outdir, f"{args.label}.{session_id}.csv")
        with open(filename, 'w', newline='') as f:
            f.write("\n".join(lines) + "\n")

        print(f" tersimpan: {filename}  ({len(lines) - 1} baris data)")
        session_id += 1

    ser.close()
    print("Selesai.")


if __name__ == '__main__':
    main()
