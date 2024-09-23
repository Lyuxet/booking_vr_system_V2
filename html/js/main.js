import { updateButtonState } from "./buttons.js";
import { updatePrices } from "./price.js";
import { SetDate } from "./date.js";
import { checkAvailability } from "./availability.js";
import { handleClick } from "./buttons.js";
import { handleInput } from "./buttons.js";

document.addEventListener('DOMContentLoaded', function () {
    
   
    console.log("test0"); 
    SetDate();
    console.log("test1");
    const bookingButtons = document.querySelectorAll('.booking-button');
    const isCloseType = document.querySelector('.booking-container-close') !== null;


    // Инициализация кнопок
    bookingButtons.forEach(button => {
        button.addEventListener('click', handleClick);
        if (isCloseType === false){
            const playerInput = button.querySelector('.player-input');
       
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

            playerInput.addEventListener('input', handleInput);
        }
        
        updateButtonState(button, isCloseType);
    });

    // Обновляем цены и состояния кнопок при изменении даты
    $('#date').on('change', function () {
        const selectedDate = $('#date').datepicker('getDate');  
        updatePrices(selectedDate, bookingButtons); // Обновление цены
        checkAvailability(bookingButtons); // Проверка доступности мест
    });

    // Проверка доступности при загрузке страницы
    checkAvailability(bookingButtons);
    const initialDate = $('#date').datepicker('getDate');
    updatePrices(initialDate, bookingButtons); // Обновление цены для начальной даты
});