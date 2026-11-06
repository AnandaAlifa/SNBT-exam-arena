#include "demo_questions.h"

static const char PU_PASSAGE[] =
    "Literasi digital sering dipahami secara sempit sebagai kemampuan mengoperasikan "
    "perangkat dan aplikasi. Padahal, kompetensi itu baru sebagian dari yang dibutuhkan. "
    "Literasi digital mencakup kemampuan memahami, mengevaluasi, dan menggunakan informasi "
    "secara kritis. Tanpa kemampuan evaluasi, pengguna mudah terjebak pada informasi yang "
    "menyesatkan.\n\n"
    "Persoalannya, arus informasi di media sosial bergerak sangat cepat. Konten yang memancing "
    "emosi cenderung lebih banyak dibagikan daripada konten yang akurat. Algoritma platform "
    "turut memperkuat kecenderungan ini karena memberi ruang lebih besar pada konten yang "
    "mampu menahan perhatian. Akibatnya, kebenaran sering kalah cepat oleh sensasi.\n\n"
    "Beberapa pihak menilai pembatasan konten adalah solusi utama. Namun, pendekatan itu tidak "
    "cukup. Pembatasan dapat mengurangi penyebaran hoaks, tetapi tidak otomatis membentuk "
    "kemampuan berpikir kritis. Jika pengguna hanya menerima konten yang sudah disaring, mereka "
    "tidak terlatih menilai informasi secara mandiri.\n\n"
    "Karena itu, pendidikan literasi digital perlu diperkuat. Sekolah dapat membiasakan siswa "
    "memeriksa sumber, membandingkan beberapa rujukan, dan memisahkan fakta dari opini. "
    "Kebiasaan ini tidak hanya membantu siswa menghindari hoaks, tetapi juga membentuk warga "
    "yang mampu mengambil keputusan berdasarkan bukti.\n\n"
    "Di sisi lain, platform digital perlu lebih transparan mengenai cara kerja algoritmanya. "
    "Transparansi memungkinkan pengguna memahami mengapa suatu konten muncul. Dengan begitu, "
    "pengguna tidak sepenuhnya bergantung pada penyaringan platform dan dapat mengambil peran "
    "aktif dalam menjaga kualitas ruang informasi.";

static const Question PU_QUESTIONS[] = {
    {
        PU_PASSAGE,
        "Ide pokok bacaan tersebut adalah ...",
        {
            "Pembatasan konten oleh platform adalah satu-satunya solusi untuk memberantas hoaks.",
            "Literasi digital perlu diperkuat melalui pendidikan dan transparansi platform.",
            "Algoritma media sosial selalu menyebarkan konten yang tidak akurat.",
            "Pengguna tidak perlu memeriksa sumber karena platform telah menyaring konten.",
            "Pendidikan literasi digital hanya menjadi tanggung jawab sekolah."
        },
        1,
        "Jawaban: B. Bacaan menekankan penguatan literasi digital melalui pendidikan dan transparansi platform."
    },
    {
        PU_PASSAGE,
        "Berdasarkan bacaan, mengapa pembatasan konten tidak cukup untuk mengatasi hoaks?",
        {
            "Pembatasan konten selalu melanggar kebebasan berekspresi.",
            "Pembatasan konten tidak didukung oleh algoritma platform.",
            "Pembatasan konten tidak melatih pengguna menilai informasi secara mandiri.",
            "Pembatasan konten hanya dilakukan oleh sekolah.",
            "Pembatasan konten membuat informasi bergerak lebih cepat."
        },
        2,
        "Jawaban: C. Paragraf ketiga menyatakan pembatasan tidak otomatis membentuk kemampuan berpikir kritis."
    },
    {
        PU_PASSAGE,
        "Pernyataan yang sesuai dengan bacaan adalah ...",
        {
            "Konten yang akurat selalu lebih banyak dibagikan daripada konten yang memancing emosi.",
            "Transparansi algoritma membuat pengguna tidak perlu berpikir kritis.",
            "Pendidikan literasi digital dapat membiasakan siswa membandingkan beberapa rujukan.",
            "Platform digital menolak menjelaskan cara kerja algoritmanya.",
            "Literasi digital hanya berarti kemampuan mengoperasikan aplikasi."
        },
        2,
        "Jawaban: C. Paragraf keempat menyebut kebiasaan membandingkan beberapa rujukan."
    }
};

static const char PPU_PASSAGE[] =
    "Batik telah diakui UNESCO sebagai Warisan Budaya Takbenda pada 2009. Pengakuan itu tidak "
    "hanya menegaskan nilai estetika batik, tetapi juga mengakui proses panjang pewarisan "
    "pengetahuan dari generasi ke generasi. Setiap motif batik sering mengandung makna simbolis "
    "yang berkaitan dengan nilai sosial, lingkungan, atau kepercayaan masyarakat pembuatnya. Di "
    "beberapa daerah, motif tertentu bahkan tidak boleh digunakan oleh sembarang orang karena "
    "terkait dengan status adat. Oleh karena itu, pelestarian batik tidak cukup dilakukan dengan "
    "menjadikannya komoditas pariwisata; pemahaman atas makna dan konteks budaya perlu terus "
    "diwariskan.";

