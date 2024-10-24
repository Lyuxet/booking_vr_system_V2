import { SetDate } from "./date.js";
import { updatePricesArena } from "./price.js";
import { initializeBookingButton } from "./buttons.js";
import { hidePriceDisplay } from "./priceDisplay.js";
import { initializeWebSocket, socket } from "./websocket.js";

const bookingButtons = document.querySelectorAll('.booking-button');
let storedDate = null;
const place = 'ARENA';

document.addEventListener('DOMContentLoaded', function () {
    SetDate();
    storedDate = document.getElementById('date').value;
    const isCloseType = document.querySelector('.booking-container-close') !== null;

    initializeWebSocket(place, storedDate, bookingButtons);

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
            initializeWebSocket(place, storedDate, bookingButtons);
        }
    });

    bookingButtons.forEach(button => {
        initializeBookingButton(button, place, isCloseType);
    });

    $('#date').on('change', function () {
        const selectedDate = $('#date').datepicker('getDate');
        const year = selectedDate.getFullYear();
        const month = ('0' + (selectedDate.getMonth() + 1)).slice(-2);
        const day = ('0' + selectedDate.getDate()).slice(-2);
        storedDate = `${year}.${month}.${day}`;
        hidePriceDisplay();
        if (socket) {
            socket.close();
        }
        initializeWebSocket(place, storedDate, bookingButtons);
        updatePricesArena(selectedDate, bookingButtons);
       
    });
});
