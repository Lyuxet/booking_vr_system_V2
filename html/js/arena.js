import { updateButtonStateArena } from "./buttons_arena.js";
import { updatePricesArena } from "./price.js";
import { SetDate } from "./date.js";
import { checkAvailabilityArena } from "./availability_arena.js";
import { handleClickArena } from "./buttons_arena.js";
import { handleInputArena } from "./buttons_arena.js";
import { button_data_open_arena}  from "./button_data.js"
import { button_data_close_arena } from "./button_data.js";

document.addEventListener('DOMContentLoaded', function () {
     
    SetDate();
    const bookingButtons = document.querySelectorAll('.booking-button');
    const isCloseType = document.querySelector('.booking-container-close') !== null;


    // Инициализация кнопок
    bookingButtons.forEach(button => {
        button.addEventListener('click', handleClickArena);
        const buttonId = button.id; // Получаем id кнопки (например, 'button11')
        if (isCloseType === false){
            const playerInput = button.querySelector('.player-input');

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
                // Разрешаем клавиши для цифр (0-9, numpad)
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
        }
        else if(isCloseType === true){
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
        checkAvailabilityArena(bookingButtons); // Проверка доступности мест
    });

    // Проверка доступности при загрузке страницы
    checkAvailabilityArena(bookingButtons);
    const initialDate = $('#date').datepicker('getDate');
    updatePricesArena(initialDate, bookingButtons); // Обновление цены для начальной даты
});