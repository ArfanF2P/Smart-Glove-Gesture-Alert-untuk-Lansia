# Studi Literatur: TinyML pada ESP32-S3 untuk Smart Glove Gesture Alert bagi Lansia

---

## 1. Pendahuluan

Dokumen ini merupakan studi literatur pendukung untuk proyek *Smart Glove Gesture Alert*, sebuah wearable device berbasis EdgeAI yang dirancang untuk membantu lansia menyampaikan kebutuhan (darurat maupun non-darurat) kepada pengasuh melalui gestur tangan, dengan keluaran berupa suara. Sistem mengklasifikasikan gestur ke dalam tiga kelas True1 (darurat), True2 (non-darurat), dan Unknown (gerakan normal, diabaikan) menggunakan model Machine Learning yang dilatih melalui Edge Impulse dan dijalankan langsung (on-device) pada ESP32-S3, dengan sensor utama accelerometer tiga sumbu LIS2DH.

Tujuan studi literatur ini adalah untuk (1) memetakan posisi TinyML dalam konteks perangkat mikrokontroler seperti ESP32-S3, (2) meninjau karakteristik sensor LIS2DH dan kesesuaiannya untuk deteksi gestur, (3) mempelajari penelitian terkait klasifikasi gestur dan deteksi jatuh berbasis accelerometer, (4) meninjau pipeline pengembangan Edge Impulse secara teknis, dan (5) memposisikan kontribusi proyek ini di antara karya-karya sejenis.

---

## 2. TinyML sebagai Paradigma Machine Learning pada Perangkat Tepi

TinyML merujuk pada sekumpulan teknik yang memungkinkan inferensi machine learning dijalankan langsung pada mikrokontroler berdaya rendah, tanpa bergantung pada komputasi cloud [1]. Zim (2021) mencatat bahwa perkembangan mikrokontroler seperti keluarga ESP32 dalam dekade terakhir telah menjadikannya cukup mumpuni untuk menjalankan beban kerja machine learning berskala kecil, mendorong proliferasi perangkat IoT yang "cerdas" secara mandiri di tepi jaringan [1]. Pendekatan ini secara khusus relevan untuk alat bantu lansia seperti proyek ini, karena tidak bergantung pada koneksi internet yang mungkin tidak stabil di lingkungan rumah, serta memberikan latensi respons yang jauh lebih rendah dibanding pendekatan berbasis cloud.

Riset ArrythML pada deteksi aritmia menunjukkan bahwa ESP32-S3 dari Espressif Systems, prosesor dual-core Xtensa LX7 32-bit hingga 240 MHz, 512 KB SRAM on-chip, serta dukungan hingga 16 MB Flash eksternal dan PSRAM opsional merupakan platform yang layak untuk deployment inferensi on-device menggunakan TensorFlow Lite for Microcontrollers (TFLite Micro) [2]. Alur kerja deployment yang digunakan dalam studi tersebut kuantisasi model, konversi ke array C melalui utilitas `xxd`, integrasi ke firmware, lalu flashing melalui ESP-IDF sejalan dengan alur kerja standar TinyML yang akan diadopsi pada proyek ini melalui ekspor model dari Edge Impulse [2].

---

## 3. ESP32-S3 sebagai Platform Klasifikasi Gestur: Bukti Kelayakan dari Penelitian Sejenis

Paper yang secara langsung paling relevan dengan proyek ini adalah *"A Generalized TinyML Workflow for Energy-Efficient Hand Gesture Recognition on ESP32S3"*, yang mengimplementasikan pipeline TinyML lengkap untuk pengenalan gestur tangan secara real-time pada ESP32S3 [3]. Studi tersebut menggunakan model TinyCNN terkuantisasi 8-bit dan mencapai akurasi klasifikasi 93,6% untuk tiga gestur umum, dengan latensi inferensi end-to-end sebesar 180 ms pada frekuensi 80 MHz, serta daya puncak sekitar 280 mW yang dapat ditekan hingga di bawah 10 mW rata-rata melalui duty-cycling [3]. Melalui kompresi model (kuantisasi dan pruning terstruktur), ukuran memori berhasil ditekan dari 1,2 MB menjadi 240 KB [3]. Temuan ini menjadi acuan realistis untuk target performa proyek ini (latensi <200 ms, akurasi ≥90%), sekaligus menunjukkan bahwa ESP32-S3 mampu menjalankan klasifikasi gestur multi-kelas dengan baik tanpa memerlukan model yang besar.

