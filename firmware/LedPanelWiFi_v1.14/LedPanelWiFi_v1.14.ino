// Гайд по постройке матрицы: https://alexgyver.ru/matrix_guide/
// Страница проекта на GitHub: https://github.com/vvip-68/LedPanelWiFi
// Автор идеи, начальный проект - GyverMatrixBT: AlexGyver Technologies, 2019 (https://alexgyver.ru/gyvermatrixbt/)
// Дальнейшее развитие: vvip-68, 2019-2023
//
// Дополнительные ссылки для Менеджера плат ESP8266 и ESP32 в Файл -> Настройки
// https://raw.githubusercontent.com/esp8266/esp8266.github.io/master/stable/package_esp8266com_index.json
// https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

#define FIRMWARE_VER F("WiFiPanel v.1.14a.2023.1218")

// --------------------------   -----------------------------------------------------------------------------
//
// Внимание!!!
// Рабочие комбинации версий ядра и библиотек:
//
// Версия FastLED      - 3.6.0  // с 3.5.0 также работает
// Версия ядра ESP8266 - 3.1.2
// Версия ядра ESP32   - 2.0.14 // Возможна установка 1.0.6; 
//                              // На версиях 2.x.x (кроме 2.0.14) скетчи может не собираться совсем или собирается, но
//                              // нет вывода на матрицу (до 2.0.6) или цифровые артефакты и подергивание эффектов и бегущей строки.
//
// -------------------------------------------------------------------------------------------------------
//
// Для ядра ESP8266 v3.1.2
//   тип микроконтроллера в меню "Инструменты -> Плата" для ESP8266:
//     - для устройств на базе NodeMCU выбирать       -- "NodeMCU 1.0 (ESP12E Module)"
//     - для устройств на базе Wemos d1 mini выбирать -- "LOLIN(WEMOS) D1 mini (clone)" 
//     - для выделения места под файловую систему в меню "Инструменты" Arduino IDE в настройке распределения памяти устройства
//       для стандарного контроллера с 4МБ флэш-памяти памяти на борту устройства выберите вариант: "Flash Size: 4MB(FS:2MB OTA:~1019KB)"
//
// Для ядра ESP32 v2.0.14 / v1.0.6 
//   тип микроконтроллера в меню "Инструменты -> Плата" 
//     - для большинства контроллеров выбирать "ESP32 Dev Module" 
//     - для разновидностей ESP32S2 выбирать "ESP32S2 Dev Module" или соответствующее плате значение, содержащее 'S2' 
//     - для разновидностей ESP32S3 выбирать "ESP32S3 Dev Module" или соответствующее плате значение, содержащее 'S3' 
//     - для разновидностей ESP32C3 выбирать "ESP32C3 Dev Module" или соответствующее плате значение, содержащее 'C3'
//
//   для выделения места под файловую систему в меню "Инструменты" Arduino IDE в настройке распределения памяти устройства
//       для стандарного контроллера с 4МБ флэш-памяти памяти на борту устройства выберите вариант: "Partition scheme: Default 4MB with spiff(1.2MB APP/1.5MB SPIFFS)";
//       Если включена поддкржка всех возможностий и компилятор ругается на недостаток памяти - придется отказаться от
//       возможности обновления "по воздуху" (OTA, Over The Air) и выбрать вариант распределения памяти устройства "Partition scheme: No OTA (2MB APP/2MB SPIFFS)";
//
//   Внимание!!! - если на версии ядра 1.0.6 при нажатии кнопки смены эффекта в Web-приложении вы получаете краш программы с сообщением в журнале либо "Stack overflow", либо "Heap corrupted"
//   перейдите в папку C:\Users\<user>\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\ и далее найдите файлы
//                     \cores\esp32\main.cpp, \tools\sdk\sdkconfig, \tools\sdk\include\config\sdkconfig.h
//   В каждом из этих файлов найдите строчку - параметр CONFIG_ARDUINO_LOOP_STACK_SIZE и измените значение по умолчанию 8192 на 16138, пересоберите проект с новым значением параметра     
// 
// -------------------------------------------------------------------------------------------------------
//
// Настройки ArduinoIDE 
//   для Wemos, NodeMCU и ESP32 смотри на скриншотов в корневой папке проекта - settings-wemos.png, settings-nodemcu.png и settings-esp32.png 
//
//   Внимание!!! 
//     Настройки сборки в меню "Инструменты" для Wemos приведены на указанном скриншоте испробованы и в большинстве случаев работают нормально.           
//     Однако мне встречались экземпляры, которые при данной настройке даже не запускались - в мониторе порта при старте контроллера только немного мусора.
//     Для таких плат выберите значение "Flash Mode: DIO". Если не запустится и с ним - выбирайте значение "Flash Mode: DOUT (compatible)"
//     Если и дальше будут проблемы - понижайте CPU Frequency со 160 MHz до 80 MHz
//     Пробуйте вариант "Flash frequency" понижать с 80 MHz до 40 MHz или 26 MHz
//     Вероятно с какими-то комбинациями этих настроек контроллер запустится
//
// -------------------------------------------------------------------------------------------------------
//
// *** MP3 DFPlayer
//
// Используйте версию библиотеки DFPlayer_Mini_Mp3_by_Makuna из папки libraries проекта - это версия 1.1.1
// Более новые версии этой библиотеки из предлагаемых в Library Manager of Arduino IDE (например, 1.2.0) - в отличии от 1.1.1 
// почему-то не находят файлов звуков на SD-карте - скетч при запуске скажет, что MP3 плеер недоступен.
//
// Будет ли работать MP3 DFPlayer - чистая лотерея, зависит от чипа и фаз луны
// В некоторых случаях может быть циклическая перезагрузка после подключения к сети,
// в некоторых случаях - перезагрузка микроконтроллера при попытке проиграть мелодию будильника из приложения
// Но, может и работать нормально.
//
// Вероятнее всего нестабильность работы плеера зависит от версии библиотеки SoftwareSerial, входящей в состав ядра.
// Используйте только рекомендованные выше проверенные версии ядра и библиотеки из папки libraries проекта.
//
// Другая вероятная причина возможных сбоев плеера - (не)стабильность или недостаточное (или завышенное) 
// напряжение питание платы плеера или недостаток тока, предоставляемого БП для питания плеера.
// По Datasheet напряжение питания DFPlayer - 4.2 вольта (с допустимым заявленным диапазоном 3.3В..5В), однако при использовании напряжения 
// 5 вольт или чуть выше - плеер работает нестабильно (зависит от чипа - как повезет).
//
// Общая рекомендация - питать всю систему от напряжения 4.8 вольта при необходимости используя подпирающий диод между GND блока питания (матрицы) 
// и пином GND микроконтроллера / DFPlayer`a
//
// -------------------------------------------------------------------------------------------------------
//
// *** Дополнительный индикатор TM1637
//
// Библиотеку TM1637 следует обязательно устанавливать из папки проекта. В ней сделаны исправления, позволяющие
// компилироваться проекту для микроконтроллера ESP32. Со стандартной версией библиотеки из менеджера библиотек
// проект не будет компилироваться.
//
// Внимание: на передачу информации в индикатор требуется довольно много времени. При включенной опции поддержки индикатора TM1637
// время выполнения одного цикла loop() примерно 75 миллисекунд, в то время как с выключенной поддержкой индикатора - один цикл loop()
// занимает всего 15 миллисекунд. То есть без индикатора эффекты и бегущая строка будет двигаться значительно быстрее чем при наличии индикатора.
//
// -------------------------------------------------------------------------------------------------------
//
// *** Бегущая строка - скорость движения
//
// В a_hard_def.h найдите параметр TEXT_SHIFT. Это значение - на сколько колонок сдвигать текст за один проход
// Скорость сдвига строки зависит от скорости вывода картинки на матьрицу. На больших матрицах требуется больше времени на вывод одного кадра
// Если вам кажется что на максимальной скорости строка все равно движется медленно - укажите параметр TEXT_SHIFT равным 2 мли 3. 
// Тогда каждый сдвиг текста будет выполняться сразу на 2 или три колонки. Текст будет двигаться быстрее, но визуально менее плавно
//
// --------------------------------------------------------
//
// *** Синхронизация устройств по протоколу E1.31
//
// Библиотеку ESPAsyncE131 следует обязательно устанавливать из папки проекта. В ней исправлены ошибки стандартной
// библиотеки (добавлен деструктор и освобождение выделяемых ресурсов), а также добавлен ряд функций, позволяющих
// осуществлять передачу сформированных пакетов в сеть. Со стандартной версией библиотеки из менеджера библиотек
// проект не будет компилироваться
//
// --------------------------------------------------------
//
// *** Web-интерфейс
//
// Библиотеку ESPAsyncWebServer владельцам IPhone следует обязательно устанавливать из папки проекта. 
// В ней в файле WebResponses.cpp в строчках 538, 569 закомментарено добавление заголовка addHeader("Content-Disposition", buf)
// Точнее перенесено в область if() else - для download=true.
// Почему-то наличие этого заголовка в айфонах не открывает запрошенную страничку в браузере, а предлагает мохранить
// загружаемый файл intex.html.gz на диск как обычный download- файл.
//
// Сама библиотека с хотфиксами взята отсюда: https://github.com/vortigont/ESPAsyncWebServer/tree/hotfixes
// Но в ней нет переноса addHeader("Content-Disposition", buf) что описано выше
//
// Прошивка требует компиляции с выделением места под файловую систему LittleFS, в которой хранятся файлы настроки подключения к сети,
// файлы резервного копирования настроек, файл карты индексов адресации светодиодов матрицы, файлы Web-интерфейва, а также файлы картинок, 
// нарисованные пользователем в режиме рисования и используемые в эффекте "Слайды"
//
// Для выделения места под файловую систему в меню "Инструменты" Arduino IDE в настройке распределения памяти устройства выберите вариант:
//   Для микроконтроллеров ESP8266 с 4МБ флэш-памяти рекомендуется вариант "Flash Size: 4MB(FS:2MB OTA:~1019KB)"
//   Для микроконтроллеров ESP32   с 4МБ флэш-памяти рекомендуется вариант "Partition scheme: Default 4MB with spiff(1.2MB APP/1.5MB SPIFFS)"; 
//
// После того, как прошивка будет загружена в микроконтроллер - не забудьте загрузить файлы из подпапки data в файловую систему микроконтроллера
// https://github.com/vvip-68/LedPanelWiFi/wiki/Загрузка-данных-в-файловую-систему-МК
//
// --------------------------------------------------------
//
// *** SD-карта
//
// Некоторые SD-shield требуют напряжения питания 5 вольт, некоторые - 3.3 вольта
// Если на SD-shield подать напряжение, не соответствующее его характеристикам - файлы с SD карты также будут не видны.
// При использовании "матрешки" из Wemos d1 mini и соотвествующего ей Shield SD-card рекомендается распсивать ОБА пина питания - и +5В и +3.3В 
//
// -------------------------------------------------------------------------------------------------------
//
// *** Настройка параметров прошивки под вашу конфигурацию оборудования
//
// Для начала работы выполните настройки параметров прошивки - укажите нужные значения в файлах 
//  a_def_hard.h - параметры вашей матрицы, наличие дополнительных модулей, пины подключения, использование возможностей прошивки
//  a_def_soft.h - параметры подулючения к сети - имя сети, пароль, временная зона, коды для получения погоды и т.д.
// Большинство параметров в последствии могут быть изменены в Web-интерфейсе при подключении к устройству или в приложении на смартфоне, 
// если таковое уже есть для текущей версии прошивки.
//
// -------------------------------------------------------------------------------------------------------
// Версии (что нового): 
// -------------------------------------------------------------------------------------------------------
//
// v1.14а - попытка оптимизировать работу с памятью. 
// v1.14  - добавлено управление через Web-интерфейс.
//          Android-приложения для настройки и управления устройством более несовместимы с этой версией и работать с ней не будут.
//          Управление устройством будет осуществляться только через Web-интерфейс.
//          Файлы web-интерфейса находятся в подпапке 'data/web' в папке с файлами скетча прошивки и загружаются в микроконтроллер отдельно
//          после компиляции и загрузки скетча. Загрузка фвйлов из папки 'data' выполняется плагином 'LittleFS DataUpload' для соответствующего 
//          типа микроконтроллера - ESP32 или ESP8266.
//
//           Прошивка стабильно работает на микроконтроллерах ESP32, предпочтительно собирать устройство на нем.
//           Микроконтроллер ESP8266 все еще поддерживается для матриц размером до 500-700 диодов, но устройство может перезагружаться при попытке
//           открытия Web-интерфейса в браузере из за нехватки оперативной памяти у этого типа микроконтроллеров.
//           По той же причине недостатка оперативной памяти Web-интерфейс может вообще не открываться - передача требуемых файлов Web-странички прерывается,
//           если микроконтроллеру недостаточно оперативной памяти. Проблемы начинаются, когда свободной оперативной памяти остается менее 12-15 килобайт.
//           Сколько памяти остается свободной - зависит от размера матрицы и включенных функций - MP3 Player, индикатор TM1637, поддержка E131 и т.д.
//
//          Управление через UDP канал прошивкой поддерживается, но в настоящий момент нет готового приложения, поддерживающего управления через UDP.
//          
//          Изменилась карта распределения хранения настроек в постоянной памяти EEPROM
//          После сборки и загрузки скомпилированной прошивки 1.14 вам придется заново перенастраивать все эффекты
//          и прочие настройки программы, в том числе набор текстов бегущей строки. Ввиду несовместимости расположения 
//          сохраненных настроек, их восстановление из файла резервной копии также недоступно или приведет к
//          сбоям в работе прошивки. Первая загрузка прошивки 1.14 поверх более ранней опции должна быть выполнена
//          с настройкой в меню "Инструменты" - "Erase Flash: Erase All"
//
// *******************************************************************************************************
// *                                              WIFI ПАНЕЛЬ                                            *
// *******************************************************************************************************

