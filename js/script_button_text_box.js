document.addEventListener('DOMContentLoaded', function () {
    const datePicker = document.querySelector('.date-picker');

    // Добавляем обработчик событий для фокуса на элемент
    datePicker.addEventListener('focus', function () {
        this.select(); // Выделяем все содержимое поля при фокусе
    });
});



document.addEventListener('DOMContentLoaded', () => {
    // Получаем все кнопки бронирования
    const bookingButtons = document.querySelectorAll('.booking-button');
    const maxPlayers = 10;
    const minPlayers = 1;
    const totalSeats = 10; // Общее количество мест

    function updateSeats(button) {
        // Для каждой кнопки получаем текстовое поле и счетчик мест
        const playerInput = button.querySelector('.player-input');
        const seatsCount = button.querySelector('.seats span');

        
        
        const currentPlayers = parseInt(playerInput.value, 10) || 0;
        if (currentPlayers > maxPlayers) {
            playerInput.value = maxPlayers;
        }
        if (currentPlayers < minPlayers && playerInput.value !== '') {
            playerInput.value = minPlayers;
            playerInput.focus(); // Устанавливаем фокус на текстовое поле
            playerInput.select();
        }
        const availableSeats = Math.max(0, totalSeats - currentPlayers); // Расчет свободных мест
        seatsCount.textContent = availableSeats; // Обновление отображаемого количества свободных мест

        // Устанавливаем кнопку как "не нажата", если текстовое поле пустое
        if (playerInput.value === '') {
            button.classList.remove('selected'); // Убираем состояние "выбрано"
        }
        if (playerInput.value !== ''){
            button.classList.add('selected');
        }

        playerInput.addEventListener('click', (event) => {
            if (button.classList.contains('selected')) {
                event.stopPropagation(); // Останавливаем всплытие события клика, если кнопка активна
                playerInput.focus(); // Устанавливаем фокус на текстовое поле
                playerInput.select();
            }
        });
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

        // Активируем проверку состояния кнопок
        updateButtonState(button);

        // Добавляем обработчик клика по кнопке
        button.addEventListener('click', handleClick);

        playerInput.setAttribute('maxlength', '2'); // Ограничение на 2 символа

        // Обновляем места при изменении значения в текстовом поле
        playerInput.addEventListener('input', () => updateSeats(button));

        // Предотвращаем клик на текстовом поле от воздействия на кнопку
        
        // Инициализация начального значения
        updateSeats(button);
    });
});
