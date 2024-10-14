import { updatePricesArena } from "./price.js";
import { SetDate } from "./date.js";
import { checkAvailability } from "./availability.js";
import { handleClick, handleInput } from "./buttons.js";
import { button_data_open_arena, button_data_close_arena } from "./button_data.js";
import { hidePriceDisplay } from "./priceDisplay.js";

const bookingButtons = document.querySelectorAll('.booking-button');
let socket = null; // Глобальная переменная для WebSocket
let storedDate = null; // Глобальная переменная для хранения даты
const place = 'ARENA';

document.addEventListener('DOMContentLoaded', function () {
    SetDate();
    storedDate = document.getElementById('date').value;
    initializeWebSocket(place, storedDate);
    const isCloseType = document.querySelector('.booking-container-close') !== null;

    window.onbeforeunload = function() {
        if (socket) {
            socket.close();
        }
    };

    document.addEventListener('visibilitychange', function() {
        if (document.visibilityState === 'visible' && socket.readyState !== WebSocket.OPEN) {
            if (socket) {
                socket.close();
            }
            initializeWebSocket(place, storedDate);
        }
    });

    bookingButtons.forEach(button => {
        initializeBookingButton(button, socket, place, isCloseType);
    });

    $('#date').on('change', function () {
        const selectedDate = $('#date').datepicker('getDate');
        const year = selectedDate.getFullYear();
        const month = ('0' + (selectedDate.getMonth() + 1)).slice(-2);
        const day = ('0' + selectedDate.getDate()).slice(-2);
        storedDate = `${year}.${month}.${day}`;
        if (socket) {
            socket.close();
        }
        initializeWebSocket(place, storedDate);
        updatePricesArena(selectedDate, bookingButtons);
        hidePriceDisplay();
    });

    const initialDate = $('#date').datepicker('getDate');
    updatePricesArena(initialDate, bookingButtons);
});

function initializeWebSocket(arena, currentDate) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        return;
    }
    socket = new WebSocket('ws://localhost:8082/ws');
    socket.addEventListener('open', function() {
        const data = JSON.stringify({ place: arena, date: currentDate });
        socket.send(data);
        checkAvailability(bookingButtons, place);
    });
    socket.addEventListener('close', function(event) {
        if (event.code === 1006) {
            setTimeout(function() {
                if (!socket || socket.readyState !== WebSocket.OPEN) {
                    initializeWebSocket(arena, currentDate);
                }
            }, 3000);
        } 
    });
    socket.addEventListener('message', function(event) {
        try {
            checkAvailability(bookingButtons, place);
        } catch (error) {
            console.error('Ошибка при обработке сообщения:', error);
        }
    });
    socket.addEventListener('error', function(error) {
        console.error('Ошибка WebSocket:', error);
    });
}

function initializeBookingButton(button, socket, place, isCloseType) {
    button.addEventListener('click', function(event) {
        handleClick.call(this, event, place); // Используем контекст текущей кнопки
        const buttonId = this.id; // Получаем id кнопки
        const playerInput = this.querySelector('.player-input');
        const playerCount = playerInput.value;
        const bookingData = { buttonId: buttonId, players: playerCount };

        if (socket.readyState === WebSocket.OPEN) {
            socket.send(JSON.stringify(bookingData)); // Отправляем данные на сервер
        } else {
            console.warn('WebSocket не открыт. Не удается отправить данные.');
        }
    });

    const buttonId = button.id;
    const playerInput = button.querySelector('.player-input');
    const buttonData = place === 'ARENA' ? (isCloseType ? button_data_close_arena : button_data_open_arena) : button_cubes;

    if (buttonData[buttonId]) {
        const data = buttonData[buttonId];
        button.querySelector('.time').textContent = data.time;
        button.querySelector('.price').textContent = `${data.price} ₽`;
        button.querySelector('.price').setAttribute('data-price', data.price);
    }

    if (!isCloseType){
        playerInput.setAttribute('maxlength', '2');
        restrictNonNumericInput(playerInput);
        playerInput.addEventListener('input', function(event) {
            handleInput.call(this, event, place); // Используем контекст текущей кнопки
        });
    }

    
}

function restrictNonNumericInput(input) {
    input.addEventListener('input', function() {
        this.value = this.value.replace(/[^0-9]/g, ''); // Удаление всех символов кроме цифр
    });
    input.addEventListener('keydown', function(event) {
        const isNumberKey = (event.keyCode >= 48 && event.keyCode <= 57) || // цифры сверху
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
