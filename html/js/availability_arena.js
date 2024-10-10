import { updateButtonsStateArena } from "./buttons_arena.js";

const bookingGrid = document.querySelector('.booking-grid');
const pacmanContainer = document.querySelector('.pac-man-container');

export function checkAvailabilityArena(bookingButtons) {
    // Скрываем кнопки и показываем элемент загрузки
    var date = document.getElementById('date').value;
    var placegame = 'ARENA';

    // Извлекаем название игры
    const gameTitleElement = document.querySelector('.navigation h1');
    const namegame = gameTitleElement ? gameTitleElement.textContent.trim() : '';

    if (!date || !namegame) {
        console.error('Заполните все поля.');
        return;
    }

    bookingGrid.style.display = 'none'; // Или flex, в зависимости от вашего дизайна
    pacmanContainer.style.display = 'flex'; // Скрываем анимацию загрузки

    const xhr = new XMLHttpRequest();
    xhr.open('GET', `http://localhost:8081/getBookingOpenArena?placegame=${encodeURIComponent(placegame)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}`, true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

    xhr.onload = function () {
        // Возвращаем кнопки и скрываем загрузку после получения ответа
        bookingGrid.style.display = 'grid'; // Или flex, в зависимости от вашего дизайна
        pacmanContainer.style.display = 'none'; // Скрываем анимацию загрузки

        if (xhr.status >= 200 && xhr.status < 300) {
            try {
                const availability = JSON.parse(xhr.responseText);
                updateButtonsStateArena(availability, bookingButtons);
            } catch (error) {
                console.error('Ошибка при обработке ответа:', error);
            }
        } else {
            console.error('Ошибка запроса доступности. Статус:', xhr.status);
        }
    };

    xhr.onerror = function () {
        // Вернуть кнопки и скрыть загрузку в случае ошибки сети
        bookingGrid.style.display = 'grid';
        pacmanContainer.style.display = 'none';
        console.error('Ошибка сети.');
    };

    xhr.send();
}
