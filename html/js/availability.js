import { updateButtonsState } from "./buttons.js";

const bookingGrid = document.querySelector('.booking-grid');
const pacmanContainer = document.querySelector('.pac-man-container');

export function checkAvailability(bookingButtons, place) {
    // Скрываем кнопки и показываем элемент загрузки
    const date = document.getElementById('date').value;
    
    
    const gameTitleElement = document.querySelector('.navigation h1');
    const namegame = gameTitleElement ? gameTitleElement.textContent.trim() : 'CUBES';
    if (!date || !namegame) {
        console.error('Заполните все поля.');
        return;
    }


    bookingGrid.style.display = 'none'; // Или flex, в зависимости от вашего дизайна
    pacmanContainer.style.display = 'flex'; // Скрываем анимацию загрузки

    const xhr = new XMLHttpRequest();
<<<<<<< HEAD:html/js/availability.js
    const url = place === 'ARENA' ? 
        `http://localhost:8081/getBookingOpenArena?placegame=${encodeURIComponent(place)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}` :
        `http://localhost:8081/getBookingCubes?placegame=${encodeURIComponent(place)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}`;

    
    xhr.open('GET', url, true);
=======
    xhr.open('GET', `http://localhost:8081/getBookingOpenArena?placegame=${encodeURIComponent(placegame)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}`, true);
>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313:html/js/availability_arena.js
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
                    updateButtonsState(availability, bookingButtons, place);
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
