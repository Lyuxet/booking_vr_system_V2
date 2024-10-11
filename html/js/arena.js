import { updatePricesArena } from "./price.js";
import { SetDate } from "./date.js";
import { checkAvailabilityArena } from "./availability_arena.js";
import { handleClickArena, handleInputArena } from "./buttons_arena.js";
import { button_data_open_arena, button_data_close_arena } from "./button_data.js";
import { hidePriceDisplay } from "./priceDisplay.js";

const bookingButtons = document.querySelectorAll('.booking-button');


document.addEventListener('DOMContentLoaded', function () {
    let socket = createWebSocket();
    SetDate();
    const isCloseType = document.querySelector('.booking-container-close') !== null;

    window.onbeforeunload = function() {
        if (socket) {
            socket.close(); // Закрываем соединение перед уходом со страницы
        }
    };

    // Обработчик события изменения видимости страницы
    document.addEventListener('visibilitychange', function() {
        if (document.visibilityState === 'visible' && socket.readyState !== WebSocket.OPEN) {
            console.log('Страница активна, восстанавливаем соединение WebSocket...');
            socket = createWebSocket(); // Переподключаемся
        }
    });


    // Инициализация кнопок
    bookingButtons.forEach(button => {
        initializeBookingButton(button, socket, isCloseType);
    });

    // Обновляем цены и состояния кнопок при изменении даты
    $('#date').on('change', function () {
        const selectedDate = $('#date').datepicker('getDate');
        updatePricesArena(selectedDate, bookingButtons); // Обновление цены
        hidePriceDisplay();
        checkAvailabilityArena(bookingButtons); // Проверка доступности мест
    });

    const initialDate = $('#date').datepicker('getDate');
    updatePricesArena(initialDate, bookingButtons); // Обновление цены для начальной даты
});

function  createWebSocket() {
    const socket = new WebSocket('ws://cmsvrdevelopment.ru/ws');

    socket.addEventListener('open', function() {
        checkAvailabilityArena(bookingButtons);
    });

    socket.addEventListener('message', function(event) {
        try {
            checkAvailabilityArena(bookingButtons);
        } catch (error) {
            console.error('Ошибка при обработке сообщения:', error);
        }
    });

    socket.addEventListener('close', function(event) {
        // Автоматическое переподключение при закрытии
        setTimeout(function() {
            createWebSocket();
        }, 3000); // Попытка переподключения через 3 секунды
    });

    socket.addEventListener('error', function(error) {
        console.error('Ошибка WebSocket:', error);
    });

    return socket;
}

function initializeBookingButton(button, socket, isCloseType) {
    button.addEventListener('click', function() {
        handleClickArena.call(this); // Используем контекст текущей кнопки

        const buttonId = this.id; // Получаем id кнопки
        const playerInput = this.querySelector('.player-input');
        const playerCount = playerInput.value;

        const bookingData = {
            buttonId: buttonId,
            players: playerCount,
        };

        socket.send(JSON.stringify(bookingData)); // Отправляем данные на сервер
    });

    const buttonId = button.id;
    const playerInput = button.querySelector('.player-input');

    if (!isCloseType) {
        if (button_data_open_arena[buttonId]) {
            const data = button_data_open_arena[buttonId];
            button.querySelector('.time').textContent = data.time;
            button.querySelector('.price').textContent = `${data.price} ₽`;
            button.querySelector('.price').setAttribute('data-price', data.price);
        }

        playerInput.setAttribute('maxlength', '2');
        restrictNonNumericInput(playerInput); // Ограничение ввода только цифр
        playerInput.addEventListener('input', handleInputArena);
    } else {
        if (button_data_close_arena[buttonId]) {
            const data = button_data_close_arena[buttonId];
            button.querySelector('.time').textContent = data.time;
            button.querySelector('.price').textContent = `${data.price} ₽`;
            button.querySelector('.price').setAttribute('data-price', data.price);
        }
    }
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