Studi lain yang relevan mengenai deployment Edge Impulse pada ESP32 (untuk pengenalan suara, bukan gestur tangan) melaporkan akurasi 87,14% dan latensi 266 ms menggunakan fitur MFCC dan TFLite Micro [4] — memberikan gambaran pembanding lintas-modalitas (audio vs. gerak) untuk alur kerja Edge Impulse pada keluarga ESP32. Selain itu, riset mengenai *smart glove* berbasis sensor tekanan kain dengan inferensi on-device di ESP32-S3 [5] serta serangkaian tutorial praktis TinyML pada varian ESP32-S3 (XIAO ESP32S3 Sense) menggunakan Edge Impulse Studio [6] memperkuat bahwa kombinasi ESP32-S3 dan Edge Impulse merupakan pasangan platform yang sudah banyak divalidasi komunitas maupun akademik untuk kasus wearable berbasis sensor gerak.

---

## 4. Karakteristik Sensor LIS2DH dan Kesesuaiannya untuk Deteksi Gestur

LIS2DH adalah accelerometer tiga sumbu ultra-low-power dari keluarga "femto" STMicroelectronics, dengan antarmuka digital I2C/SPI, full-scale yang dapat dipilih pengguna (±2g/±4g/±8g/±16g), serta output data rate (ODR) yang dapat diatur dari 1 Hz hingga 5,3 kHz [7]. Sensor ini juga memiliki kemampuan self-test dan dapat dikonfigurasi untuk menghasilkan sinyal interrupt berbasis peristiwa wake-up/free-fall secara mandiri di level hardware [7] — sebuah fitur yang berpotensi dimanfaatkan sebagai mekanisme trigger tambahan yang hemat daya, di luar jalur inferensi ML utama.

Bukti empiris mengenai kelayakan pendekatan "satu accelerometer di pergelangan tangan + classifier ringan" untuk gestur ditemukan pada platform OpenHealth, yang melaporkan akurasi 98,6% dalam mengenali empat gestur (atas, bawah, kiri, kanan) menggunakan neural network sederhana dengan konsumsi daya aktif hanya sekitar 10 mW [8]. Marqués dan Basterretxea juga membahas algoritma efisien untuk sistem pengenalan gestur tangan wearable berbasis accelerometer yang dioptimalkan untuk perangkat tertanam dengan sumber daya terbatas [9]. Kedua studi ini mendukung asumsi desain proyek bahwa accelerometer tunggal — tanpa gyroscope tambahan — cukup memadai untuk membedakan gestur tangan yang dirancang secara sengaja (True1/True2) dari gerakan sehari-hari (Unknown), selama pola gestur dirancang cukup berbeda secara kinematik.

---

## 5. Klasifikasi Gestur & Deteksi "Distress" Berbasis Accelerometer: Tinjauan Penelitian Terkait

Karena True1 pada proyek ini merepresentasikan kondisi darurat, literatur mengenai deteksi jatuh (*fall detection*) berbasis accelerometer — yang secara konseptual serupa (mendeteksi peristiwa kritis dari pola akselerasi) — menjadi rujukan penting meskipun sensor pada studi-studi tersebut umumnya dipasang di pinggang/dada, bukan di tangan.

Alves dkk. mengusulkan algoritma accelerometer tunggal yang dapat bekerja pada tiga lokasi pemasangan berbeda (dada, pinggang, saku) tanpa memerlukan kalibrasi, dan melalui studi trade-off antara sensitivitas dan false alarm menemukan bahwa sampling rate 50 Hz memberikan performa paling sesuai [10]. Hussain dkk. melaporkan akurasi 99,98% menggunakan classifier Support Vector Machine (SVM) pada dataset publik SisFall untuk deteksi jatuh [11]. Kraft dkk. secara spesifik meninjau deteksi jatuh berbasis accelerometer yang dipasang di pergelangan tangan menggunakan deep learning [12], dan Rescio dkk. menunjukkan skema machine learning terawasi yang invarian terhadap usia, berat, tinggi badan, serta posisi pemasangan sensor [13]. Secara kolektif, literatur ini menegaskan bahwa pendekatan single-accelerometer dengan model ML ringan merupakan pendekatan yang matang dan telah tervalidasi secara luas untuk mendeteksi peristiwa kritis pada populasi lansia — memberi dasar yang kuat bagi desain True1 pada proyek ini.

Pada sisi kebutuhan non-darurat (True2) dan konsep komunikasi berbasis gestur secara umum, beberapa proyek wearable assistive relevan ditemukan: *iGest*, yang memetakan gerakan tangan (misalnya rotasi pergelangan) menjadi kalimat yang telah ditentukan sebelumnya untuk membantu individu dengan gangguan bicara dan motorik [18]; proyek *Smart Glove for Speech Impairment Communication*, yang memakai kombinasi flex sensor dan MPU6050 untuk menerjemahkan gestur menjadi output teks dan suara [17]; serta penelitian terbaru mengenai perangkat komunikasi asistif untuk lansia dengan afasia pasca-stroke [19]. Ketiganya memperkuat validitas pendekatan "gestur → keluaran suara" sebagai jalur komunikasi alternatif bagi pengguna dengan keterbatasan, sejalan dengan tujuan proyek ini.

