import { showPriceDisplay } from "./priceDisplay.js";
import { hidePriceDisplay } from "./priceDisplay.js";
import { isWeekendOrHoliday } from "./date.js";

const isCloseType = document.querySelector('.booking-container-close') !== null;

export function updatePricesArena(date, bookingButtons) {
    const isSpecialDay = isWeekendOrHoliday(date);
    bookingButtons.forEach(button => {
        const priceElement = button.querySelector('.price'); 
        let basePrice = priceElement.dataset.price; 
        basePrice = isSpecialDay ? (isCloseType ? 15000 : 1600) : basePrice;
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

          
            totalPrice += playerCount * pricePerPlayer;
        });
    }

    
    const displayAmountElement = document.getElementById('displayAmount');
    if (displayAmountElement) {
        displayAmountElement.textContent = totalPrice;
    }

    const anyButtonSelected = document.querySelectorAll('.booking-button.selected').length > 0;

    const priceDisplayElement = document.querySelector('.price-display');
    if (priceDisplayElement) {
        if (anyButtonSelected) {
            showPriceDisplay();
        } else {
            hidePriceDisplay();
        }
    }
}