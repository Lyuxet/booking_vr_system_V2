import { checkAvailability } from "./availability.js";
import { hidePriceDisplay } from "./priceDisplay.js";

export let socket = null;

export function initializeWebSocket(place, currentDate, bookingButtons) {
    if (socket && socket.readyState === WebSocket.OPEN) {
        return;
    }

    socket = new WebSocket('ws://localhost:8082/ws');

    socket.addEventListener('open', function() {
        const data = JSON.stringify({ place: place, date: currentDate });
        socket.send(data);
        checkAvailability(bookingButtons, place);
        hidePriceDisplay();
    });

    socket.addEventListener('close', function(event) {
        if (event.code === 1006) {
            setTimeout(function() {
                if (!socket || socket.readyState !== WebSocket.OPEN) {
                    initializeWebSocket(place, currentDate, bookingButtons);
                }
            }, 3000);
        }
    });

    socket.addEventListener('message', function() {
        try {
            checkAvailability(bookingButtons, place);
            hidePriceDisplay();
        } catch (error) {
            console.error('Ошибка при обработке сообщения:', error);
        }
    });

    socket.addEventListener('error', function(error) {
        console.error('Ошибка WebSocket:', error);
    });
}
