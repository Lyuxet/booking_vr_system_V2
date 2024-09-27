import { showPriceDisplay } from "./priceDisplay.js";
import { hidePriceDisplay } from "./priceDisplay.js";
import { isWeekendOrHoliday } from "./date.js";

const isCloseType = document.querySelector('.booking-container-close') !== null;

export function updatePricesArena(date, bookingButtons) {
    const isSpecialDay = isWeekendOrHoliday(date);
    bookingButtons.forEach(button => {
        const priceElement = button.querySelector('.price'); // Получаем элемент цены
        let basePrice = priceElement.dataset.price; // Получаем базовую цену из data-атрибута

        // Устанавливаем цену в зависимости от условий
        basePrice = isSpecialDay ? (isCloseType ? 15000 : 1600) : basePrice;
        

        // Обновляем текстовое содержимое элемента с учётом символа ₽
        priceElement.textContent = `${basePrice} ₽`;
    });
}


export function calculateTotalPrice() {
    let totalPrice = 0;

    if (isCloseType) {

        document.querySelectorAll('.booking-button.selected').forEach(button => {
            const fixedPricePerSelection = parseInt(button.querySelector('.price').textContent);
            totalPrice += fixedPricePerSelection;
        });

        
    } else {
        document.querySelectorAll('.booking-button.selected').forEach(button => {
            const playerInput = button.querySelector('.player-input');
            const playerCount = parseInt(playerInput.value, 10) || 0;
            const pricePerPlayer = parseInt(button.querySelector('.price').textContent);

            // Добавляем к общей стоимости
            totalPrice += playerCount * pricePerPlayer;
        });
    }

    // Обновляем содержимое таблички
    const displayAmountElement = document.getElementById('displayAmount');
    if (displayAmountElement) {
        displayAmountElement.textContent = totalPrice;
    }

    // Проверяем, выбрана ли хотя бы одна кнопка
    const anyButtonSelected = document.querySelectorAll('.booking-button.selected').length > 0;

    const priceDisplayElement = document.querySelector('.price-display');
    if (priceDisplayElement) {
        // Показываем или скрываем табличку в зависимости от состояния кнопок
        if (anyButtonSelected) {
            showPriceDisplay();
        } else {
            hidePriceDisplay();
        }
    }
}