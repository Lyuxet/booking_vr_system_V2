import { updateButtonStateCubes } from "./buttons_cubes.js";
import { SetDate } from "./date.js";
import { checkAvailability } from "./availability_cubes.js";
import { handleClickCubes } from "./buttons_cubes.js";
import { handleInputCubes } from "./buttons_cubes.js";
import {button_cubes} from "./button_data.js";


document.addEventListener('DOMContentLoaded', function () {
     
    SetDate();
    const bookingButtons = document.querySelectorAll('.booking-button');

    // Инициализация кнопок
    bookingButtons.forEach(button => {
        button.addEventListener('click', handleClickCubes);
        const playerInput = button.querySelector('.player-input');
        const buttonId = button.id; 
        if (button_cubes[buttonId]) {
            const data = button_cubes[buttonId];
            button.querySelector('.time').textContent = data.time;
            button.querySelector('.price').textContent = `${data.price} ₽`;
            button.querySelector('.price').setAttribute('data-price', data.price);
        }
       
        playerInput.setAttribute('maxlength', '1');

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

        playerInput.addEventListener('input', handleInputCubes);
        
        
        updateButtonStateCubes(button);
    });

    // Обновляем цены и состояния кнопок при изменении даты
    $('#date').on('change', function () {
        const selectedDate = $('#date').datepicker('getDate');  
        checkAvailability(bookingButtons); // Проверка доступности мест
    });

    // Проверка доступности при загрузке страницы
    checkAvailability(bookingButtons);
    const initialDate = $('#date').datepicker('getDate');
});