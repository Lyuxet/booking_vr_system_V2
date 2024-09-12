document.addEventListener('DOMContentLoaded', function () {
    const datePicker = document.querySelector('.date-picker');
    if (datePicker) {
        const today = new Date().toISOString().split('T')[0]; // Форматируем дату в формате YYYY-MM-DD
        datePicker.value = today;
    }

    datePicker.addEventListener('focus', function () {
        this.select(); // Выделяем все содержимое поля при фокусе
    });

    // Получаем все кнопки бронирования
    const bookingButtons = document.querySelectorAll('.booking-button');
    const maxPlayers = 10;
    const minPlayers = 1;

    function updateSeats(button, availableSeats) {
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

        // Устанавливаем количество свободных мест, полученное с сервера
        seatsCount.textContent = availableSeats !== undefined ? availableSeats : Math.max(0, 10 - currentPlayers);

        if (playerInput.value === '') {
            button.classList.remove('selected'); // Убираем состояние "выбрано"
        } else {
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
            playerInput.value = 1;
        } else {
            playerInput.value = '';
        }

        updateSeats(this);
    }

    function updateButtonsState(availability) {
        bookingButtons.forEach((button, index) => {
            const availableSeats = availability[index]; // Получаем доступные места для текущей кнопки
            updateSeats(button, availableSeats); // Обновляем места на кнопке
        });
    }


    function checkAvailability() {
        // Get date and game name
        var date = document.getElementById('date').value;
        var namegame = 'QUEST'; // Example value
    
        if (!date || !namegame) {
            console.error('Заполните все поля.');
            return;
        }
    
        // Create XMLHttpRequest
        const xhr = new XMLHttpRequest();
        xhr.open('GET', `http://localhost:8080/getBookingOpenArena?date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}`, true);
        xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    
        xhr.onload = function () {
            if (xhr.status >= 200 && xhr.status < 300) {
                const response = xhr.responseText;
                console.log(response); // Handle the response
            } else {
                console.error('Ошибка запроса доступности. Статус:', xhr.status);
            }
        };
    
        xhr.onerror = function () {
            console.error('Ошибка сети.');
        };
    
        xhr.send(); // Send the request
    }
    



















    

    // Инициализация кнопок
    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');
        //updateButtonState(button);
        button.addEventListener('click', handleClick);
        playerInput.setAttribute('maxlength', '2');
        playerInput.addEventListener('input', () => updateSeats(button));
        updateSeats(button);
    });

    // Вызов функции для проверки доступности
    checkAvailability();
});
