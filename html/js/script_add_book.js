import { updateButtonsState } from "./buttons.js";
import { hidePriceDisplay } from "./priceDisplay.js";

const addButton = document.getElementById('add');
function showNotification(message, isError = false) {
    const notification = document.getElementById('notification');
    notification.textContent = message;
    notification.classList.toggle('error', isError); // Добавляем класс 'error', если это ошибка
    
    // Показываем уведомление
    notification.classList.add('show');
    notification.classList.remove('hide'); // Убираем класс 'hide', если он был

    // Скрываем уведомление через 3 секунды с плавной анимацией
    setTimeout(() => {
        notification.classList.remove('show');
        notification.classList.add('hide');
        addButton.classList.remove('disabled');
    }, 3000);
}



document.addEventListener('DOMContentLoaded', function () {
    addButton.addEventListener('click', function (event) {
        event.preventDefault();
        addButton.classList.add('disabled');

        const selectedButtons = document.querySelectorAll('.booking-button.selected');
        const firstname = document.getElementById('firstName').value;
        const lastname = document.getElementById('lastName').value;
        const phone = document.getElementById('phone').value;
        const email = document.getElementById('email').value;
        const date = document.getElementById('date').value;
        const comment = document.getElementById('comment').value;

        const lastname_admin = document.getElementById('lastname_admin') ? document.getElementById('lastname_admin').value : 'Клиент';
        const book_status = document.getElementById('book_status') ? document.getElementById('book_status').value : 'Принят';

        if (!firstname || !phone) {
            showNotification("Пожалуйста, заполните все обязательные поля", true);
            return;
        }

        if (selectedButtons.length === 0) {
            showNotification('Пожалуйста, выберите хотя бы одно время бронирования', true);
            return;
        }

        if (!document.getElementById('checkbox').checked) {
            showNotification('Пожалуйста, примите соглашение на обработку персональных данных', true);
            return;
        }

        if (!isValidEmailDomain(email)){
            showNotification("Пожалуйста, введите корректную почту",true);
            return;
        }

        let selectedTimes = [];
        for (let button of selectedButtons) {
            const timeText = button.querySelector('.time').textContent.trim();
            const [startTime] = timeText.split(' - ').map(time => time.trim());
            const [startHours, startMinutes] = startTime.split(':').map(Number);

            const now = new Date();
            const selectedDateText = document.getElementById('date').value;
            const [year, month, day] = selectedDateText.split('.').map(Number);
            const buttonDate = new Date(year, month - 1, day, startHours, startMinutes);

            if (buttonDate <= now) {
                showNotification(`Время ${timeText} уже прошло, выберите другое время`, true);
                updateBookingContainer();
                return;
            }
            selectedTimes.push(timeText);
        }

        if (!selectedTimes.length) return;

        const gameTitleElement = document.querySelector('.navigation h1');
        const namegame = gameTitleElement ? gameTitleElement.textContent.trim() : 'На Ваш выбор';
        const isCloseType = document.querySelector('.booking-container-close') !== null;

        let selectedPlayersCount = [];
        let selectedPrice = [];

        selectedButtons.forEach(function (button) {
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

        let placegame = 'VR Арена';
        if (document.querySelector('.booking-container-close') == null && document.querySelector('.booking-container-open') == null) {
            placegame = 'VR Кубы';
        }

        const postData = {
            firstname,
            lastname,
            phone,
            email,
            placegame,
            typegame: (placegame == 'VR Арена' ? (isCloseType ? 'Закрытая игра' : 'Открытая игра') : ''),
            namegame,
            date,
            times: selectedTimes,
            playerCount: selectedPlayersCount,
            price: selectedPrice,
            comment,
            lastname_admin,
            book_status
        };

        const xhr = new XMLHttpRequest();
        const url = placegame === 'ARENA' 
            ? 'http://localhost:8081/addBookingOpenArena'
            : 'http://localhost:8081/addBookingCubes';
        xhr.open('POST', url, true);
        xhr.setRequestHeader('Content-Type', 'application/json');
        xhr.onload = function () {
            if (xhr.status >= 200 && xhr.status < 300) {
                showNotification('Бронирование успешно отправлено');
                updateBookingContainer();
            } else {
                showNotification(`Ошибка отправки бронирования: ${xhr.responseText}`, true);
                updateBookingContainer();
            }
        };
        xhr.onerror = function () {
            showNotification('Ошибка сети', true);
        };
        xhr.send(JSON.stringify(postData));
    });
});


function updateBookingContainer() {
    const date = document.getElementById('date').value;
    let placegame = 'VR Арена';
        if (document.querySelector('.booking-container-close') == null && document.querySelector('.booking-container-open') == null){
            placegame = 'VR Кубы';
    }
    const bookingButtons = document.querySelectorAll('.booking-button');
    const gameTitleElement = document.querySelector('.navigation h1');
    const namegame = gameTitleElement ? gameTitleElement.textContent.trim() : 'На Ваш выбор';

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
                    updateButtonsState(availability, bookingButtons, placegame);
                    hidePriceDisplay();
                    addButton.classList.remove('disabled');
                   
                } catch (error) {
                    console.error('Ошибка при обработке ответа:', error);
                    addButton.classList.remove('disabled');
                }
            } else {
                console.error('Ошибка запроса доступности. Статус:', xhr.status);
                addButton.classList.remove('disabled');
            }
        };

        if (elapsedTime < minDuration) {
            setTimeout(handleResponse, minDuration - elapsedTime);
        } else {
            handleResponse();
        }
    };

    xhr.onerror = function () {
        console.error('Ошибка сети');
        addButton.classList.remove('disabled');
    };

    xhr.send();
}


function isValidEmailDomain(email) { 
    if (!email || email.trim() === "") { 
        return true; 
    }

    const emailDomains = [ 
        "@gmail.com", 
        "@yahoo.com", 
        "@hotmail.com", 
        "@utlook.com", 
        "@icloud.com", 
        "@mail.ru", 
        "@aol.com", 
        "@yandex.ru", 
        "@mail.com", 
        "@protonmail.com",
        "@inbox.ru"
    ];

    return emailDomains.some(domain => email.endsWith(domain)); 
}