static const Question PPU_QUESTIONS[] = {
    {
        PPU_PASSAGE,
        "Kata \"komoditas\" dalam bacaan tersebut paling dekat maknanya dengan ...",
        {
            "barang dagangan",
            "warisan budaya",
            "karya seni",
            "simbol adat",
            "pengetahuan"
        },
        0,
        "Jawaban: A. Komoditas berarti barang dagangan atau sesuatu yang diperdagangkan."
    },
    {
        PPU_PASSAGE,
        "Pernyataan yang tidak sesuai dengan bacaan adalah ...",
        {
            "UNESCO mengakui batik sebagai Warisan Budaya Takbenda pada 2009.",
            "Motif batik dapat mengandung makna simbolis.",
            "Semua motif batik boleh digunakan oleh siapa saja.",
            "Pelestarian batik memerlukan pemahaman konteks budaya.",
            "Pengetahuan membatik diwariskan antargenerasi."
        },
        2,
        "Jawaban: C. Bacaan menyebut motif tertentu tidak boleh digunakan sembarang orang."
    }
};

static const char PBM_PASSAGE[] =
    "Perkembangan teknologi informasi telah mengubah cara masyarakat berkomunikasi. Kehadiran "
    "media sosial membuat aktifitas berinteraksi dapat dilakukan tanpa dibatasi jarak dan waktu. "
    "Namun, kemudahan tersebut juga membawa tantangan, yaitu meningkatnya penyebaran informasi "
    "yang tidak terverifikasi. Oleh sebab itu, pengguna perlu bersikap kritis terhadap setiap "
    "informasi yang diterima.";

static const Question PBM_QUESTIONS[] = {
    {
        PBM_PASSAGE,
        "Kalimat utama bacaan tersebut terdapat pada ...",
        {
            "kalimat pertama",
            "kalimat kedua",
            "kalimat ketiga",
            "kalimat keempat",
            "seluruh kalimat"
        },
        0,
        "Jawaban: A. Kalimat pertama memuat gagasan umum tentang perubahan cara berkomunikasi."
    },
    {
        PBM_PASSAGE,
        "Kata tidak baku dalam bacaan tersebut adalah ...",
        {
            "aktifitas",
            "teknologi",
            "informasi",
            "masyarakat",
            "komunikasi"
        },
        0,
        "Jawaban: A. Bentuk baku yang benar adalah \"aktivitas\"."
    }
};

static const char PK_PASSAGE[] =
    "Sebuah koperasi sekolah menjual tiga jenis minuman. Harga satu botol air mineral Rp4.000, "
    "satu kotak susu Rp6.000, dan satu botol jus Rp8.000. Pada hari Senin, koperasi menjual 25 "
    "botol air mineral, 20 kotak susu, dan 12 botol jus. Pada hari Selasa, penjualan air mineral "
    "naik 20%, penjualan susu turun 10%, dan penjualan jus sama dengan hari Senin.";

static const Question PK_QUESTIONS[] = {
    {
        PK_PASSAGE,
        "Total pendapatan koperasi pada hari Senin adalah ...",
        {
            "Rp292.000",
            "Rp304.000",
            "Rp316.000",
            "Rp328.000",
            "Rp340.000"
        },
        2,
        "Jawaban: C. (25 x 4.000) + (20 x 6.000) + (12 x 8.000) = 316.000."
    },
    {
        PK_PASSAGE,
        "Selisih pendapatan hari Selasa dan hari Senin adalah ...",
        {
            "Rp6.000",
            "Rp8.000",
            "Rp10.000",
            "Rp12.000",
            "Rp14.000"
        },
        1,
        "Jawaban: B. Pendapatan Selasa 324.000; selisihnya 324.000 - 316.000 = 8.000."
    }
};

static const char LBI_PASSAGE[] =
    "Penggunaan plastik sekali pakai telah menjadi persoalan lingkungan yang serius. Plastik "
    "membutuhkan waktu sangat lama untuk terurai, sementara konsumsinya terus meningkat. Beberapa "
    "negara telah menerapkan larangan penggunaan kantong plastik di pusat perbelanjaan. Kebijakan "
    "itu menuai pro dan kontra. Pihak yang mendukung menilai larangan mampu mengurangi sampah "
    "secara signifikan. Pihak yang menolak berpendapat bahwa larangan dapat memberatkan pelaku "
    "usaha kecil jika tidak disertai alternatif yang terjangkau. Menurut saya, larangan perlu "
    "tetap dijalankan, tetapi harus disertai penyediaan alternatif dan edukasi kepada masyarakat.";

