import { calculateTotalPrice } from "./price.js";
import { button_data } from "./button_data.js";

const maxPlayersArena = 10;
const maxPlayersCubes = 4;
const minPlayers = 1;
const isCloseType = document.querySelector('.booking-container-close') !== null;


function updateSeats(button, availableSeats, maxPlayers, place) {
    const seatsCount = button.querySelector('.seats span');
   
    if (availableSeats == 0) {
        button.classList.add('disabled');
        seatsCount.textContent = 0;
    } else {
        button.classList.remove('disabled');
        seatsCount.textContent = button.classList.contains('selected') ? 0 : maxPlayers;
    }
    
    if (isCloseType){
        return;
    }
    const playerInput = button.querySelector('.player-input');
    const currentPlayers = parseInt(playerInput.value, 10) || 0;

    if (currentPlayers > availableSeats) {
        playerInput.value = availableSeats;
    }
    if (currentPlayers < minPlayers && playerInput.value !== '') {
        playerInput.value = minPlayers;
        playerInput.focus();
        playerInput.select();
    }

    seatsCount.textContent = availableSeats - playerInput.value;

    if (playerInput.value === '') {
        button.classList.remove('selected');
    } else {
        button.classList.add('selected');
    }

    playerInput.setAttribute('max', availableSeats);
    
}

export function updateSeatsArena(button, availableSeats) {
    updateSeats(button, availableSeats, maxPlayersArena, "VR Арена");
}

export function updateSeatsCubes(button, availableSeats) {
    updateSeats(button, availableSeats, maxPlayersCubes, "VR Кубы");
}


function updateButtonState(button, place) {
    const timeText = button.querySelector('.time').textContent.trim();
    const [startTime] = timeText.split(' - ').map(time => time.trim());
    const [startHours, startMinutes] = startTime.split(':').map(Number);

    const now = new Date();
    const selectedDateText = document.getElementById('date').value;
    const [year, month, day] = selectedDateText.split('.').map(Number);

    const buttonDate = new Date(year, month - 1, day, startHours, startMinutes);
    const seatsCountElement = button.querySelector('.seats span');
    const availableSeats = parseInt(seatsCountElement.textContent, 10);

    if (buttonDate <= now || availableSeats <= 0) {
        button.classList.remove('selected');
        button.classList.add('disabled');
        button.removeEventListener('click', handleClick.bind(button, place));

        if (place !== 'VR Арена' || !isCloseType) {
            button.querySelector('.player-input').disabled = true;
            button.querySelector('.player-input').value = '';
        }
        seatsCountElement.textContent = 0;
    } else {
        button.classList.remove('disabled');
        button.addEventListener('click', handleClick.bind(button, place));

        if (place !== 'VR Арена' || !isCloseType) {
            button.querySelector('.player-input').disabled = false;
        }
    }
}


export function updateButtonStateArena(button) {
    updateButtonState(button, 'VR Арена');
}

export function updateButtonStateCubes(button) {
    updateButtonState(button, 'VR Кубы');
}


export function handleInput(event, place) {
    const input = event.target;
    const button = input.closest('.booking-button');
    let maxAvailableSeats;
    if (place === "VR Арена"){
        maxAvailableSeats = parseInt(input.getAttribute('max'), 10) || maxPlayersArena;
    }
    else if (place === "VR Кубы"){
        maxAvailableSeats = parseInt(input.getAttribute('max'), 10) || maxPlayersCubes;
    }
    else{
        console.log("Неизвестный тип площадки");
        return;
    }
    updateSeatsArena(button, maxAvailableSeats);
    calculateTotalPrice();
}

export function handleClick(event, place) {
    if (!event.target) return;
    if (this.classList.contains('disabled')) return;

    if (place === 'VR Арена') {
        if (isCloseType) {
            this.classList.toggle('selected');
            updateSeatsArena(this, maxPlayersArena);
        } else {
            const playerInput = this.querySelector('.player-input');
            if (event.target.closest('.player-input') && this.classList.contains('selected')) return;
            const maxAvailableSeats = parseInt(playerInput?.getAttribute('max'), 10) || maxPlayersArena;
            this.classList.toggle('selected');
            if (this.classList.contains('selected')) {
                if (playerInput) playerInput.value = minPlayers;
            } else {
                if (playerInput) playerInput.value = '';
            }
            updateSeatsArena(this, maxAvailableSeats);
        }
    } else if (place === "VR Кубы") {
        const playerInput = this.querySelector('.player-input');
        if (event.target.closest('.player-input') && this.classList.contains('selected')) return;
        const maxAvailableSeats = parseInt(playerInput.getAttribute('max'), 10) || maxPlayersCubes;
        this.classList.toggle('selected');
        if (this.classList.contains('selected')) {
            playerInput.value = minPlayers;
        } else {
            playerInput.value = '';
        }
        updateSeatsCubes(this, maxAvailableSeats);
    } else {
        console.log("Неизвестный тип площадки кнопка");
        console.log(place);
        return;
    }
    calculateTotalPrice();
}


export function updateButtonsState(data, bookingButtons, place) {
    const updateSeatsFn = place === 'VR Арена' ? updateSeatsArena : updateSeatsCubes;
    const updateButtonStateFn = place === 'VR Арена' ? updateButtonStateArena : updateButtonStateCubes;
    const maxPlayers = place === 'VR Арена' ? maxPlayersArena : maxPlayersCubes;

    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');
        if (playerInput) {
            playerInput.value = '';
        }
        button.classList.remove('selected');
        updateSeatsFn(button, maxPlayers);
        updateButtonStateFn(button);
    });

    const availability = typeof data === 'string' ? JSON.parse(data) : data; // Проверка, является ли data строкой или объектом
    bookingButtons.forEach(button => {  
        const buttonId = button.getAttribute('id');
        const slotData = availability[buttonId];

        if (slotData) {
            button_data[buttonId] = {
                time: slotData.time,
                price: slotData.price,
                availability: slotData.availability_place
            };
            //В последствии доработать, что бы количествои мест расчитывалось на серевере и прайс на выходные и праздничные дни долнжен быть на сервере 
            button.querySelector('.time').textContent = button_data[buttonId].time;
            button.querySelector('.price').textContent = `${button_data[buttonId].price} ₽`;
            button.querySelector('.price').setAttribute('data-price', button_data[buttonId].price);
            const availableSeats = button_data[buttonId].availability;
            updateSeatsFn(button, availableSeats);
            updateButtonStateFn(button);
        }
    });
}
export function initializeBookingButton(button, place, isCloseType) {
    button.addEventListener('click', function(event) {
        handleClick.call(this, event, place);
    });

    const playerInput = button.querySelector('.player-input');
    
    if (!isCloseType) {
        playerInput.setAttribute('maxlength', '2');
        restrictNonNumericInput(playerInput);
        playerInput.addEventListener('input', function(event) {
            handleInput.call(this, event, place);
        });
    }
}

function restrictNonNumericInput(input) {
    input.addEventListener('input', function() {
        this.value = this.value.replace(/[^0-9]/g, '');
    });

    input.addEventListener('keydown', function(event) {
        const isNumberKey = (event.keyCode >= 48 && event.keyCode <= 57) ||
                            (event.keyCode >= 96 && event.keyCode <= 105) ||
                            event.keyCode === 8 ||
                            event.keyCode === 46;
        if (!isNumberKey) {
            event.preventDefault();
        }
    });

    input.addEventListener('paste', function(event) {
        const pastedData = (event.clipboardData || window.clipboardData).getData('text');
        if (!/^\d*$/.test(pastedData)) {
            event.preventDefault();
        }
    });
}