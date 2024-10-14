import { SetDate } from "./date.js";
import { checkAvailability } from "./availability.js";
import { handleClick, handleInput } from "./buttons.js";
import { button_cubes } from "./button_data.js";
import { hidePriceDisplay } from "./priceDisplay.js";

const bookingButtons = document.querySelectorAll('.booking-button');
let socket = null; // Глобальная переменная для WebSocket
let storedDate = null; // Глобальная переменная для хранения даты
<<<<<<< HEAD
const place = 'CUBES'; // Указываем место как 'CUBES'
=======
>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313

document.addEventListener('DOMContentLoaded', function () {
    SetDate();
    storedDate = document.getElementById('date').value;
<<<<<<< HEAD
    initializeWebSocket(place, storedDate);
    
=======
    var place = 'CUBES'; // Указываем место как 'CUBES'
    initializeWebSocket(place, storedDate);

>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
    window.onbeforeunload = function() {
        if (socket) {
            socket.close(); // Закрываем соединение перед уходом со страницы
        }
    };

    // Обработчик события изменения видимости страницы
    document.addEventListener('visibilitychange', function() {
        if (document.visibilityState === 'visible' && (!socket || socket.readyState !== WebSocket.OPEN)) {
            console.log('Страница активна, восстанавливаем соединение WebSocket...');
            if (socket) {
                socket.close();
            }
            initializeWebSocket(place, storedDate); // Переподключаемся с сохранённой датой
        }
    });

    // Инициализация кнопок
    bookingButtons.forEach(button => {
<<<<<<< HEAD
        initializeBookingButton(button, socket, place); // Без учета isCloseType
=======
        initializeBookingButton(button); // Без учета isCloseType
>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
    });

    // Обновляем цены и состояния кнопок при изменении даты
    $('#date').on('change', function () {
        const selectedDate = $('#date').datepicker('getDate');
<<<<<<< HEAD
=======

>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
        // Получаем локальную дату без временной зоны
        const year = selectedDate.getFullYear();
        const month = ('0' + (selectedDate.getMonth() + 1)).slice(-2); // Добавляем ведущий ноль
        const day = ('0' + selectedDate.getDate()).slice(-2); // Добавляем ведущий ноль
<<<<<<< HEAD
        storedDate = `${year}.${month}.${day}`;
=======
        storedDate = `${year}-${month}-${day}`;

        console.log(storedDate);
>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
        if (socket) {
            socket.close(); // Закрываем текущее WebSocket соединение перед созданием нового
        }
        initializeWebSocket(place, storedDate); // Инициализируем новое соединение с сохранённой датой
<<<<<<< HEAD
=======

>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
        hidePriceDisplay();
        checkAvailability(bookingButtons, place); // Проверка доступности мест
    });

<<<<<<< HEAD
=======
    const initialDate = $('#date').datepicker('getDate');
    checkAvailability(bookingButtons); // Проверка доступности мест для начальной даты
>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
});

function initializeWebSocket(arena, currentDate) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        return; // Возвращаемся, если соединение уже установлено
    }
<<<<<<< HEAD
=======

>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
    socket = new WebSocket('ws://localhost:8082/ws'); // используем глобальную переменную
    socket.addEventListener('open', function() {
        // Отправляем JSON с информацией о месте игры и дате
        const data = JSON.stringify({ place: arena, date: currentDate });
        socket.send(data);
<<<<<<< HEAD
        checkAvailability(bookingButtons, place);
    });
    socket.addEventListener('close', function(event) {
        if (event.code === 1006) {
            console.log('Соединение закрыто неожиданно, пытаемся переподключиться...');
            setTimeout(function() {
                if (!socket || socket.readyState !== WebSocket.OPEN) {
                    initializeWebSocket(arena, currentDate); // Переподключение
                }
            }, 3000); // Попытка переподключения через 3 секунды
        } 
    });
=======
        checkAvailability(bookingButtons);
    });

    socket.addEventListener('close', function(event) {
        if (event.code === 1006) {
            console.log('Соединение закрыто неожиданно, пытаемся переподключиться...');
            setTimeout(function() {
                if (!socket || socket.readyState !== WebSocket.OPEN) {
                    initializeWebSocket(arena, currentDate); // Переподключение
                }
            }, 3000); // Попытка переподключения через 3 секунды
        } else {
            console.log(`Соединение закрыто: ${event.code}`);
        }
    });

>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
    socket.addEventListener('message', function(event) {
        try {
            checkAvailability(bookingButtons, place);
        } catch (error) {
            console.error('Ошибка при обработке сообщения:', error);
        }
    });
<<<<<<< HEAD
=======

>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
    socket.addEventListener('error', function(error) {
        console.error('Ошибка WebSocket:', error);
    });
    return socket;
}

<<<<<<< HEAD
function initializeBookingButton(button, socket, place) {
    button.addEventListener('click', function(event) {
        handleClick.call(this,event, place); // Используем контекст текущей кнопки
        const buttonId = this.id; // Получаем id кнопки
        const playerInput = this.querySelector('.player-input');
        const playerCount = playerInput.value;
        const bookingData = { buttonId: buttonId, players: playerCount };

=======
function initializeBookingButton(button) {
    button.addEventListener('click', function() {
        handleClickCubes.call(this); // Используем контекст текущей кнопки
        const buttonId = this.id; // Получаем id кнопки
        const playerInput = this.querySelector('.player-input');
        const playerCount = playerInput.value;
        const bookingData = {
            buttonId: buttonId,
            players: playerCount,
        };
>>>>>>> 986f3a47fbf8062dd8860f05dee3044ceaac9313
        if (socket.readyState === WebSocket.OPEN) {
            socket.send(JSON.stringify(bookingData)); // Отправляем данные на сервер
        } else {
            console.warn('WebSocket не открыт. Не удается отправить данные.');
        }
    });

    const buttonId = button.id;
    const playerInput = button.querySelector('.player-input');
    if (button_cubes[buttonId]) {
        const data = button_cubes[buttonId];
        button.querySelector('.time').textContent = data.time;
        button.querySelector('.price').textContent = `${data.price} ₽`;
        button.querySelector('.price').setAttribute('data-price', data.price);
    }
    playerInput.setAttribute('maxlength', '2');
    restrictNonNumericInput(playerInput); // Ограничение ввода только цифр
    playerInput.addEventListener('input', function(event) {
        handleInput.call(this, event, place); // Используем контекст текущей кнопки
    });
}

function restrictNonNumericInput(input) {
    input.addEventListener('input', function() {
        this.value = this.value.replace(/[^0-9]/g, ''); // Удаление всех символов кроме цифр
    });
    input.addEventListener('keydown', function(event) {
        const isNumberKey = (event.keyCode >= 48 && event.keyCode <= 57) ||  // цифры сверху
                            (event.keyCode >= 96 && event.keyCode <= 105) || // цифры numpad
                            event.keyCode === 8 ||  // backspace
                            event.keyCode === 46;   // delete
        if (!isNumberKey) {
            event.preventDefault();
        }
    });
    input.addEventListener('paste', function(event) {
        const pastedData = (event.clipboardData || window.clipboardData).getData('text');
        if (!/^\d*$/.test(pastedData)) {
            event.preventDefault(); // Блокировка вставки, если содержимое не цифры
        }
    });
}