---

## 6. Edge Impulse: Tinjauan Teknis Pipeline Pengembangan

Edge Impulse merupakan platform pengembangan TinyML yang menyediakan alur kerja end-to-end: pengumpulan data langsung dari perangkat, pemrosesan sinyal digital (DSP), pelatihan model, hingga deployment ke firmware mikrokontroler [14]. Untuk data accelerometer, blok DSP yang direkomendasikan adalah *Spectral Analysis/Spectral Features*, yang dirancang khusus untuk menganalisis gerakan repetitif dengan mengekstraksi karakteristik frekuensi dan daya sinyal menggunakan FFT [15], [16].

Dua prinsip metodologis penting dari literatur teknis Edge Impulse yang akan menjadi acuan pada tahap pengumpulan data (Minggu 3–4):

1. **Aturan sampling rate:** frekuensi sampling harus minimal dua kali frekuensi maksimum yang relevan dalam sinyal (prinsip Nyquist), agar dinamika gerakan yang diteliti tertangkap dengan baik [15].
2. **Windowing:** data time-series accelerometer perlu disegmentasi menggunakan sliding window sebelum ekstraksi fitur; salah satu proyek referensi publik Edge Impulse menggunakan window 2 detik dengan sampling rate 62,5 Hz untuk klasifikasi gerakan multi-kelas [16].

Nilai-nilai ini akan menjadi titik awal (baseline) yang disesuaikan dengan karakteristik ODR LIS2DH pada tahap eksperimen selanjutnya, alih-alih ditentukan secara sembarangan.

---

## 7. Sintesis dan Posisi Proyek

| Aspek | Temuan Literatur | Implikasi bagi Proyek |

| Platform MCU | ESP32-S3 terbukti mampu menjalankan TFLite Micro untuk klasifikasi gestur real-time dengan akurasi >90% dan latensi <200 ms [3] | Target performa proyek (latensi <200 ms, akurasi ≥90%) realistis dan sejalan dengan hasil penelitian sejenis |
| Sensor | LIS2DH: I2C, ODR hingga 5,3 kHz, hardware wake-up/free-fall interrupt [7] | Sensor tunggal cukup untuk 3 kelas gestur; fitur interrupt hardware bisa jadi pengembangan lanjutan hemat daya |
| Algoritma gestur | Akurasi 93–99% dicapai dengan classifier ringan (NN/CNN kecil, SVM) pada accelerometer tunggal [3],[8],[9],[11] | Model klasifikasi ringan (bukan deep network besar) sudah cukup untuk 3 kelas True1/True2/Unknown |
| Deteksi kondisi kritis | Fall detection accelerometer tervalidasi luas, termasuk pada posisi pergelangan tangan [10],[12],[13] | Mendukung validitas desain True1 sebagai sinyal darurat berbasis pola akselerasi |
| Komunikasi asistif | Proyek gestur-ke-suara untuk populasi dengan keterbatasan komunikasi telah banyak dieksplorasi [17],[18],[19] | Menegaskan relevansi sosial dan kelayakan teknis dari pendekatan "gestur → output suara" untuk lansia |
| Pipeline pengembangan | Edge Impulse menyediakan blok Spectral Features khusus data accelerometer, dengan aturan sampling & windowing yang jelas [14],[15],[16] | Menjadi acuan metodologis langsung untuk protokol pengumpulan data pada Minggu 3–4 |

Sejauh penelusuran literatur ini, kombinasi spesifik yang diusung proyek — accelerometer tunggal (LIS2DH) pada ESP32-S3 untuk membedakan gestur darurat vs. non-darurat vs. gerakan normal, dipadukan dengan keluaran audio otomatis yang ditujukan khusus bagi populasi lansia — belum ditemukan sebagai satu kesatuan solusi di literatur yang ditelusuri. Sebagian besar karya sejenis berfokus pada salah satu dari: (a) deteksi jatuh saja, (b) pengenalan gestur generik tanpa konteks urgensi, atau (c) komunikasi asistif untuk gangguan bicara tanpa pembedaan tingkat urgensi. Proyek ini memposisikan diri sebagai kombinasi dari ketiga elemen tersebut dalam satu perangkat sederhana dan hemat biaya.

---

## 8. Kesimpulan Studi Literatur