#include "a_def_hard.h"     // Определение параметров матрицы, пинов подключения и т.п
#include "a_def_soft.h"     // Определение параметров эффектов, переменных программы и т.п.

void setup() {
  #if defined(ESP8266)
    ESP.wdtEnable(WDTO_8S);
  #endif

  // allocate heap mem for buffer
  incomeBuffer = new char[BUF_MAX_SIZE];
  
  // Инициализация EEPROM и загрузка начальных значений переменных и параметров
  EEPROM.begin(EEPROM_MAX);
  delay(100);

  uint8_t eeprom_id = EEPROMread(0);

  // Загрузть настройки подключения пинов микроконтроллера к устройствам проекта
  bool isEEPROMInitialized = eeprom_id == EEPROM_OK;
  if (!isEEPROMInitialized) {
    clearEEPROM();
  }
  
  bool isWireInitialized = getWiringInitialized() && isEEPROMInitialized;
  if (!isWireInitialized) {
    initializeWiring();
  }
  loadWiring();

  if (vDEBUG_SERIAL) {
    Serial.begin(115200);
    delay(300);
  }

  // пинаем генератор случайных чисел
  #if defined(ESP8266) && defined(TRUE_RANDOM)
  uint32_t seed = (int)RANDOM_REG32;
  #else
  uint32_t seed = (int)(analogRead(0) ^ micros());
  #endif
  randomSeed(seed);
  random16_set_seed(seed);

  host_name = HOST_NAME;
  #ifdef DEVICE_ID
    host_name += '-';
    host_name += DEVICE_ID;
  #endif

  DEBUGLN();
  DEBUGLN(FIRMWARE_VER);
  
  DEBUG(F("Контроллер: '"));
  DEBUG(MCUType());
  DEBUGLN("'");

  {  
    #if defined(ESP32) && defined(ARDUINO_ESP32_RELEASE)
      String core_type(F("ESP32"));
      String core_version(ARDUINO_ESP32_RELEASE);
    #endif  
    #if defined(ESP8266) && defined(ARDUINO_ESP8266_RELEASE)
      String core_type(F("ESP8266"));
      String core_version(ARDUINO_ESP8266_RELEASE);
    #endif
    if (core_version.length() > 0) {
      core_version.replace("_", ".");
      DEBUG(F("Версия ядра: "));
      DEBUG(core_type);
      DEBUG(F(" v"));
      DEBUGLN(core_version);
    }
  }
  
  {
    String fv(FASTLED_VERSION);
    String fv_maj(fv.substring(1,4).toInt());
    String fv_min(fv.substring(4).toInt());
    DEBUG(F("FastLED: ")); DEBUG(fv[0]); DEBUG('.');  DEBUG(fv_maj); DEBUG('.'); DEBUG(fv_min);    
    DEBUGLN();
  }
  
  if (!isEEPROMInitialized) {
    // Сама инициализация выполняется выше по коду. Тут - просто вывод сообщения о событии
    DEBUGLN(F("Инициализация EEPROM..."));
  }  
  
  DEBUG(F("Версия EEPROM: 0x"));
  DEBUGLN(IntToHex(eeprom_id, 2));  
  if (eeprom_id != EEPROM_OK) {
    DEBUG(F("Обновлено до: 0x"));
    DEBUGLN(IntToHex(EEPROM_OK, 2));  
  }
  DEBUG(F("Host: '"));
  DEBUG(host_name);  
  DEBUGLN(F("\'\n\nИнициализация файловой системы... "));
  
  spiffs_ok = LittleFS.begin();
  if (!spiffs_ok) {
    DEBUGLN(F("Выполняется разметка файловой системы... "));
    LittleFS.format();
    spiffs_ok = LittleFS.begin();    
  }

  if (spiffs_ok) {
    #if defined(ESP32)
      spiffs_total_bytes = LittleFS.totalBytes();
      spiffs_used_bytes  = LittleFS.usedBytes();
      DEBUG(F("Использовано ")); DEBUG(spiffs_used_bytes); DEBUG(F(" из ")); DEBUG(spiffs_total_bytes); DEBUGLN(F(" байт"));
    #else
      FSInfo fs_info;
      if (LittleFS.info(fs_info)) {
        spiffs_total_bytes = fs_info.totalBytes;
        spiffs_used_bytes  = fs_info.usedBytes;
        DEBUG(F("Использовано ")); DEBUG(spiffs_used_bytes); DEBUG(F(" из ")); DEBUG(spiffs_total_bytes); DEBUGLN(F(" байт"));
      } else {
        DEBUGLN(F("Ошибка получения сведений о файловой системе."));
    }
    #endif
  } else {
    DEBUGLN(F("Файловая система недоступна."));
    DEBUGLN(F("Управление через Web-канал недоступно."));
    DEBUGLN(F("Функционал \"Бегущая строка\" недоступен."));
  }
  DEBUGLN();
 
  loadSettings();

  // -----------------------------------------  
  // Вывод основных возможностей: поддержка в прошивке - 
  // включена или выключена + некоторые параметры
  // -----------------------------------------  

  DEBUGLN();
  DEBUG(F("Матрица: "));
  if (vDEVICE_TYPE == 0) DEBUG(F("труба "));
  if (vDEVICE_TYPE == 1) DEBUG(F("плоская "));
  DEBUG(pWIDTH); DEBUG('x'); DEBUGLN(pHEIGHT);

  if (sMATRIX_TYPE == 2) {
    DEBUGLN(F("Aдресация: карта индексов"));
  } else {
    DEBUGLN(F("Адресация: по подключению"));
    DEBUG(F("  Угол: ")); 
    if (sCONNECTION_ANGLE == 0) { DEBUGLN(F("левый нижний")); } else
    if (sCONNECTION_ANGLE == 1) { DEBUGLN(F("левый верхний")); } else
    if (sCONNECTION_ANGLE == 2) { DEBUGLN(F("правый верхний")); } else
    if (sCONNECTION_ANGLE == 3) { DEBUGLN(F("правый нижний")); }
    DEBUG(F("  Направление: "));
    if (sSTRIP_DIRECTION == 0) { DEBUGLN(F("вправо")); } else
    if (sSTRIP_DIRECTION == 1) { DEBUGLN(F("вверх")); } else
    if (sSTRIP_DIRECTION == 2) { DEBUGLN(F("влево")); } else
    if (sSTRIP_DIRECTION == 3) { DEBUGLN(F("вниз")); } 
    DEBUG(F("  Тип: "));
    if (sMATRIX_TYPE == 0) { DEBUGLN(F("зигзаг")); } else
    if (sMATRIX_TYPE == 1) { DEBUGLN(F("параллельная")); }
    
    if (mWIDTH > 1 || mHEIGHT > 1) {
      DEBUG(F("  Размер сегмента: "));
      DEBUG(WIDTH); DEBUG('x'); DEBUGLN(HEIGHT);
      DEBUG(F("Cегменты: "));
      DEBUG(mWIDTH); DEBUG('x'); DEBUGLN(mHEIGHT);
      DEBUG(F("  Угол: ")); 
      if (mANGLE == 0) { DEBUGLN(F("левый нижний")); } else
      if (mANGLE == 1) { DEBUGLN(F("левый верхний")); } else
      if (mANGLE == 2) { DEBUGLN(F("правый верхний")); } else
      if (mANGLE == 3) { DEBUGLN(F("правый нижний")); }
      DEBUG(F("  Направление: "));
      if (mDIRECTION == 0) { DEBUGLN(F("вправо")); } else
      if (mDIRECTION == 1) { DEBUGLN(F("вверх")); } else
      if (mDIRECTION == 2) { DEBUGLN(F("влево")); } else
      if (mDIRECTION == 3) { DEBUGLN(F("вниз")); }
      DEBUG(F("  Тип: "));
      if (mTYPE == 0) { DEBUGLN(F("зигзаг")); } else
      if (mTYPE == 1) { DEBUGLN(F("параллельная")); }
    }
  }
  
  // ---------- Пины подключения ленты --------------
  
  bool hasLedOut = getLedLineUsage(1) || getLedLineUsage(2) || getLedLineUsage(3) || getLedLineUsage(4);
  if (hasLedOut) {
    DEBUGLN(F("Вывод на ленту:"));  
    for (uint8_t i = 1; i <= 4; i++) {
      bool isLineUsed = getLedLineUsage(i);
      if (isLineUsed) {
        int8_t    led_pin = getLedLinePin(i);
        uint16_t  led_start = getLedLineStartIndex(i);
        uint16_t  led_count = getLedLineLength(i);
        if (led_start + led_count > NUM_LEDS) {
          led_count = NUM_LEDS - led_start;
        }      
        DEBUG(F("  Линия ")); DEBUG(i); DEBUG(F(" PIN=")); DEBUG(pinName(led_pin)); DEBUG(F(", START=")); DEBUG(led_start); DEBUG(F(", COUNT=")); DEBUGLN(led_count);        
      }
    }
  } else {
    DEBUGLN(F("Вывод на ленту: нет назначенных пинов вывода"));  
  }
  
  // ------------------------------------------------

  DEBUGLN();
  DEBUGLN(F("Доступные возможности:"));
  
  DEBUG(F("+ Язык интерфейса: "));
  DEBUGLN(UI);
  
  DEBUG(F("+ Бегущая строка: шрифт "));
  if (BIG_FONT == 0)
    DEBUGLN(F("5x8"));
  if (BIG_FONT == 2)
    DEBUGLN(F("8x13"));
  if (BIG_FONT == 1)
    DEBUGLN(F("10x16"));

  #if (USE_BUTTON  == 1)
    DEBUG(F("+ Кнопка управления: "));
    if (vBUTTON_TYPE == 0) DEBUG(F("сенсорная PIN="));
    if (vBUTTON_TYPE == 1) DEBUG(F("тактовая PIN="));
    DEBUGLN(pinName(getButtonPin()));
  #else
    DEBUGLN(F("- Кнопка управления"));
  #endif

  DEBUGLN(F("+ Синхронизация времени с сервером NTP"));

  DEBUG((USE_POWER == 1 ? '+' : '-'));
  DEBUG(F(" Управление питанием матрицы"));
  #if (USE_POWER == 1)
    DEBUG(F(" PIN=")); DEBUG(pinName(getPowerPin()));
    if (isTurnedOff) { DEBUGLN(F("; ВЫКЛ")); } else { DEBUGLN(F("; ВКЛ")); }
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_ALARM == 1 ? '+' : '-'));
  DEBUG(F(" Управление питанием (линия будильника)"));
  #if (USE_ALARM == 1)
    DEBUG(F(" PIN=")); DEBUGLN(pinName(getAlarmPin()));
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_AUX == 1 ? '+' : '-'));
  DEBUG(F(" Управление питанием (дополнительная линия)"));
  #if (USE_AUX == 1)
    DEBUG(F(" PIN=")); DEBUG(pinName(getAuxPin()));
    if (isAuxActive) { DEBUGLN(F("; ВКЛ")); } else { DEBUGLN(F("; ВЫКЛ")); }
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_WEATHER == 1 ? '+' : '-'));
  DEBUGLN(F(" Получение информации о погоде"));

  if (spiffs_ok) {
    DEBUGLN(F("+ Управление через Web-канал"));
  }

  DEBUG((USE_E131 == 1 ? '+' : '-'));
  DEBUGLN(F(" Групповая синхронизация по протоколу E1.31"));

  DEBUG((USE_TM1637 == 1 ? '+' : '-'));
  DEBUG(F(" Дополнительный индикатор TM1637"));
  #if (USE_TM1637 == 1)
    DEBUG(F(" DIO=")); DEBUG(pinName(getTM1637DIOPin())); DEBUG(F(" CLK=")); DEBUGLN(pinName(getTM1637CLKPin()));
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_SD == 1 ? '+' : '-'));
  DEBUG(F(" Эффекты Jinx! с SD-карты"));
  DEBUG(USE_SD == 1 && FS_AS_SD == 1 ? String(F(" (эмуляция в FS)")) : "");
  #if (USE_SD == 1 && FS_AS_SD == 0)
    // CS (chip select) может быть изменен и задается в настройках
    // CLK, MISO, MOSI - аппаратные пины, фиксированы и SD-карта может быть подключена только к ним
    DEBUG(F(" CS=")); DEBUG(pinName(SD_CS_PIN));
    #if defined(ESP8266)
      DEBUGLN(F(", CLK=D5, MISO=D6, MOSI=D7"));
    #else
      DEBUGLN(F(", CLK=G18, MISO=G19, MOSI=G23"));
    #endif
  #else
    DEBUGLN();
  #endif

  DEBUG((USE_MP3 == 1 ? '+' : '-'));
  DEBUG(F(" Поддержка MP3 Player"));
  #if (USE_MP3 == 1)
    DEBUG(F(" STX=")); DEBUG(pinName(getDFPlayerSTXPin())); DEBUG(F(" SRX=")); DEBUGLN(pinName(getDFPlayerSRXPin()));
  #else
    DEBUGLN();
  #endif

  DEBUGLN();

  if (sMATRIX_TYPE == 2) {
    pWIDTH = mapWIDTH;
    pHEIGHT = mapHEIGHT;
  } else {
    pWIDTH = sWIDTH * mWIDTH;
    pHEIGHT = sHEIGHT * mHEIGHT;
  }

  NUM_LEDS = pWIDTH * pHEIGHT;
  maxDim   = max(pWIDTH, pHEIGHT);
  minDim   = min(pWIDTH, pHEIGHT);

  // Если Wiring еще не был инициализирован при старте программы - он инициализируется выше, в начале скетча
  // Но на тот момент неизвестно общее количество светодиодов в матрице - оно становится известным после выполнения loadSettings()
  // При инициализации включается активной только линия 1 и все светодиоды назначаются на нее
  if (!isWireInitialized) {
    putLedLineLength(1, NUM_LEDS);
  }

  // -----------------------------------------
  // В этом блоке можно принудительно устанавливать параметры, которые должны быть установлены при старте микроконтроллера
  // -----------------------------------------
  
  // -----------------------------------------  
    
  // Настройки ленты
  allocateLeds();         // leds =  new CRGB[NUM_LEDS];          
  allocateOverlay();      // overlayLEDs = new CRGB[OVERLAY_SIZE];
  DEBUGLN();

  FastLED.setBrightness(globalBrightness);
  if (CURRENT_LIMIT > 0) {
    FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  }

  if (leds != nullptr) {
    FastLED.clear();
    FastLED.show();
  }

  // Распечатать список активных эффектов
  printEffectUsage();
  DEBUGLN();
  
  // Создать массив для карты индексов адресации светодиодов в ленте
  bool ok = loadIndexMap();
  if (sMATRIX_TYPE == 2 && (!ok || mapListLen == 0)) {
    sMATRIX_TYPE = 0;
    putMatrixSegmentType(sMATRIX_TYPE);
  }
  
  // Инициализация SD-карты
  #if (USE_SD == 1)
    InitializeSD1();
  #endif

  // Инициализация SD-карты
  #if (USE_SD == 1)
    InitializeSD2();
  #endif

  // Проверить наличие резервной копии настроек EEPROM в файловой системе MK и/или на SD-карте
  eeprom_backup = checkEepromBackup();
  if ((eeprom_backup & 0x02) > 0) {
    DEBUG(F("Найдены сохраненные настройки: "));
    DEBUGLN(F("SD://eeprom.bin"));
  }
  if ((eeprom_backup & 0x01) > 0) {
    DEBUG(F("Найдены сохраненные настройки: "));
    DEBUGLN(F("FS://eeprom.bin"));
  }

  // Поиск доступных анимаций
  initAnimations();

  // Поиск картинок, пригодных для эффекта "Слайды"
  initialisePictures();

  web_ok = checkWebDirectory();
   
  #if (USE_POWER == 1)
    if (vPOWER_PIN >= 0) {
      pinMode(vPOWER_PIN, OUTPUT);
    }
  #endif

  #if (USE_ALARM == 1)
    if (vALARM_PIN >= 0) {
      pinMode(vALARM_PIN, OUTPUT);
    }
  #endif

  #if (USE_AUX == 1)
    if (vAUX_PIN >= 0) {
      pinMode(vAUX_PIN, OUTPUT);
    }
  #endif

  #if (USE_MP3 == 1)
    // Первый этап инициализации плеера - подключение и основные настройки
    InitializeDfPlayer1();
    // Второй этап инициализации плеера - проверка наличия файлов звуков на SD карте
    {
      String message(F("MP3 плеер недоступен."));
      if (isDfPlayerOk) {
        InitializeDfPlayer2();
        if (!isDfPlayerOk) {
          DEBUGLN(message);
        }
      } else {
          DEBUGLN(message);
      }
    }
  #endif
     
  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
  #endif

  // Настройка кнопки
  #if (USE_BUTTON  == 1)
    int8_t pin_btn = getButtonPin();
    if (pin_btn >= 0) {
      if (vBUTTON_TYPE == 0)
        butt = new GButton(pin_btn, LOW_PULL, NORM_OPEN);    // Для сенсорной кнопки
      else
        butt = new GButton(pin_btn, HIGH_PULL, NORM_OPEN);   // Для обычной кнопки
    }
    if (butt != nullptr) {
      butt->setStepTimeout(100);
      butt->setClickTimeout(300);
      butt->setDebounce(50);
      butt->tick();
      butt->isHolded();
    }
  #endif

  InitializeTexts();

  DEBUG(F("Свободно памяти: "));
  DEBUGLN(ESP.getFreeHeap());

  // Подключение к сети
  connectToNetwork();

  // Обработчики событий web-сервера
  server.serveStatic("/", LittleFS, BASE_WEB)
    .setDefaultFile("index.html")
    .setCacheControl("public, max-age=3600, must-revalidate");  // 1 hr for caching, then revalidate based on etag/IMS headers

  server.onNotFound(handleNotFound);
  server.begin();
  
  if (spiffs_ok) {
    DEBUGLN(F("HTTP server started"));  
  }

  #if (USE_E131 == 1)
    InitializeE131();
  #endif

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
 
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(host_name.c_str());
 
  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");
 
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = F("скетча...");
    else // U_SPIFFS
      type = F("файловой системы SPIFFS...");
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    DEBUG(F("Начато обновление "));    
    DEBUGLN(type);    
  });

  ArduinoOTA.onEnd([]() {
    DEBUGLN(F("\nОбновление завершено"));
  });

  ArduinoOTA.onProgress([](uint32_t progress, uint32_t total) {
    if (vDEBUG_SERIAL) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    }
  });

  ArduinoOTA.onError([](ota_error_t error) {
    DEBUG(F("Ошибка: "));
    DEBUGLN(error);
    if      (error == OTA_AUTH_ERROR)    { DEBUGLN(F("Неверное имя/пароль сети")); }
    else if (error == OTA_BEGIN_ERROR)   { DEBUGLN(F("Не удалось запустить обновление")); }
    else if (error == OTA_CONNECT_ERROR) { DEBUGLN(F("Не удалось установить соединение")); }
    else if (error == OTA_RECEIVE_ERROR) { DEBUGLN(F("Не удалось получить данные")); }
    else if (error == OTA_END_ERROR)     { DEBUGLN(F("Ошибка завершения сессии")); }
  });

  ArduinoOTA.begin();

  // UDP-клиент на указанном порту
  udp.begin(localPort);

  // Открываем Web-сокет управления через web-интерфейс
  initWebSocket();

  // Настройка внешнего дисплея TM1637
  #if (USE_TM1637 == 1)
    int8_t clk_pin = getTM1637CLKPin(); 
    int8_t dio_pin = getTM1637DIOPin();
    if (clk_pin >= 0 && dio_pin >= 0) {
      display = new TM1637Display(clk_pin, dio_pin, 75);  // 75 - 75 мкс - задержка импульсов при выводе. Default - 100; если на дисплее не отображается или неверные символы - увеличивайте до 100
      lastDisplay[0] = _empty;
      lastDisplay[1] = _empty;
      lastDisplay[2] = _empty;
      lastDisplay[3] = _empty;
      lastDisplayBrightness = 7;
      lastDotState = false;
      display->setBrightness(7);
      display->point(false);
      display->displayByte(_empty, _empty, _empty, _empty);
    }
  #endif

  DEBUG(F("Свободно памяти: "));
  DEBUGLN(ESP.getFreeHeap());

  // Таймер синхронизации часов
  ntpSyncTimer.setInterval(1000 * 60 * SYNC_TIME_PERIOD);

  #if (USE_WEATHER == 1)     
  // Таймер получения погоды
  weatherTimer.setInterval(1000 * 60 * SYNC_WEATHER_PERIOD);
  #endif

  // Таймер рассвета
  dawnTimer.stopTimer();
  
  // Проверить соответствие позиции вывода часов размерам матрицы
  // При необходимости параметры отображения часов корректируются в соответствии с текущими аппаратными возможностями
  checkClockOrigin();
  
  // Если был задан спец.режим во время предыдущего сеанса работы матрицы - включить его
  // Номер спец-режима запоминается при его включении и сбрасывается при включении обычного режима или игры
  // Это позволяет в случае внезапной перезагрузки матрицы (например по wdt), когда был включен спец-режим (например ночные часы или выкл. лампы)
  // снова включить его, а не отображать случайный обычный после включения матрицы
  int8_t spc_mode = getCurrentSpecMode();

  if (spc_mode >= 0 && spc_mode < MAX_SPEC_EFFECT) {
    setSpecialMode(spc_mode);
    set_isTurnedOff(spc_mode == 0);
    set_isNightClock(spc_mode == 8);
  } else {
    set_thisMode(getCurrentManualMode());
    if (thisMode < 0 || thisMode == MC_TEXT || thisMode >= SPECIAL_EFFECTS_START) {
      setRandomMode();
    } else {
      setEffect(thisMode);        
    }
  }
  
  autoplayTimer = millis();
  
  setIdleTimer();
}

