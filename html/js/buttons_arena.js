import { calculateTotalPrice } from "./price.js";

const maxPlayersArena = 10;
const minPlayers = 1;
const isCloseType = document.querySelector('.booking-container-close') !== null;


function updateSeatsArena(button, availableSeats) {
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

export function updateButtonStateArena(button) {
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
        button.addEventListener('click', handleClickArena); // Привязываем контекст кнопки
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

export function handleClickArena(event) {
    // Проверка наличия event.target
    if (!event.target) return; 

    if (this.classList.contains('disabled')) return;

    if (isCloseType) {
        // Для CLOSE типа игры, не использовать поле ввода
        this.classList.toggle('selected');
        updateSeatsArena(this, maxPlayersArena); // Используем максимальное количество мест
    } else {
        const playerInput = this.querySelector('.player-input');
        
        // Убедитесь, что playerInput существует перед его использованием
        if (event.target.closest('.player-input') && this.classList.contains('selected')) return;

        const maxAvailableSeats = parseInt(playerInput?.getAttribute('max'), 10) || maxPlayersArena;

        this.classList.toggle('selected');
        if (this.classList.contains('selected')) {
            if (playerInput) {
                playerInput.value = 1; // Устанавливаем 1 только если playerInput существует
            }
        } else {
            if (playerInput) {
                playerInput.value = ''; // Сбрасываем только если playerInput существует
            }
        }

        updateSeatsArena(this, maxAvailableSeats);
    }

    calculateTotalPrice();
}


export function updateButtonsStateArena(availability, bookingButtons) {
    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');
        if (playerInput) {
            playerInput.value = ''; 
        }
        button.classList.remove('selected'); 

        updateSeatsArena(button, maxPlayersArena);
        updateButtonStateArena(button); 
    });

    if (Array.isArray(availability)) {
        bookingButtons.forEach(button => {
            const timeText = button.querySelector('.time').textContent.trim();
            const availableSlotData = availability.find(slot => slot.time_game === timeText);
            
            if (availableSlotData) {
                const availableSeats = availableSlotData.available_slots;
                updateSeatsArena(button, availableSeats);
                updateButtonStateArena(button);
            }
        });
    }
}
