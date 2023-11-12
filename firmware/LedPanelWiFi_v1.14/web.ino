// --------------- WEB-SERVER CALLBACK ----------------
#define BASE_WEB F("/web")
// Эта страница отображается, если по какой-то причине файловая система не активирована
// например, прошивка загружена, а файлы WebServer`a не загружены или прошивка скомпилирована
// без выделения места под файловую систему
static const char PGindex_page[] PROGMEM = R"===(
<!doctype html><html><head><meta charset="utf-8"><title>Server not active</title><style>
* {margin:0;padding:0;font-family:Arial,Helvetica,sans-serif;}
section {position:relative;width:100%;height:100vh;}
section .box {position:absolute;top:0;left:0;width:100%;height:100%;display:flex;justify-content:center;align-items:center;}
section .box.box2 {background:#7f54ce;clip-path:polygon(0 0,100% 0,100% 50%,0 50%);}
section .box h2 {color:#7f54ce;font-size:6vw;text-align:center;animation:anim 3s ease-in-out infinite;}
section .box.box2 h2 {color:#fff;}
@keyframes anim {0%,45%{transform:translateY(-70%);} 55%,90%{transform:translateY(70%);} 100%{transform:translateY(-70%);}}</style></head>
<body><section><div class="box box1"><h2>Server not active</h2></div><div class="box box2"><h2>Сервер не активен</h2></div></section></body></html>
)===";

void handleNotActive(AsyncWebServerRequest *request) {
  request->send_P(200, F("text/html"), PGindex_page);
}

void handleRoot(AsyncWebServerRequest *request) {
  if (!spiffs_ok) {
    handleNotActive(request);
    return;
  }
  
  String index_file = String(BASE_WEB) + String(F("/index.html"));
  String gz_file = index_file + F(".gz");
  
  if (LittleFS.exists(gz_file.c_str())) {
    DEBUGLN(String(F("web -> '")) + gz_file + String(F("' -> send (text/html)")));
    AsyncWebServerResponse *response = request->beginResponse(LittleFS, gz_file.c_str(), F("text/html"), false);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    response->addHeader(F("Cache-Control"), F("public, max-age=31536000"));
    request->send(response);
    return;
  }
  
  if (LittleFS.exists(index_file)) {
    request->send(LittleFS, index_file, F("text/html"));
    return;
  }
  
  handleNotActive(request);
}
 
void handleNotFound(AsyncWebServerRequest *request) {
  if (!spiffs_ok) {
    handleNotActive(request);
    return;  
  }  

  // Запрос на отправку файла требует не менее 4Кбайт. Если свободной памяти не хватает - запрошенный файл либо не отправляется,
  // либо микроконтроллер перезагружается из за недостатка памяти. Чтобы освободить память - временно освобождаем память оверлея часов и бегущей строки
  // Если этого недостаточно - временно освобождаем память под массив светодиодов
  // Массивы овердея и светодиодов будут воссозданы в основном цикле как только это позводит объем свободной памяти

  if (ESP.getFreeHeap() < 4096) {
    freeOverlay();
  }
  
  if (ESP.getFreeHeap() < 4096) {
    freeLeds();
  }

  if (ESP.getFreeHeap() < 4096) {
    int32_t freeMemory1 = ESP.getFreeHeap();
    DEBUGLN(F("Смена режима для освобождения памяти - Часы: "));
    releaseEffectResources(resourcesMode);
    setEffect(MC_CLOCK);
    int32_t freeMemory2 = ESP.getFreeHeap();
    DEBUGLN(String(freeMemory1) + String(F(" -> ")) + String(freeMemory2) +  String(F(" -- ")) + String(freeMemory2 - freeMemory1));    
  }

  String file = request->url();

  if (!file.startsWith(String(BASE_WEB))) {
    file = String(BASE_WEB) + file;
  }
  
  String gz_file = file + F(".gz");
  String type = "";    
  
  if (file.endsWith(F(".css")))
    type = F("text/css");
  else if (file.endsWith(F(".js")))
    type = F("text/javascript");
  else if (file.endsWith(F(".gif")))
    type = F("image/gif");
  else if (file.endsWith(F(".jpg")))
    type = F("image/jpeg");
  else if (file.endsWith(F(".png")))
    type = F("image/png");
  else if (file.endsWith(F(".svg")))
    type = F("image/svg+xml");
  else if (file.endsWith(F(".txt")))
    type = F("text/plain");
  else if (file.endsWith(F(".htm")))
    type = F("text/html");
  else if (file.endsWith(F(".html")))
    type = F("text/html");
  else if (file.endsWith(F(".ico")))
    type = F("image/x-icon");
  else if (file.endsWith(F(".json")))
    type = F("application/json");
  else if (file.endsWith(F(".woff2")))
    type = F("font/woff2");    

  if (type.length() > 0) {  
    if (LittleFS.exists(gz_file.c_str())) {
      DEBUGLN(String(F("web -> '")) + gz_file + String(F("' -> send (")) + type + ')');
      AsyncWebServerResponse *response = request->beginResponse(LittleFS, gz_file.c_str(), type.c_str(), false);
      response->addHeader(F("Content-Encoding"), F("gzip"));
      response->addHeader(F("Cache-Control"), F("public, max-age=31536000"));

      // При поступлении запроса на загрузку каждого файла вывести в консоль информацию о свободной памяти
      DEBUG(F("FM: "));
      DEBUG(ESP.getFreeHeap());
      
      request->send(response);

      // Вывод ДО и ПОСЛЕ
      DEBUG(F(" - "));
      DEBUGLN(ESP.getFreeHeap());

      return;
    }
    
    if (LittleFS.exists(file.c_str())) {      
      DEBUGLN(String(F("web -> '")) + file + String(F("' -> send (")) + type + ')');
      
      // При поступлении запроса на загрузку каждого файла вывести в консоль информацию о свободной памяти
      DEBUG(F("FM: "));
      DEBUG(ESP.getFreeHeap());

      request->send(LittleFS, file.c_str(), type.c_str());
      
      // ДО и ПОСЛЕ
      DEBUG(F(" - "));
      DEBUGLN(ESP.getFreeHeap());

      return;
    }
  }

  String message = String(F("File Not Found\nURI: ")) + file + '\n';
  request->send(404, F("text/plain"), message);
  DEBUGLN(String(F("web -> '")) + file + String(F("' -> not found")));
}

// --------------- WEB-SOCKET CALLBACK ----------------

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, AsyncWebSocketClient *client) { 
  // обрабатываем получаемые сообщения
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    //data[len] = 0;
    //memset(incomeCmdBuffer, 0, BUF_CMD_SIZE);
    //memcpy(incomeCmdBuffer, data, len);
    
    //String json = String(incomeCmdBuffer);
    //DEBUGLN("json='" + json + "'");
    doc.clear();
    DeserializationError error = deserializeJson(doc, (const char*)data, len);  // deserialize with deep-copy
    if (error) {
      //DEBUGLN(String(F("Ошибка в JSON: '")) + json + "'; err: " + String(error.f_str()));
      DEBUG(F("Ошибка в JSON: '"));
      DEBUGWR(data, len);
      DEBUG("'; err: ");
      DEBUGLN(error.f_str());
      return;
    }
    
    String event(doc["e"].as<const char*>());
    String command(doc["d"].as<const char*>()); 

    // DEBUGLN(String(F("<-- ")) + json);

    if (event == "cmd" && command.length() > 0) {  
      // В одном сообщении может быть несколько команд. Каждая команда начинается с '$' и заканчивается ';'/ Пробелы между ';' и '$' НЕ допускаются.
      command.replace("\n", "~");
      command.replace(";$", "\n");
      uint32_t count = CountTokens(command, '\n');

      for (uint8_t i=1; i<=count; i++) {
        String cmd(GetToken(command, i, '\n'));
        cmd.replace('~', '\n');
        cmd.trim();
        // После разделения команд во 2 и далее строке '$' (начало команды) удален - восстановить
        if (!cmd.startsWith(F("$"))) {
          cmd = '$' + cmd;
        }
        // После разделения команд во 2 и далее строке ';' (конец команды) удален - восстановить
        // Команда '$6 ' не может быть в пакете и признак ';' (конец команды) не используется - не восстанавливать
        if (!cmd.endsWith(";") && !cmd.startsWith(F("$6 "))) {
          cmd += ';';
        }        
        if (cmd.length() > 0) {
          if (queueLength < QSIZE_IN) {
            queueLength++;
            cmdQueue[queueWriteIdx++] = cmd;
            if (queueWriteIdx >= QSIZE_IN) queueWriteIdx = 0;
          } else {
            DEBUGLN(String(F("Переполнение очереди входящих команд - ")) + String(QSIZE_IN));
            DEBUG(F("Free: "));
            DEBUG(ESP.getFreeHeap());
            DEBUG(F(" Max: "));
            #if defined(ESP8266)
            DEBUG(ESP.getMaxFreeBlockSize());
            DEBUG(F("  Frag: "));
            DEBUG(ESP.getHeapFragmentation());
            #else
            DEBUG(ESP.getMaxAllocHeap());        
            #endif     
            DEBUGLN();                   
          }
        }
      }    
    }    
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      web_client_count++;
      last_web_client_id = client->id();
      Serial.printf("WebSocket клиент #%u с адреса %s\n", last_web_client_id, client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      web_client_count--;
      Serial.printf("WebSocket клиент #%u отключен\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len, client);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// ----------------------------------------------------

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String prepareMessage(const String &message, const String &topic) {
  doc.clear();
  doc["e"] = topic;
  doc["d"] = message;
  String out;
  serializeJson(doc, out);
//DEBUGLN(out);
  return out;
}

void putOutQueueW(const String& topic, const String& message) {
  
  // Если нет подключенных клиентов - ничего никому отправлять не нужно
  if (web_client_count == 0) {
    outWQueueReadIdx = 0;
    outWQueueWriteIdx = 0;
    outWQueueLength = 0;
    return;
  }  

  // Помещаем сообщение в очередь отправки
  if (outWQueueLength < QSIZE_OUT) {
    outWQueueLength++;
    outWQueue[outWQueueWriteIdx] = message;
    tpcWQueue[outWQueueWriteIdx] = topic;
    outWQueueWriteIdx++;
    if (outWQueueWriteIdx >= QSIZE_OUT) outWQueueWriteIdx = 0;
  } else {
    DEBUGLN(String(F("Переполнение очереди исходящих команд - ")) + String(QSIZE_OUT));
  }
}

void SendWeb(const String &message, const String& topic) {
  if (web_client_count < 0) web_client_count = 0;
  if (web_client_count == 0) return;

  putOutQueueW(topic, message);
}

void SendWebError(String &message) {
  SendWebKey("ER", message);
}

void SendWebInfo(const String &message) {
  SendWebKey("NF", message);
}

void SendWebKey(const String& key, const String& value) {
  String out, topic = TOPIC_STT;

  doc.clear();
  doc[key] = value;
  serializeJson(doc, out);      
  
  // Если key - ключ с возможно большим значением - отправлять как готовый json-объект
  // Если key - обычный ключ - удалить начальную и конечную фигурные скобки - тогда это будет
  // просто составная часть для накопительного объекта json
  if (!isBigSizeKey(key)) {
    out = out.substring(1, out.length() - 1);  
    SendWeb(out, topic);  
  } else {
    bool canWrite = ws.availableForWriteAll(); 
    if (canWrite) {
      out = prepareMessage(out, topic);
      ws.textAll(out);          
    } else {
      SendWeb(out, topic);
    }
  }
}

bool isClosed = false;

// Отправка сообщений из очереди в Web-сокет
void processOutQueueW() {

  long interval = millis() - lastWebSend;
  if (interval < 250) return;

  String message, topic, out, outMessage = "";
    
  // Пока есть сообщения в очереди и WebSocket способен их отправлять (не заполнена внутренняя очередь сокета)
  // выполнить отправку накопившихся сообшщений из очереди
  while (outWQueueLength > 0 && web_client_count > 0) {
    
    bool canWrite = ws.availableForWriteAll();      
    if (!canWrite) {
      if (!isClosed) {        
        DEBUGLN(F("-----------------------------"));                      // !!!
        DEBUGLN(F("Запись в сокет недоступна!"));                         // !!!
        DEBUG(F("Клиентов подключено: ")); DEBUGLN( ws.count());          // !!!
        DEBUGLN(F("Перезапуск сокета..."));                               // !!!
        // !!! Этой функции нет в стандартной библиотеке. Она есть только в измененной из папки проекта.
        // !!! Если здесь возникает ошибка - скопируйте библиотеку ESPAsyncWebServer из папки проекта libraries в
        // !!! папку, где Arduino IDE хранит установленные библиотеки        
        // !!! Впрочем, похоже это не очень помогает. Можно просто закомментировать данную строку...
        // Вызов ws.availableForWriteAll() выше проверяет доступна ли запись новых данных в очередь сокета.
        // По непонятной причине сокет перестает отправлять данные, сообщая, что его очередь переполнена
        // Вызов этой функции очищает переполненную очередь вебсокета, что по идее должно его перезапустить
        ws.clearQueueAll();
        int8_t cnt = 0;
        while (ws.count() > 0 && cnt < 300) {
          ws.cleanupClients(0);
          delay(10);
          cnt++;
        }
        DEBUGLN(String(F("Клиентов подключено: ")) + String(ws.count())); // !!!
        isClosed = true;        
      }
      break;
    }
    
    isClosed = false;
    
    // Cодержимое отправляемого сообщения
    message = outWQueue[outWQueueReadIdx];
    topic = tpcWQueue[outWQueueReadIdx];

    if (message.length() > 0) {
      // Если message начинается с '{' - это готовое сообщение для отправки клиентам, только обернуть в конверт
      // Если message НЕ начинается с '{' - это строка часть JSON-объекта пара ключ-значение
      if (message[0] == '{') {
        // {"act":"TIME","server_name":"ru.pool.ntp.org","server_ip":"85.21.78.23","result":"REQUEST"}
        // {FM":"9008"}
        // Такие сообщения могут быть а в любом топике
        out = prepareMessage(message, topic);          
        ws.textAll(out);          
        lastWebSend = millis();  
      } else {
        // "FM":"9008" -- выделить значение ключа
        // Такие сообщения могут быть только в топике TOPIC_STT
        outMessage += message + ',';
      }
    }
    
    // Извлекаем сообщение из очереди
    outWQueue[outWQueueReadIdx].clear();
    tpcWQueue[outWQueueReadIdx].clear();
    outWQueueReadIdx++;
    
    if (outWQueueReadIdx >= QSIZE_OUT) outWQueueReadIdx = 0;
    outWQueueLength--;  
  }

  if (outMessage.length() > 0) {
    // Были добавлены ключи-значения в накопительный объект -
    // сформировать сообщение и отправить клиентам
    outMessage = '{' + outMessage.substring(0, outMessage.length() - 1) + '}';
    // Такие сообщения могут быть а в любом топике
    topic = TOPIC_STT;
    out = prepareMessage(outMessage, topic);
    ws.textAll(out);
    lastWebSend = millis();  
  }
  
}

void checkWebDirectory() {

  String directoryName(BASE_WEB);
  DEBUG(F("Папка '"));
  DEBUG(directoryName);
  
  if (LittleFS.exists(directoryName)) {    
    DEBUGLN(F("' обнаружена."));
  } else {
    DEBUGLN(F("' не обнаружена."));
    return;
  }
  
  DEBUGLN(F("Cпискок файлов web-интерфейса:"));  

  String dir_name(directoryName);
  int16_t p = dir_name.lastIndexOf("/");
  if (p >= 0) dir_name = dir_name.substring(p + 1);

  checkWebSubDir(1, directoryName, dir_name);  
}

void checkWebSubDir(uint8_t level, const String& full_dir_name, const String& dir_name) {
  
  String   file_name, fs, sp;
  uint32_t file_size;
  uint8_t  lvl;

  sp = " ";
  DEBUGLN(padRight(sp, level * 3) + "[" + dir_name + "]");
  
  #if defined(ESP32)
    File folder = LittleFS.open(full_dir_name);
  #else
    Dir  folder = LittleFS.openDir(full_dir_name);
  #endif
    
  while (true) {

    #if defined(ESP32)
      File entry = folder.openNextFile();
      if (!entry) break;
      file_name = entry.name();
      // Полученное имя файла содержит имя папки (на ESP32 это так, на ESP8266 - только имя файла) - оставить только имя файла
      int16_t p = file_name.lastIndexOf("/");
      if (p >= 0) file_name = file_name.substring(p + 1);
    #else        
      if (!folder.next()) break;
      File entry = folder.openFile("r");
      file_name = entry.name();
    #endif

    if (!entry.isDirectory()) {
            
      file_size = entry.size();
      fs = String(file_size);
      sp = " ";
      
      if (file_size == 0) {
        entry.close();
        continue;    
      }

      sp = padRight(sp, (level + 1) * 3) + padRight(file_name, 26);
      sp = padRight(sp, 40);
      
      DEBUGLN(sp + padLeft(fs, 8));
      
    } else {
      lvl = level + 1;
      checkWebSubDir(lvl, full_dir_name + "/" + file_name, file_name);
    }
    entry.close();
  }        
}