void loop() {
  if (wifi_connected) {
    ArduinoOTA.handle();
  }

  ws.cleanupClients();
  processOutQueueW();

  // Если память под светодиоды была временно освобожденя для Web-сервера - рисовать негде. Ничего не делаем, просто ждем
  // пока WebServer закончит отправку файла и освободит память. В это время картинка на матрице "замрет".
  // Если память есть - выделяем ее под массив светодиодов
  if (leds == nullptr) {
    if (ESP.getFreeHeap() > NUM_LEDS * sizeof(CRGB)) {
      allocateLeds();
    }
  }

  if (leds == nullptr) {
    // Памяти нет, массив не создан - ничего не делаем.
    delay (10);
  } else {  
    // Если память под оверлей была временно освобожденя для Web-сервера - оверлей работать не будет. На матрице будут наблюдаться дефекты,
    // особенно на эффектах, работающих с оверлеем и в бегущей строке. Ждем пока WebServer закончит отправку файла и освободит памятью
    // Если свободная память появилась - воссоздаем массив оверлея.
    if (overlayLEDs == nullptr) {
      if (ESP.getFreeHeap() > OVERLAY_SIZE * sizeof(CRGB)) {
        allocateOverlay();
      }
    }
    process();
  }

  // При нехватки памяти для ответа на Web-запрос происходит временное освобождение памяти оверлея и массива диодов
  // На каждом цикле проверяется достаточно ли свободной памяти и попытка выделить память для массива светодиодов и овердея
  // Иногда память настолько фрагменирована, что выделить цельный кусок не удается.
  // После 1000 неудачных попыток - перезагрузить контроллер
  if (restartGuard > 1000) {
    DEBUGLN(F("Не удается выделить достаточно памяти."));
    DEBUGLN(F("Выполняется перезапуск системы."));
    delay(100);
    ESP.restart();
  }
}