static const Question LBI_QUESTIONS[] = {
    {
        LBI_PASSAGE,
        "Gagasan utama penulis dalam bacaan tersebut adalah ...",
        {
            "Larangan plastik sekali pakai harus dihentikan karena memberatkan pelaku usaha.",
            "Larangan plastik perlu dijalankan dengan dukungan alternatif dan edukasi.",
            "Plastik sekali pakai tidak dapat terurai dalam waktu singkat.",
            "Semua negara telah melarang penggunaan kantong plastik.",
            "Pelaku usaha kecil menolak semua kebijakan lingkungan."
        },
        1,
        "Jawaban: B. Penulis menegaskan larangan tetap dijalankan dengan alternatif dan edukasi."
    },
    {
        LBI_PASSAGE,
        "Pernyataan yang merupakan opini penulis adalah ...",
        {
            "Plastik membutuhkan waktu lama untuk terurai.",
            "Konsumsi plastik terus meningkat.",
            "Beberapa negara melarang kantong plastik di pusat perbelanjaan.",
            "Larangan plastik perlu disertai alternatif dan edukasi.",
            "Kebijakan larangan plastik menuai pro dan kontra."
        },
        3,
        "Jawaban: D. Kalimat tersebut ditandai frasa \"Menurut saya\" sebagai opini penulis."
    }
};

static const char LBE_PASSAGE[] =
    "Many students believe that studying longer hours automatically leads to better grades. "
    "However, research on learning suggests that the quality of study time matters more than the "
    "quantity. Active recall, spacing practice sessions, and testing oneself are often more "
    "effective than rereading notes. These methods force the brain to retrieve information, "
    "strengthening memory. Nevertheless, students often prefer rereading because it feels easier "
    "and creates a false sense of mastery. Educators therefore recommend that students combine "
    "effective strategies with regular breaks and sufficient sleep.";

static const Question LBE_QUESTIONS[] = {
    {
        LBE_PASSAGE,
        "What is the main idea of the passage?",
        {
            "Students should study as many hours as possible.",
            "Rereading notes is the most effective learning method.",
            "The quality of study methods matters more than study duration.",
            "Sleep has no effect on academic performance.",
            "Active recall is only useful for science subjects."
        },
        2,
        "Answer: C. The passage contrasts study quality with study quantity."
    },
    {
        LBE_PASSAGE,
        "According to the passage, why do students often prefer rereading?",
        {
            "It strengthens memory more than active recall.",
            "It feels easier and gives a false sense of mastery.",
            "It requires regular breaks and sufficient sleep.",
            "It forces the brain to retrieve information.",
            "It is recommended by all educators."
        },
        1,
        "Answer: B. The passage states that rereading feels easier and creates a false sense of mastery."
    }
};

static const char PM_PASSAGE[] =
    "Sebuah tangki air berbentuk balok memiliki panjang 2 m, lebar 1,5 m, dan tinggi 1 m. Tangki "
    "diisi air hingga 80% kapasitasnya. Air tersebut digunakan untuk menyiram tanaman sebanyak "
    "600 liter. (1 m^3 = 1.000 liter)";

static const Question PM_QUESTIONS[] = {
    {
        PM_PASSAGE,
        "Volume air dalam tangki sebelum digunakan adalah ...",
        {
            "1.800 liter",
            "2.100 liter",
            "2.400 liter",
            "2.700 liter",
            "3.000 liter"
        },
        2,
        "Jawaban: C. Volume tangki 3 m^3 = 3.000 liter; 80% x 3.000 = 2.400 liter."
    },
    {
        PM_PASSAGE,
        "Setelah digunakan, tinggi air dalam tangki adalah ...",
        {
            "0,3 m",
            "0,4 m",
            "0,5 m",
            "0,6 m",
            "0,8 m"
        },
        3,
        "Jawaban: D. Sisa air 1.800 liter = 1,8 m^3; tinggi = 1,8 / (2 x 1,5) = 0,6 m."
    }
};

const Question *const DEMO_QUESTIONS[SUBTEST_COUNT] = {
    PU_QUESTIONS,
    PPU_QUESTIONS,
    PBM_QUESTIONS,
    PK_QUESTIONS,
    LBI_QUESTIONS,
    LBE_QUESTIONS,
    PM_QUESTIONS
};

const int DEMO_QUESTION_COUNTS[SUBTEST_COUNT] = {
    (int)(sizeof(PU_QUESTIONS) / sizeof(PU_QUESTIONS[0])),
    (int)(sizeof(PPU_QUESTIONS) / sizeof(PPU_QUESTIONS[0])),
    (int)(sizeof(PBM_QUESTIONS) / sizeof(PBM_QUESTIONS[0])),
    (int)(sizeof(PK_QUESTIONS) / sizeof(PK_QUESTIONS[0])),
    (int)(sizeof(LBI_QUESTIONS) / sizeof(LBI_QUESTIONS[0])),
    (int)(sizeof(LBE_QUESTIONS) / sizeof(LBE_QUESTIONS[0])),
    (int)(sizeof(PM_QUESTIONS) / sizeof(PM_QUESTIONS[0]))
};
