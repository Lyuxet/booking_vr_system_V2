import { calculateTotalPrice } from "./price.js";

const maxPlayersArena = 10;
const minPlayers = 1;

function updateSeatsArena(button, availableSeats, isCloseType) {
    const seatsCount = button.querySelector('.seats span');
    
    if (isCloseType) {
        // Проверяем доступные места и блокируем кнопку, если меньше 10 свободных мест
        if (availableSeats < maxPlayersArena) {
            button.classList.add('disabled');
            seatsCount.textContent = 0;
            return; // Прекращаем выполнение функции
        } else {
            button.classList.remove('disabled');
            seatsCount.textContent = button.classList.contains('selected') ? 0 : maxPlayersArena;
        }
    } else {
        const playerInput = button.querySelector('.player-input');
        const maxAvailableSeats = availableSeats !== undefined ? availableSeats : maxPlayersArena;
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

        button.classList.toggle('selected', playerInput.value !== '');
        playerInput.setAttribute('max', maxAvailableSeats);
    }
}

export function updateButtonStateArena(button, isCloseType) {
    const timeText = button.querySelector('.time').textContent.trim();
    const [startTime] = timeText.split(' - ').map(time => time.trim());
    const [startHours, startMinutes] = startTime.split(':').map(Number);

    // Получаем текущую дату
    const now = new Date();

    // Получаем выбранную дату (например, из input с id='date', где пользователь выбирает дату игры)
    const selectedDateText = document.getElementById('date').value;
    const [year, month, day] = selectedDateText.split('.').map(Number);  // Формат даты yy.mm.dd

    // Создаем объект даты и времени для кнопки
    const buttonDate = new Date(year, month - 1, day, startHours, startMinutes); // month - 1, потому что месяцы в JS идут с 0

    // Получаем доступные места
    const seatsCountElement = button.querySelector('.seats span');
    const availableSeats = parseInt(seatsCountElement.textContent, 10);
    
    // Если дата игры уже прошла или мест нет, кнопка отключена
    if (buttonDate <= now || availableSeats <= 0) {
        button.classList.remove('selected');
        button.classList.add('disabled');
        button.removeEventListener('click', handleClickArena); // Удаляем слушатель, чтобы избежать утечек памяти
        if (!isCloseType){
            button.querySelector('.player-input').disabled = true;
            button.querySelector('.player-input').value = '';
        }
        seatsCountElement.textContent = 0;
    } else {
        // Если дата и время еще актуальны, и есть места — кнопка активна
        button.classList.remove('disabled');
        button.addEventListener('click', handleClickArena.bind(button, isCloseType)); // Привязываем контекст кнопки
        if (!isCloseType){
            button.querySelector('.player-input').disabled = false;
        }
    }
}

export function handleInputArena(event) {
    const input = event.target;
    const button = input.closest('.booking-button');
    const maxAvailableSeats = parseInt(input.getAttribute('max'), 10) || maxPlayersArena;
    updateSeatsArena(button, maxAvailableSeats);
    calculateTotalPrice();
}

export function handleClickArena(event, isCloseType) {
    const button = event.currentTarget; // Используем event.currentTarget вместо this

    // Проверяем, отключена ли кнопка
    if (button.classList.contains('disabled')) return;

    // Проверяем, был ли клик по полю ввода
    const playerInput = button.querySelector('.player-input'); // Получаем поле ввода
    if (event.target === playerInput && button.classList.contains('selected')) return;

    // Проверяем, является ли тип закрытым
    if (isCloseType) {
        // Если тип "CLOSE", просто переключаем состояние кнопки
        button.classList.toggle('selected');
        updateSeatsArena(button, maxPlayersArena, isCloseType); // Используем максимальное количество мест
    } else {
        button.classList.toggle('selected');
        if (button.classList.contains('selected')) {
            playerInput.value = 1;
        } else {
            playerInput.value = '';
        }

        const maxAvailableSeats = parseInt(playerInput.getAttribute('max'), 10) || maxPlayersArena;
        updateSeatsArena(button, maxAvailableSeats); // Обновляем состояние мест
    }

    calculateTotalPrice(); // Обновляем общую цену
}





export function updateButtonsStateArena(availability, bookingButtons, isCloseType) {
    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');
        if (playerInput) {
            playerInput.value = ''; 
        }
        button.classList.remove('selected'); 

        updateSeatsArena(button, maxPlayersArena, isCloseType);
        updateButtonStateArena(button, isCloseType); 
    });

    if (Array.isArray(availability)) {
        bookingButtons.forEach(button => {
            const timeText = button.querySelector('.time').textContent.trim();
            const availableSlotData = availability.find(slot => slot.time_game === timeText);
            
            if (availableSlotData) {
                const availableSeats = availableSlotData.available_slots;
                updateSeatsArena(button, availableSeats, isCloseType);
                updateButtonStateArena(button, isCloseType);
            }
        });
    }
}