// -----------------------------------------

void startWiFi(uint32_t waitTime) {
  
  WiFi.disconnect(true);
  set_wifi_connected(false);
  
  delay(10);               // Иначе получаем Core 1 panic'ed (Cache disabled but cached memory region accessed)
  WiFi.mode(WIFI_STA);
 
  // Пытаемся соединиться с роутером в сети
  if (ssid.length() > 0) {
    DEBUG(F("\nПодключение к "));
    DEBUG(ssid);

    if (IP_STA[0] + IP_STA[1] + IP_STA[2] + IP_STA[3] > 0) {
      WiFi.config(IPAddress(IP_STA[0], IP_STA[1], IP_STA[2], IP_STA[3]),  // 192.168.0.106
                  IPAddress(IP_STA[0], IP_STA[1], IP_STA[2], GTW),        // 192.168.0.1
                  IPAddress(255, 255, 255, 0),                            // Mask
                  IPAddress(IP_STA[0], IP_STA[1], IP_STA[2], GTW),        // DNS1 192.168.0.1
                  IPAddress(8, 8, 8, 8));                                 // DNS2 8.8.8.8                  
      DEBUG(F(" -> "));
      DEBUG(IP_STA[0]);
      DEBUG('.');
      DEBUG(IP_STA[1]);
      DEBUG('.');
      DEBUG(IP_STA[2]);
      DEBUG('.');
      DEBUG(IP_STA[3]);                  
    } 

    #if (USE_BUTTON == 1)
      DEBUG(F("\nДвойное нажатие на кнопку для прерывания подключения\nи переход в режим точки доступа"));
    #else
      DEBUG(F("\nЕсли подключение не будет установлено в течение 3 минут -\nбудет создана точка доступа"));
    #endif
    
    WiFi.begin(ssid.c_str(), pass.c_str());
  
    // Проверка соединения (таймаут 180 секунд, прерывается при необходимости нажатием кнопки)
    // Такой таймаут нужен в случае, когда отключают электричество, при последующем включении устройство стартует быстрее
    // чем роутер успеет загрузиться и создать сеть. При коротком таймауте устройство не найдет сеть и создаст точку доступа,
    // не сможет получить время, погоду и т.д.
    bool     stop_waiting = false;
    uint32_t start_wifi_check = millis();
    uint32_t last_wifi_check = 0;
    int16_t  cnt = 0;
    while (!(stop_waiting || wifi_connected)) {
      delay(1);
      if (millis() - last_wifi_check > 250) {
        last_wifi_check = millis();
        set_wifi_connected(WiFi.status() == WL_CONNECTED); 
        if (wifi_connected) {
          // Подключение установлено
          DEBUGLN();
          DEBUG(F("WiFi подключен. IP адрес: "));
          DEBUGLN(WiFi.localIP());
          break;
        }
        if (cnt % 50 == 0) {
          DEBUGLN();
        }
        DEBUG('.');
        cnt++;
      }
      if (millis() - start_wifi_check > waitTime) {
        // Время ожидания подключения к сети вышло
        break;
      }
      
      // Опрос состояния кнопки
      #if (USE_BUTTON  == 1)
        delay(1);
        if (butt != nullptr) {
          butt->tick();
          if (butt->hasClicks()) {
            int8_t clicks = butt->getClicks();
            // Двойной клик прерывает ожидание подключения к локальной сети
            if (clicks == 2) {
              DEBUGLN();
              DEBUGLN(F("Дважды нажата кнопка.\nОжидание подключения к сети WiFi прервано."));  
              stop_waiting = true;            
              break;
            }
          }
        }
      #endif
    }
    DEBUGLN();

    if (!wifi_connected && !stop_waiting) {
      DEBUGLN(F("Не удалось подключиться к сети WiFi."));
    }
  }  
}

