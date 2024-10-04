import { updateButtonStateCubes } from "./buttons_cubes.js";
import { SetDate } from "./date.js";
import { checkAvailability } from "./availability_cubes.js";
import { handleClickCubes, handleInputCubes } from "./buttons_cubes.js";
import { button_cubes } from "./button_data.js";
import { hidePriceDisplay } from "./priceDisplay.js";

document.addEventListener('DOMContentLoaded', function () {
    SetDate();
    const bookingButtons = document.querySelectorAll('.booking-button');

    console.log('Попытка подключения к WebSocket...');
    const socket = new WebSocket('ws://localhost:8082/ws');

    socket.addEventListener('open', function() {
        console.log('Соединение установлено');
    });

    socket.addEventListener('message', function(event) {
        try {
            console.log('Сообщение от сервера:', event.data);
            checkAvailability(bookingButtons);
            console.log('Обновление произошло');
        } catch (error) {
            console.error('Ошибка при обработке сообщения:', error);
        }
    });

    socket.addEventListener('close', function(event) {
        console.log('Соединение закрыто:', event.code, event.reason);
    });

    socket.addEventListener('error', function(error) {
        console.error('Ошибка WebSocket:', error);
    });

    window.onbeforeunload = function() {
        if (socket) {
            socket.close(); // Закрываем соединение перед уходом со страницы
        }
    };

    console.log('Инициализация');

    // Инициализация кнопок
    bookingButtons.forEach(button => {
        initializeBookingButton(button, socket);
    });

    // Обновляем состояния кнопок при изменении даты
    $('#date').on('change', function () {
        const selectedDate = $('#date').datepicker('getDate');
        hidePriceDisplay();
        checkAvailability(bookingButtons);
    });

    // Проверка доступности при загрузке страницы
    checkAvailability(bookingButtons);
    const initialDate = $('#date').datepicker('getDate');
});

function initializeBookingButton(button, socket) {
    button.addEventListener('click', function() {
        handleClickCubes.call(this); // Используем контекст текущей кнопки
    });

    const buttonId = button.id;
    const playerInput = button.querySelector('.player-input');

    if (button_cubes[buttonId]) {
        const data = button_cubes[buttonId];
        button.querySelector('.time').textContent = data.time;
        button.querySelector('.price').textContent = `${data.price} ₽`;
        button.querySelector('.price').setAttribute('data-price', data.price);
    }

    playerInput.setAttribute('maxlength', '1');
    restrictNonNumericInput(playerInput); // Ограничение ввода только цифр
    playerInput.addEventListener('input', handleInputCubes);
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
