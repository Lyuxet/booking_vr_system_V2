import { showPriceDisplay } from "./priceDisplay.js";
import { hidePriceDisplay } from "./priceDisplay.js";
import { button_data } from "./button_data.js";

const isCloseType = document.querySelector('.booking-container-close') !== null;

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
            const buttonId = button.getAttribute('id');
            const pricePerPlayer = button_data[buttonId].price;

          
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