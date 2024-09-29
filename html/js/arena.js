import { updateButtonStateArena } from "./buttons_arena.js";
import { updatePricesArena } from "./price.js";
import { SetDate } from "./date.js";
import { checkAvailabilityArena } from "./availability_arena.js";
import { handleClickArena, handleInputArena } from "./buttons_arena.js";
import { button_data_open_arena, button_data_close_arena } from "./button_data.js";

document.addEventListener('DOMContentLoaded', function () {
    SetDate();
    const bookingButtons = document.querySelectorAll('.booking-button');
    const isCloseType = document.querySelector('.booking-container-close') !== null;

    // Инициализация кнопок
    bookingButtons.forEach(button => {
        button.addEventListener('click', event => handleClickArena(event, isCloseType));
        const buttonId = button.id; // Получаем id кнопки (например, 'button11')
        const playerInput = button.querySelector('.player-input');

        if (!isCloseType) {
            if (button_data_open_arena[buttonId]) {
                const data = button_data_open_arena[buttonId];
                button.querySelector('.time').textContent = data.time;
                button.querySelector('.price').textContent = `${data.price} ₽`;
                button.querySelector('.price').setAttribute('data-price', data.price);
            }

            playerInput.setAttribute('maxlength', '2');

            // Запретить ввод нецифровых символов
            playerInput.addEventListener('input', function() {
                this.value = this.value.replace(/[^0-9]/g, '');
            });

            // Запретить ввод нецифровых символов с клавиатуры, включая numpad
            playerInput.addEventListener('keydown', function(event) {
                const isNumberKey = (event.keyCode >= 48 && event.keyCode <= 57) || // цифры сверху
                                    (event.keyCode >= 96 && event.keyCode <= 105) || // цифры numpad
                                    event.keyCode === 8 || 
                                    event.keyCode === 46; 
                
                if (!isNumberKey) {
                    event.preventDefault();
                }
            });

            // Запретить вставку нецифровых символов
            playerInput.addEventListener('paste', function(event) {
                const pastedData = (event.clipboardData || window.clipboardData).getData('text');
                if (!/^\d*$/.test(pastedData)) {
                    event.preventDefault();
                }
            });

            playerInput.addEventListener('input', handleInputArena);
        } else {
            if (button_data_close_arena[buttonId]) {
                const data = button_data_close_arena[buttonId];
                button.querySelector('.time').textContent = data.time;
                button.querySelector('.price').textContent = `${data.price} ₽`;
                button.querySelector('.price').setAttribute('data-price', data.price);
            }
        }

        updateButtonStateArena(button, isCloseType);
    });

    // Обновляем цены и состояния кнопок при изменении даты
    $('#date').on('change', function () {
        const selectedDate = $('#date').datepicker('getDate');
        updatePricesArena(selectedDate, bookingButtons); // Обновление цены
        checkAvailabilityArena(bookingButtons, isCloseType); // Проверка доступности мест
    });

    // Проверка доступности при загрузке страницы
    checkAvailabilityArena(bookingButtons, isCloseType);
    const initialDate = $('#date').datepicker('getDate');
    updatePricesArena(initialDate, bookingButtons); // Обновление цены для начальной даты
});
