# TLOG Viewer

MAVLink v2 protokolüyle kaydedilmiş `.tlog` uçuş kayıt dosyalarını parse eden ve görselleştiren Qt6/QML masaüstü uygulaması.

---

## Özellikler

- `.tlog` dosyasını binary olarak okur, MAVLink v2 paketlerini parse eder
- Uçuş rotasını **OpenStreetMap** üzerinde 2D harita olarak gösterir (API key gerekmez)
- Tüm mesajları zaman damgalı ve okunabilir biçimde **Ham Veri** sekmesinde listeler
- Aktif mesajı vurgular, tıklamayla timeline'a atlanabilir
- **Timeline / medya kontrolü**: play/pause, 1×/5×/10×/50×/100× hız, slider
- Sekmeler arası geçişte timeline konumu korunur

---

## Gereksinimler

| Bileşen | Minimum Versiyon |
|---------|-----------------|
| C++ standardı | C++17 |
| CMake | 3.16 |
| Qt | 6.5 |

**Gerekli Qt modülleri:** `Core`, `Gui`, `Quick`, `Qml`, `Location`, `Positioning`, `QuickControls2`

---

## Build Talimatları

### Linux

```bash
# 1. Kaynak dizine gir
cd tlog-viewer

# 2. Build dizini oluştur
mkdir build && cd build

# 3. CMake yapılandır (Qt kurulum yolunu belirt)
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 ..

# 4. Derle
cmake --build . -j$(nproc)
```

Qt sistem PATH'inde kuruluysa `-DCMAKE_PREFIX_PATH` parametresini atlayabilirsiniz.

### Çalıştırma (Linux, X11)

```bash
DISPLAY=:1 ./tlog-viewer
```

Wayland ortamında `DISPLAY` değişkeni gerekmez, doğrudan çalıştırabilirsiniz:

```bash
./tlog-viewer
```

### Windows / macOS

CMake yapılandırmasında Qt kurulum yolunu belirtin:

```bash
# Windows
cmake -DCMAKE_PREFIX_PATH=C:/Qt/6.x.x/msvc2019_64 ..

# macOS
cmake -DCMAKE_PREFIX_PATH=~/Qt/6.x.x/macos ..
```

---

## Proje Yapısı

```
tlog-viewer/
├── CMakeLists.txt              # Build sistemi
├── README.md
├── src/
│   ├── main.cpp                # Uygulama giriş noktası
│   ├── MavlinkMessage.h        # Tek mesajı temsil eden struct
│   ├── TlogParser.h/.cpp       # .tlog binary parser
│   ├── MessageListModel.h/.cpp # QAbstractListModel (QML ListView)
│   └── AppController.h/.cpp   # C++ ↔ QML köprüsü
├── qml/
│   ├── main.qml                # ApplicationWindow + StackView
│   ├── WelcomePage.qml         # Dosya yükleme ekranı
│   ├── MainPage.qml            # Sekmeli ana ekran
│   ├── MapView.qml             # OSM harita görünümü
│   ├── RawDataView.qml         # Ham mesaj listesi
│   └── TimelineControl.qml    # Medya kontrolü (alt bar)
└── tests/
    ├── CMakeLists.txt          # Test build tanımı
    ├── helpers/
    │   └── TlogBuilder.h       # .tlog binary oluşturucu (test fixture helper)
    ├── tst_tlogparser.cpp      # TlogParser unit testleri
    ├── tst_messagelistmodel.cpp# MessageListModel unit testleri
    └── tst_appcontroller.cpp   # AppController unit testleri
```

---

## Mimari Kararlar

### Neden Qt6 / QML?

QML'in reaktif binding sistemi, timeline slider'ı ile harita ve liste gibi birden fazla UI bileşenini tek bir `currentIndex` property üzerinden senkronize etmeyi kolaylaştırır. C++ tarafı veri işleme ve parse görevlerini, QML tarafı ise saf UI görevlerini üstlenir; bu ayrım kodun bakımını kolaylaştırır.

### Neden harici MAVLink kütüphanesi yok?

Proje, yalnızca desteklenen mesaj ID'lerini parse etmek üzere MAVLink v2 wire formatını doğrudan implement eder. Bu yaklaşım derleme süresini kısaltır, dış bağımlılık gerektirmez ve `.tlog` formatına özgü 8-byte big-endian timestamp ön ekini kolayca ele alır.

### Harita için neden Qt Location (OSM)?

API key gerektirmez, `tile.openstreetmap.org` altyapısını kullanır ve Qt'ye native entegre olduğu için ek bağımlılık yoktur.

### Performans: mesafe tabanlı örnekleme

Harita polyline'ı için ardışık GPS noktaları arasında en az 1 metrelik mesafe koşulu aranır. Bu yöntem dosya boyutundan bağımsız çalışır ve coğrafi doğruluğu korurken render yükünü minimize eder.

### posCache

`AppController`, her mesaj indeksi için "o ana kadarki son bilinen GPS koordinatı"nı önbelleğe alır. İlk GPS fix'i bulunarak önceki tüm indeksler de aynı koordinata pre-seed edilir; böylece dosya yüklenir yüklenmez harita uçuş bölgesini gösterir.

---

## Desteklenen MAVLink Mesajları

