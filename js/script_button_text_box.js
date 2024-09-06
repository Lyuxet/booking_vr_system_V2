document.addEventListener('DOMContentLoaded', () => {
    // Получаем все кнопки бронирования
    const bookingButtons = document.querySelectorAll('.booking-button');
    const maxPlayers = 10;
    const totalSeats = 10; // Общее количество мест

    function updateSeats(button) {
        // Для каждой кнопки получаем текстовое поле и счетчик мест
        const playerInput = button.querySelector('.player-input');
        const seatsCount = button.querySelector('.seats span');
        
        
        const currentPlayers = parseInt(playerInput.value, 10) || 0;
        if (currentPlayers > maxPlayers) {
            playerInput.value = maxPlayers;
        }
        const availableSeats = Math.max(0, totalSeats - currentPlayers); // Расчет свободных мест
        seatsCount.textContent = availableSeats; // Обновление отображаемого количества свободных мест
    }

    
    function updateButtonState(button) {
        const timeText = button.querySelector('.time').textContent.trim();
        const [startTime] = timeText.split(' - ').map(time => time.trim());
        const [startHours, startMinutes] = startTime.split(':').map(Number);

        const now = new Date();
        const buttonTime = new Date(now.getFullYear(), now.getMonth(), now.getDate(), startHours, startMinutes);

        if (buttonTime <= now) {
            button.classList.add('disabled');
            button.removeEventListener('click', handleClick);
            button.querySelector('.player-input').disabled = true; // Блокируем текстовое поле
        } else {
            button.classList.remove('disabled');
            button.addEventListener('click', handleClick);
            button.querySelector('.player-input').disabled = false; // Разблокируем текстовое поле
        }
    }
    

    function handleClick() {
        if (this.classList.contains('disabled')) return;

        const playerInput = this.querySelector('.player-input');
        this.classList.toggle('selected');
        
        if (this.classList.contains('selected')) {
            // Если кнопка выбрана, установить значение "1" в текстовое поле
            playerInput.value = 1;
        } else {
            // Если кнопка не выбрана, очистить текстовое поле
            playerInput.value = '';
        }

        // Обновляем количество свободных мест
        updateSeats(this);
    }

    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');

        // Закомментируйте следующую строку, если не хотите обрабатывать состояние кнопок
        // updateButtonState(button);
        button.addEventListener('click', handleClick);
        // Обновляем места при изменении значения в текстовом поле
        playerInput.addEventListener('input', () => updateSeats(button));

        // Инициализация начального значения
        updateSeats(button);
    });
});
