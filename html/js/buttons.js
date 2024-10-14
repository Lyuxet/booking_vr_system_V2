import { calculateTotalPrice } from "./price.js";

const maxPlayersArena = 10;
const maxPlayersCubes = 4;
const minPlayers = 1;
const isCloseType = document.querySelector('.booking-container-close') !== null;


function updateSeats(button, availableSeats, maxPlayers, place) {
    const seatsCount = button.querySelector('.seats span');
    const playerInput = button.querySelector('.player-input');
    const maxAvailableSeats = availableSeats !== undefined ? availableSeats : maxPlayers;

    if (place === "ARENA" && isCloseType) {
        if (availableSeats < maxPlayers) {
            button.classList.add('disabled');
            seatsCount.textContent = 0;
        } else {
            button.classList.remove('disabled');
            seatsCount.textContent = button.classList.contains('selected') ? 0 : maxPlayers;
        }
    } else {
        const currentPlayers = parseInt(playerInput.value, 10) || 0;

        if (currentPlayers > maxAvailableSeats) {
            playerInput.value = maxAvailableSeats;
        }
        if (currentPlayers < minPlayers && playerInput.value !== '') {
            playerInput.value = minPlayers;
            playerInput.focus();
            playerInput.select();
        }

        seatsCount.textContent = maxAvailableSeats - playerInput.value;

        if (playerInput.value === '') {
            button.classList.remove('selected');
        } else {
            button.classList.add('selected');
        }

        playerInput.setAttribute('max', maxAvailableSeats);
    }
}

export function updateSeatsArena(button, availableSeats) {
    updateSeats(button, availableSeats, maxPlayersArena, "ARENA");
}

export function updateSeatsCubes(button, availableSeats) {
    updateSeats(button, availableSeats, maxPlayersCubes, "CUBES");
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

        if (place !== 'ARENA' || !isCloseType) {
            button.querySelector('.player-input').disabled = true;
            button.querySelector('.player-input').value = '';
        }
        seatsCountElement.textContent = 0;
    } else {
        button.classList.remove('disabled');
        button.addEventListener('click', handleClick.bind(button, place));

        if (place !== 'ARENA' || !isCloseType) {
            button.querySelector('.player-input').disabled = false;
        }
    }
}


export function updateButtonStateArena(button) {
    updateButtonState(button, 'ARENA');
}

export function updateButtonStateCubes(button) {
    updateButtonState(button, 'CUBES');
}


export function handleInput(event, place) {
    const input = event.target;
    const button = input.closest('.booking-button');
    let maxAvailableSeats;
    if (place === "ARENA"){
        maxAvailableSeats = parseInt(input.getAttribute('max'), 10) || maxPlayersArena;
    }
    else if (place === "CUBES"){
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

    if (place === 'ARENA') {
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
    } else if (place === "CUBES") {
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


export function updateButtonsState(availability, bookingButtons, place) {
    const updateSeatsFn = place === 'ARENA' ? updateSeatsArena : updateSeatsCubes;
    const updateButtonStateFn = place === 'ARENA' ? updateButtonStateArena : updateButtonStateCubes;
    const maxPlayers = place === 'ARENA' ? maxPlayersArena : maxPlayersCubes;

    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');
        if (playerInput) {
            playerInput.value = '';
        }
        button.classList.remove('selected');
        updateSeatsFn(button, maxPlayers);
        updateButtonStateFn(button);
    });

    if (Array.isArray(availability)) {
        bookingButtons.forEach(button => {
            const timeText = button.querySelector('.time').textContent.trim();
            const availableSlotData = availability.find(slot => slot.time_game === timeText);

            if (availableSlotData) {
                const availableSeats = availableSlotData.available_slots;
                updateSeatsFn(button, availableSeats);
                updateButtonStateFn(button);
            }
        });
    }
}