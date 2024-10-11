import { updateButtonsStateCubes } from "./buttons_cubes.js";

const bookingGrid = document.querySelector('.booking-grid');
const pacmanContainer = document.querySelector('.pac-man-container');


export function checkAvailability(bookingButtons) {
    var date = document.getElementById('date').value;
    var placegame = 'CUBES';
    // Проверяем URL и задаем значение для namegame в зависимости от пути файла
    // Извлекаем название игры
    const namegame = 'CUBES';

    if (!date || !namegame) {
        console.error('Заполните все поля.');
        return;
    }

    bookingGrid.style.display = 'none'; // Или flex, в зависимости от вашего дизайна
    pacmanContainer.style.display = 'flex'; // Скрываем анимацию загрузки

    const xhr = new XMLHttpRequest();
    xhr.open('GET', `http://cmsvrdevelopment.ru/api/getBookingCubes?placegame=${encodeURIComponent(placegame)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}`, true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

    const startTime = Date.now(); // Запоминаем время начала запроса

    xhr.onload = function () {
        const elapsedTime = Date.now() - startTime;
        const minDuration = 750; // Минимальная продолжительность анимации в миллисекундах

        const handleResponse = () => {
            bookingGrid.style.display = 'grid'; // Показываем сетку кнопок
            pacmanContainer.style.display = 'none'; // Скрываем анимацию загрузки

            if (xhr.status >= 200 && xhr.status < 300) {
                try {
                    const availability = JSON.parse(xhr.responseText);
                    updateButtonsStateCubes(availability, bookingButtons);
                } catch (error) {
                    console.error('Ошибка при обработке ответа:', error);
                }
            } else {
                console.error('Ошибка запроса доступности. Статус:', xhr.status);
            }
        };

        if (elapsedTime < minDuration) {
            // Если время выполнения запроса меньше минимального времени, ждем оставшееся время
            setTimeout(handleResponse, minDuration - elapsedTime);
        } else {
            // Если время запроса достаточно, сразу обрабатываем ответ
            handleResponse();
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
