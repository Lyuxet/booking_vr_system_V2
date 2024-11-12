import { updateButtonsState } from "./buttons.js";

const bookingGrid = document.querySelector('.booking-grid');
const pacmanContainer = document.querySelector('.pac-man-container');

export function checkAvailability(bookingButtons, place, isCloseType) {
    
    const date = document.getElementById('date').value;
    
    
    const gameTitleElement = document.querySelector('.navigation h1');
    const namegame = gameTitleElement ? gameTitleElement.textContent.trim() : 'На Ваш выбор';
    
    bookingGrid.style.display = 'none'; 
    pacmanContainer.style.display = 'flex'; 

    console.log(isCloseType);
    const xhr = new XMLHttpRequest();
    const url = place === 'VR Арена' ? 
        `http://localhost:8081/getBookingOpenArena?placegame=${encodeURIComponent(place)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}&typegame=${encodeURIComponent(isCloseType)}` :
        `http://localhost:8081/getBookingCubes?placegame=${encodeURIComponent(place)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}&typegame=${encodeURIComponent(isCloseType)}`;

    xhr.open('GET', url, true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

    const startTime = Date.now(); 

    xhr.onload = function () {
        const elapsedTime = Date.now() - startTime;
        const minDuration = 750; 

        const handleResponse = () => {
            bookingGrid.style.display = 'grid'; 
            pacmanContainer.style.display = 'none'; 

            if (xhr.status >= 200 && xhr.status < 300) {
                try {
                    const availability = JSON.parse(xhr.responseText);
                    updateButtonsState(availability, bookingButtons, place);
                } catch (error) {
                    console.error('Ошибка при обработке ответа:', error);
                }
            } else {
                console.error('Ошибка запроса доступности. Статус:', xhr.status);
            }
        };

        if (elapsedTime < minDuration) {
            setTimeout(handleResponse, minDuration - elapsedTime);
        } else {
            handleResponse();
        }
    };

    xhr.onerror = function () {
        bookingGrid.style.display = 'grid';
        pacmanContainer.style.display = 'none';
        console.error('Ошибка сети.');
    };

    xhr.send();
}