Tinjauan literatur ini memberikan dasar yang cukup kuat bahwa: (1) ESP32-S3 adalah platform yang layak dan telah tervalidasi untuk inferensi TinyML klasifikasi gestur secara real-time; (2) LIS2DH sebagai accelerometer tunggal cukup memadai untuk tugas klasifikasi 3-kelas yang diusung proyek ini, sejalan dengan berbagai studi gestur dan fall-detection sejenis; (3) Edge Impulse menyediakan pipeline dan prinsip metodologis (sampling rate, windowing, Spectral Features block) yang akan menjadi acuan langsung pada tahap pengumpulan data di minggu-minggu berikutnya; dan (4) proyek ini memiliki celah kontribusi yang jelas dibanding literatur yang ada, yaitu memadukan klasifikasi urgensi gestur dengan keluaran audio yang ditujukan khusus untuk konteks bantuan lansia.

---

## Daftar Pustaka

[1] M. Z. H. Zim, "TinyML: Analysis of Xtensa LX6 Microprocessor for Neural Network Applications by ESP32 SoC," *arXiv preprint* arXiv:2106.10652, 2021. https://arxiv.org/pdf/2106.10652

[2] "ArrythML: An Autoencoder-Based TinyML Approach for On-Device Arrhythmia Detection on Resource-Constrained Embedded Systems," *arXiv preprint* arXiv:2606.02256. https://arxiv.org/pdf/2606.02256

[3] "A Generalized TinyML Workflow for Energy-Efficient Hand Gesture Recognition on ESP32S3," *IEEE Xplore*. https://ieeexplore.ieee.org/document/11382541/

[4] "Deploying Real-Time Speech Recognition on ESP32 Using TinyML and Edge Impulse," in *Springer Lecture Notes*. https://link.springer.com/chapter/10.1007/978-3-031-97907-1_17

[5] "Thin Fabric Pressure Sensors and TinyML Smart Gloves for Edge IoT," *PMC*. https://www.ncbi.nlm.nih.gov/pmc/articles/PMC13453505/

[6] M. R. (mjrobot), "TinyML Made Easy: Object Detection with XIAO ESP32S3 Sense," *Hackster.io*. https://www.hackster.io/mjrobot/tinyml-made-easy-object-detection-with-xiao-esp32s3-sense-6be28d

[7] "Content for LIS2DH" (ringkasan datasheet), *jpralves.net*. https://jpralves.net/tag/lis2dh.html

[8] "OpenHealth: Open Source Platform for Wearable Health Monitoring," *arXiv preprint* arXiv:1903.03168. https://arxiv.org/pdf/1903.03168

[9] G. Marqués and K. Basterretxea, "Efficient Algorithms for Accelerometer-Based Wearable Hand Gesture Recognition Systems," in *Proc. IEEE/IFIP 13th Int. Conf. on Embedded and Ubiquitous Computing (EUC)*, 2015, pp. 132–139.

[10] J. Alves, J. Silva, E. Grifo, C. Resende, and I. Sousa, "Wearable Embedded Intelligence for Detection of Falls Independently of on-Body Location," *Sensors*, vol. 19, no. 11, p. 2426, 2019. doi:10.3390/s19112426

[11] F. Hussain, M. B. Umair, M. Ehatisham-ul-Haq, I. M. Pires, T. Valente, N. M. Garcia, and N. Pombo, "An Efficient Machine Learning-based Elderly Fall Detection Algorithm," *arXiv preprint* arXiv:1911.11976.

[12] D. Kraft, K. Srinivasan, and G. Bieber, "Wrist-worn Accelerometer Based Fall Detection for Embedded Systems and IoT Devices Using Deep Learning Algorithms," in *Proc. Int. Conf. on PErvasive Technologies Related to Assistive Environments (PETRA)*, 2020.

[13] G. Rescio, A. Leone, and P. Siciliano, "Supervised Machine Learning Scheme for Wearable Accelerometer-Based Fall Detector," 2014.

[14] Edge Impulse, "Classifying Movements in Wearables Using 3-Axis Accelerometers and Machine Learning," *Edge Impulse Blog*. https://edgeimpulse.com/blog/classifying-movements-in-wearables-using-3-axis-accelerometers-and-machine-learning/

[15] M. R. (mjrobot), "TinyML Under the Hood: Spectral Analysis," *jpralves.net*. https://jpralves.net/post/2023/03/23/tinyml-under-the-hood-spectral-analysis.html

[16] "DSP Spectral Features," Edge Impulse Studio documentation via *mlsysbook.ai*. https://mlsysbook.ai/contents/labs/shared/dsp_spectral_features_block/dsp_spectral_features_block

[17] "Smart Glove for Speech Impairment Communication," laporan proyek, *Scribd*. https://www.scribd.com/document/640226987/Untitled

[18] "iGest," direktori alat bantu asistif, *Punarbhava*. https://punarbhava.in/index.php/assistive-devices/speech-impairment/13-speech-impairment/5-igest

[19] "Toward Harmonized Human–Machine Interaction: Assistive Communication for Elderly with Aphasia," *Open Access CMS Conferences*. https://openaccess.cms-conferences.org/publications/book/978-1-964867-99-1/article/978-1-964867-99-1_14