void startSoftAP() {
  WiFi.softAPdisconnect(true);
  ap_connected = false;

  DEBUG(F("Создание точки доступа "));
  DEBUGLN(apName);
  
  ap_connected = WiFi.softAP(apName, apPass);

  for (uint8_t j = 0; j < 10; j++ ) {    
    delay(0);
    if (ap_connected) {
      DEBUGLN();
      DEBUG(F("Точка доступа создана. Сеть: '"));
      DEBUG(apName);
      // Если пароль совпадает с паролем по умолчанию - печатать для информации,
      // если был изменен пользователем - не печатать
      if (strcmp(apPass, "12341234") == 0) {
        DEBUG(F("'. Пароль: '"));
        DEBUG(apPass);
      }
      DEBUGLN(F("'."));
      DEBUG(F("IP адрес: "));
      DEBUGLN(WiFi.softAPIP());
      break;
    }    
    
    WiFi.enableAP(false);
    WiFi.softAPdisconnect(true);
    delay(500);
    
    DEBUG('.');
    ap_connected = WiFi.softAP(apName, apPass);
  }  
  DEBUGLN();  

  if (!ap_connected) { 
    DEBUGLN(F("Не удалось создать WiFi точку доступа."));
  }
}

void connectToNetwork() {
  // Подключиться к WiFi сети, ожидать подключения 180 сек пока, например, после отключения электричества роутер загрузится и поднимет сеть
  startWiFi(180000);

  // Если режим точки доступа не используется и к WiFi сети подключиться не удалось - создать точку доступа
  if (!wifi_connected){
    WiFi.mode(WIFI_AP);
    startSoftAP();
  }

  if (useSoftAP && !ap_connected) startSoftAP();    

  // Сообщить UDP порт, на который ожидаются подключения
  if (wifi_connected || ap_connected) {
    DEBUG(F("UDP-сервер на порту "));
    DEBUGLN(localPort);
  }
}
