import { updateButtonsStateCubes } from "./buttons_cubes.js";
import { hidePriceDisplay } from "./priceDisplay.js";

document.addEventListener('DOMContentLoaded', function () {

    const addButton = document.getElementById('add');
    if (!addButton) {
        console.error('Кнопка "Добавить" не найдена');
        return;
    }

    addButton.addEventListener('click', function (event) {
        event.preventDefault(); // Предотвращаем стандартное поведение кнопки

        const selectedButtons = document.querySelectorAll('.booking-button.selected');
        const firstname = document.getElementById('firstName').value;
        const lastname = document.getElementById('lastName').value;
        const phone = document.getElementById('phone').value;
        const email = document.getElementById('email').value;
        const date = document.getElementById('date').value;
        const comment = document.getElementById('comment').value;

        if (!firstname || !phone || !email) {
            alert("Пожалуйста, заполните все обязательные поля.");
            return;
        }

        if (selectedButtons.length === 0) {
            alert('Пожалуйста, выберите хотя бы одно время бронирования.');
            return;
        }

        const namegame = 'CUBES';
        let selectedTimes = [];
        let selectedPlayersCount = [];
        let selectedPrice = [];

        selectedButtons.forEach(function (button) {
            const time = button.querySelector('.time').textContent;
            selectedTimes.push(time);

            const priceButton = parseInt(button.querySelector('.price').textContent, 10);
            const playersCount = parseInt(button.querySelector('.player-input').value, 10);
            selectedPlayersCount.push(playersCount);
            const price = priceButton * playersCount;
            selectedPrice.push(price);
        });

        // Создаем объект данных для отправки в формате JSON
        const postData = {
            firstname: firstname,
            lastname: lastname,
            phone: phone,
            email: email,
            placegame: 'CUBES',
            typegame: 'CUBES',
            namegame: namegame,
            date: date,
            times: selectedTimes,
            playerCount: selectedPlayersCount,
            price: selectedPrice,
            comment: comment
        };

        console.log('Sending data:', JSON.stringify(postData));

        // Создаем запрос через XMLHttpRequest
        const xhr = new XMLHttpRequest();
        xhr.open('POST', 'http://cmsvrdevelopment.ru/api/addBookingCubes', true);
        xhr.setRequestHeader('Content-Type', 'application/json');

        // Время начала запроса
        const startTime = Date.now();
        const minDuration = 750; // Минимальная продолжительность анимации (в миллисекундах)

        xhr.onload = function () {
            const elapsedTime = Date.now() - startTime;

            const handleResponse = () => {
                if (xhr.status >= 200 && xhr.status < 300) {
                    try {
                        const availability = JSON.parse(xhr.responseText);
                        if (typeof updateButtonsStateArena === 'function') {
                            updateButtonsStateCubes(availability, bookingButtons);
                            hidePriceDisplay(); 
                        } else {
                            console.error('Функция updateButtonsStateArena не найдена');
                        }
                    } catch (error) {
                        console.error('Ошибка при обработке ответа:', error);
                    }
                } else {
                    console.error('Ошибка запроса доступности. Статус:', xhr.status);
                }
            };

            // Если время выполнения запроса меньше минимальной длительности, ждем оставшееся время
            if (elapsedTime < minDuration) {
                setTimeout(handleResponse, minDuration - elapsedTime);
            } else {
                handleResponse();
            }
        };
        xhr.onerror = function () {
            alert('Ошибка сети.');
        };

        // Отправляем данные на сервер в формате JSON
        xhr.send(JSON.stringify(postData));
    });

});

// Функция обновления контейнера бронирования
function updateBookingContainer() {
    // Получаем данные о доступных слотах
    const date = document.getElementById('date').value;
    const placegame = 'CUBES';
    const bookingButtons = document.querySelectorAll('.booking-button');

    const namegame = 'CUBES';

    const xhr = new XMLHttpRequest();
    xhr.open('GET', `http://cmsvrdevelopment.ru/api/getBookingCubes?placegame=${encodeURIComponent(placegame)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}`, true);
    xhr.setRequestHeader('Content-Type', 'application/json');

    xhr.onload = function () {
        if (xhr.status >= 200 && xhr.status < 300) {
            try {
                const availability = JSON.parse(xhr.responseText);
                if (typeof updateButtonsStateCubes === 'function') {
                    updateButtonsStateCubes(availability, bookingButtons);
                    hidePriceDisplay();
                } else {
                    console.error('Функция updateButtonsState не найдена');
                }
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
