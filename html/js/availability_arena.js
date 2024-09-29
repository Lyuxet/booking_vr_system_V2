import { updateButtonsStateArena } from "./buttons_arena.js";

export function checkAvailabilityArena(bookingButtons, isCloseType) {
    var date = document.getElementById('date').value;
    var placegame = 'ARENA';
    // Проверяем URL и задаем значение для namegame в зависимости от пути файла
    // Извлекаем название игры
    const gameTitleElement = document.querySelector('.navigation h1');
    const namegame = gameTitleElement ? gameTitleElement.textContent.trim() : '';

    if (!date || !namegame) {
        console.error('Заполните все поля.');
        return;
    }

    const xhr = new XMLHttpRequest();
    xhr.open('GET', `http://localhost:8080/getBookingOpenArena?placegame=${encodeURIComponent(placegame)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}`, true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

    xhr.onload = function () {
        if (xhr.status >= 200 && xhr.status < 300) {
            try {
                const availability = JSON.parse(xhr.responseText);

                updateButtonsStateArena(availability, bookingButtons, isCloseType);
            } catch (error) {
                console.error('Ошибка при обработке ответа:', error);
            }
        } else {
            console.error('Ошибка запроса доступности. Статус:', xhr.status);
        }
    };

    xhr.onerror = function () {
        console.error('Ошибка сети.');
    };

    xhr.send();
}
