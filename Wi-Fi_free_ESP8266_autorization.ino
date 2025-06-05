#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

// Создаем объект веб-сервера на порту 80
ESP8266WebServer server(80);
DNSServer dnsServer;

// Указываем имя точки доступа
const char* ssid = "100%-Free-WiFi";
const char* password = ""; // Пустой пароль для открытой сети

// Структура для хранения информации о клиенте
struct ClientInfo {
    String ip;
    String mac;
};

// Массив для хранения информации о клиентах
ClientInfo clients[10]; // Максимум 10 клиентов
int clientCount = 0; // Счетчик подключенных клиентов

// HTML-код для страницы авторизации с анимацией и информацией о клиенте
const char* authPageTemplate = R"rawliteral(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Установка вируса</title>
    <style>
        body {
            background-color: black;
            color: red;
            font-family: 'Courier New', Courier, monospace;
            text-align: center;
            padding: 50px;
        }
        h1 {
            font-size: 50px;
        }
        p {
            font-size: 20px;
        }
        #info {
            margin-top: 20px;
            font-size: 18px;
            color: white;
        }
        #progress {
            width: 100%;
            background-color: #555;
            height: 30px;
            border-radius: 5px;
            overflow: hidden;
            margin-top: 20px;
        }
        #bar {
            height: 100%;
            width: 0;
            background-color: red;
            animation: loading 5s linear infinite;
        }
        @keyframes loading {
            from { width: 0; }
            to { width: 100%; }
        }
    </style>
</head>
<body>
    <h1>Установка вируса...</h1>
    <p>Не беспокойтесь, всё будет происходить автоматически!</p>    
    <div id="progress">
        <div id="bar"></div>
    </div>
    <div id="info">
        <p>Ваш IP-адрес: %IP%</p>
        <p>Ваш MAC-адрес: %MAC%</p>
    </div>
    <script>
        // Функция для перехода в полноэкранный режим
        function goFullscreen() {
            let elem = document.documentElement; // Получаем элемент документа
            if (elem.requestFullscreen) {
                elem.requestFullscreen();
            } else if (elem.mozRequestFullScreen) { // Firefox
                elem.mozRequestFullScreen();
            } else if (elem.webkitRequestFullscreen) { // Chrome, Safari и Opera
                elem.webkitRequestFullscreen();
            } else if (elem.msRequestFullscreen) { // IE/Edge
                elem.msRequestFullscreen();
            }
        }

        // Переход в полноэкранный режим
        goFullscreen();

        setTimeout(function() {
            document.body.innerHTML = "<h1>Установка вируса завершена!</h1><p>Все ваши данные постепенно будут уничтожены в течение следующих 5 минут...<br>Спасибо, что воспользовались нашим сервисом.</p>";
        }, 5000); // Через 5 секунд меняем текст
    </script>
</body>
</html>
)rawliteral";

void setup() {
    // Инициализация последовательного порта
    Serial.begin(115200);
    delay(100); // Задержка для стабильности

    // Настройка точки доступа
    WiFi.softAP(ssid, password);
    Serial.println("Точка доступа запущена");
    Serial.print("IP адрес: ");
    Serial.println(WiFi.softAPIP());

    // Настройка DNS-сервера
    dnsServer.start(53, "*", WiFi.softAPIP());

    // Обработчик для корневого URL
    server.on("/", []() {
        // Получаем IP и MAC адрес клиента
        String clientIP = WiFi.softAPIP().toString(); // IP адрес точки доступа
        String clientMAC = WiFi.macAddress(); // MAC адрес точки доступа

        // Формируем страницу с подстановкой IP и MAC адресов
        String authPage = authPageTemplate;
        authPage.replace("%IP%", clientIP);
        authPage.replace("%MAC%", clientMAC);

        server.send(200, "text/html", authPage);
        
        // Выводим информацию о клиенте в консоль
        Serial.print("Подключен клиент: IP = ");
        Serial.print(clientIP);
        Serial.print(", MAC = ");
        Serial.println(clientMAC);
        
        // Сохраняем информацию о клиенте
        if (clientCount < 10) { // Проверяем, не превышает ли число клиентов максимальное значение
            clients[clientCount].ip = clientIP;
            clients[clientCount].mac = clientMAC;
            clientCount++;
        }
    });

    // Обработчик для всех других URL
    server.onNotFound([]() {
        server.sendHeader("Location", "/", true);
        server.send(302, "text/html", "");
    });

    // Запуск сервера
    server.begin();
    Serial.println("HTTP сервер запущен");
}

void loop() {
    // Обработка запросов сервера и DNS
    dnsServer.processNextRequest();
    server.handleClient();
}