| ID | Mesaj Adı |
|----|-----------|
| 0 | HEARTBEAT |
| 22 | PARAM_VALUE |
| 24 | GPS_RAW_INT |
| 30 | ATTITUDE |
| 31 | ATTITUDE_QUATERNION |
| 32 | LOCAL_POSITION_NED |
| 33 | GLOBAL_POSITION_INT |
| 36 | SERVO_OUTPUT_RAW |
| 74 | VFR_HUD |
| 83 | ATTITUDE_TARGET |
| 85 | POSITION_TARGET_LOCAL_NED |
| 141 | ALTITUDE |
| 147 | BATTERY_STATUS |
| 241 | VIBRATION |
| 253 | STATUSTEXT |
| Diğer | Ham hex (ilk 16 bayt) |

---

---

## Unit Testler

### Çalıştırma

```bash
cd build
cmake --build . -j$(nproc)  # testler de derlenir
ctest --output-on-failure    # tüm testleri çalıştır
```

Bireysel olarak çalıştırmak için:

```bash
./tests/tst_tlogparser
./tests/tst_messagelistmodel
./tests/tst_appcontroller
```

Ayrıntılı çıktı için:

```bash
ctest -V
# ya da:
./tests/tst_tlogparser -v2
```

### Test grubu: `tst_tlogparser` (10 test)

`TlogParser::parse()` fonksiyonunu doğrular. Testler `TlogBuilder` helper'ı ile hafızada MAVLink v2 binary paketleri üretip geçici dosyaya yazarak parser'ı gerçekçi girdilerle test eder.

| Test | Ne test edilir |
|------|---------------|
| `parse_emptyFile` | Boş dosya → boş liste + hata mesajı dolu |
| `parse_truncatedPacket` | Kırpılmış paket → parser gracefully durur, crash yok |
| `parse_junkPrefix_resyncs` | Başa 5 byte junk → parser 0xFD magic'i bulup senkrona girer |
| `parse_shortPayload_doesNotCrash` | 10 byte payload (28 beklenen) → `hasPosition=false`, boş `fieldsText`, crash yok |
| `parse_singleHeartbeat` | Tek HEARTBEAT → 1 mesaj, `msgName="HEARTBEAT"` |
| `parse_globalPositionInt_correctLatLonAlt` | lat/lon 1e-7 ölçeği + alt mm→m dönüşümü doğruluğu |
| `parse_globalPositionInt_zeroLatLon` | lat=lon=0 → `hasPosition=false` |
| `parse_attitude_piRadians` | π rad → "180.00" (radyan→derece dönüşüm kontrolü) |
| `parse_statusText_severityLabels` | severity 0→`EMERGENCY`, 3→`ERROR`, 6→`INFO`, 7→`DEBUG` |
| `parse_unknownMsgId` | Bilinmeyen msgid → `MSG_XXXX` ismi, `fieldsText` "payload:" ile başlar |

### Test grubu: `tst_messagelistmodel` (10 test)

`MessageListModel` (QAbstractListModel türevi) davranışını doğrular.

| Test | Ne test edilir |
|------|---------------|
| `rowCount_freshModel_isZero` | Boş model → `rowCount() == 0` |
| `rowCount_afterSetMessages` | `setMessages(5)` → `rowCount() == 5` |
| `data_outOfBoundsRow_returnsNull` | Sınır dışı satır ve geçersiz index → boş `QVariant` |
| `data_rolesReturnCorrectValues` | `MsgNameRole`, `MsgIdRole`, `FieldsTextRole` doğru değer döner |
| `data_isActive_defaultFalseForAll` | Başlangıçta tüm satırlar `isActive=false` |
| `data_isActive_trueOnlyForCurrentIndex` | `setCurrentIndex(2)` → yalnızca satır 2 `true`; değişince eski `false` |
| `setCurrentIndex_emitsDataChanged_forBothOldAndNew` | Eski ve yeni satır için `dataChanged` emit edilir |
| `setCurrentIndex_doesNotEmitForEveryRow` | 100 satırlı modelde yalnızca 2 emit — **performans garantisi** |
| `setMessages_resetsCurrentIndex` | Yeni `setMessages` → `modelReset` emit, hiçbir satır aktif değil |
| `timeRole_relativeTimeFormattedCorrectly` | 90 saniye → `"01:30"` |

### Test grubu: `tst_appcontroller` (10 test)

`AppController` (C++↔QML köprüsü) state machine'ini doğrular. `QTemporaryFile` ile gerçek `.tlog` binary'si oluşturulur.

| Test | Ne test edilir |
|------|---------------|
| `initial_state_isClean` | `fileLoaded=false`, `messageCount=0`, lat/lon=0, zaman stringleri "00:00", `messageModel` null değil |
| `loadFile_nonexistentPath` | `false` döner, `loadError` sinyali emit, `fileLoaded` değişmez |
| `loadFile_validFile` | `true` döner, `fileLoaded=true`, `messageCount>0`, `fileLoadedChanged` + `messagesChanged` emit |
| `loadFile_acceptsFileUrlScheme` | `"file:///..."` URL formatı kabul edilir (QML FileDialog uyumluluğu) |
| `loadFile_secondLoad_resetsIndex` | İkinci dosya yüklenince `currentIndex` sıfırlanır |
| `setCurrentIndex_negative_clampsToZero` | `-5` → `0` |
| `setCurrentIndex_tooLarge_clampsToLast` | `999999` → `messageCount - 1` |
| `setCurrentIndex_sameValue_noSignal` | Aynı index → `currentIndexChanged` emit edilmez |
| `posCache_beforeGpsFix_seedsFromFirstFix` | Index 0 (GPS öncesi) → posCache, ilk GPS fix koordinatını döndürür |
| `endTimeStr_formattedAsMMSS` | 65 saniyelik dosya → `"01:05"` |


