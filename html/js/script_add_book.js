import { updateButtonsState } from "./buttons.js";
import { hidePriceDisplay } from "./priceDisplay.js";

document.addEventListener('DOMContentLoaded', function () {
    const addButton = document.getElementById('add');
    if (!addButton) {
        console.error('Кнопка "Добавить" не найдена');
        return;
    }
    
    addButton.addEventListener('click', function (event) {
        event.preventDefault();
        
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

        const gameTitleElement = document.querySelector('.navigation h1');
        const namegame = gameTitleElement ? gameTitleElement.textContent.trim() : 'CUBES';
        const isCloseType = document.querySelector('.booking-container-close') !== null;
        
        let selectedTimes = [];
        let selectedPlayersCount = [];
        let selectedPrice = [];
        
        selectedButtons.forEach(function (button) {
            const time = button.querySelector('.time').textContent;
            selectedTimes.push(time);
            const priceButton = parseInt(button.querySelector('.price').textContent, 10);
            if (!isCloseType) {
                const playersCount = parseInt(button.querySelector('.player-input').value, 10);
                selectedPlayersCount.push(playersCount);
                const price = priceButton * playersCount;
                selectedPrice.push(price);
            } else {
                selectedPlayersCount.push(10);
                selectedPrice.push(priceButton);
            }
        });

        let placegame = 'ARENA';
        if (document.querySelector('.booking-container-close') == null && document.querySelector('.booking-container-open') == null){
            placegame = 'CUBES';
        }


        const postData = {
            firstname, lastname, phone, email, placegame,
            typegame: isCloseType ? 'CLOSE' : 'OPEN',
            namegame, date, times: selectedTimes,
            playerCount: selectedPlayersCount, price: selectedPrice, comment
        };

        console.log('Sending data:', postData);

        const xhr = new XMLHttpRequest();
        const url = isCloseType 
            ? 'http://localhost:8081/addBookingOpenArena' 
            : 'http://localhost:8081/addBookingCubes';

        xhr.open('POST', url, true);
        xhr.setRequestHeader('Content-Type', 'application/json');
        
        xhr.onload = function () {
            if (xhr.status >= 200 && xhr.status < 300) {
                alert('Бронирование успешно отправлено.');
                updateBookingContainer();
            } else {
                alert(`Ошибка отправки бронирования: ${xhr.responseText}`);
                updateBookingContainer();
            }
        };
        
        xhr.onerror = function () {
            alert('Ошибка сети.');
        };

        xhr.send(JSON.stringify(postData));
    });
});

function updateBookingContainer() {
    const date = document.getElementById('date').value;
    let placegame = 'ARENA';
        if (document.querySelector('.booking-container-close') == null && document.querySelector('.booking-container-open') == null){
            placegame = 'CUBES';
    }
    const bookingButtons = document.querySelectorAll('.booking-button');
    const gameTitleElement = document.querySelector('.navigation h1');
    const namegame = gameTitleElement ? gameTitleElement.textContent.trim() : 'CUBES';

    const xhr = new XMLHttpRequest();
    const url = placegame === 'ARENA' 
        ? `http://localhost:8081/getBookingOpenArena?placegame=${encodeURIComponent(placegame)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}` 
        : `http://localhost:8081/getBookingCubes?placegame=${encodeURIComponent(placegame)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}`;

    xhr.open('GET', url, true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

    const startTime = Date.now();
    const minDuration = 750;

    xhr.onload = function () {
        const elapsedTime = Date.now() - startTime;
        const handleResponse = () => {
            if (xhr.status >= 200 && xhr.status < 300) {
                try {
                    const availability = JSON.parse(xhr.responseText);
                    if (typeof updateButtonsState === 'function') {
                        updateButtonsState(availability, bookingButtons, placegame);
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

        if (elapsedTime < minDuration) {
            setTimeout(handleResponse, minDuration - elapsedTime);
        } else {
            handleResponse();
        }
    };

    xhr.onerror = function () {
        console.error('Ошибка сети.');
    };

    xhr.send();
}
