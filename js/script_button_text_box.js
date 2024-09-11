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
    const totalSeats = 10; // Общее количество мест

    function updateSeats(button) {
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

    function checkAvailability() {
        // Создаем запрос через XMLHttpRequest

        var date = document.getElementById('date').value;
        
        const xhr = new XMLHttpRequest();
        xhr.open('GET', 'http://localhost:8080/checkAvailability', true); // Замените URL на нужный вам
        xhr.onload = function () {
            if (xhr.status >= 200 && xhr.status < 300) {
                const response = JSON.parse(xhr.responseText);
                updateButtonsState(response); // Обновляем состояние кнопок на основе ответа
            } else {
                console.error('Ошибка запроса доступности. Статус:', xhr.status);
            }
        };
        xhr.onerror = function () {
            console.error('Ошибка сети.');
        };
        xhr.send();
    }

    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');
        updateButtonState(button);
        button.addEventListener('click', handleClick);
        playerInput.setAttribute('maxlength', '2');
        playerInput.addEventListener('input', () => updateSeats(button));
        updateSeats(button);
    });

    // Вызов функции для проверки доступности
    checkAvailability();
});
