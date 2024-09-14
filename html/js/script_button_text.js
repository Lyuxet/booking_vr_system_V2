document.addEventListener('DOMContentLoaded', function () {
    var today = new Date();
    $('#date').datepicker({
        dateFormat: 'yy.mm.dd',
        minDate: 0
    }).datepicker("setDate", today);

    const bookingButtons = document.querySelectorAll('.booking-button');
    const maxPlayers = 10;
    const minPlayers = 1;

    function updateSeats(button, availableSeats) {
        const playerInput = button.querySelector('.player-input');
        const seatsCount = button.querySelector('.seats span');
        const maxAvailableSeats = availableSeats !== undefined ? availableSeats : maxPlayers;
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

    function updateButtonState(button) {
        // Получаем текст времени начала из кнопки
        const timeText = button.querySelector('.time').textContent.trim();
        const [startTime] = timeText.split(' - ').map(time => time.trim());
        const [startHours, startMinutes] = startTime.split(':').map(Number);
    
        // Текущее время
        const now = new Date();
        const buttonTime = new Date(now.getFullYear(), now.getMonth(), now.getDate(), startHours, startMinutes);
    
        // Получаем количество свободных мест
        const seatsCountElement = button.querySelector('.seats span');
        const availableSeats = parseInt(seatsCountElement.textContent, 10);
    
        // Проверяем, что время кнопки прошло или свободных мест 0
        if (buttonTime <= now || availableSeats === 0) {
            button.classList.add('disabled');
            button.removeEventListener('click', handleClick);
            button.querySelector('.player-input').disabled = true;
        } else {
            button.classList.remove('disabled');
            button.addEventListener('click', handleClick);
            button.querySelector('.player-input').disabled = false;
        }
    }
    

    function handleClick(event) {
        // Проверяем, что курсор находится на элементе input и кнопка уже выбрана
        if (event.target.closest('.player-input') && this.classList.contains('selected')) return;
        
        // Проверяем, содержит ли элемент класс 'disabled'
        if (this.classList.contains('disabled')) return;
        
        // Получаем элементы ввода и счетчика мест
        const playerInput = this.querySelector('.player-input');
        const maxAvailableSeats = parseInt(playerInput.getAttribute('max'), 10) || maxPlayers;
        
        // Переключаем класс 'selected' и устанавливаем значение для playerInput
        this.classList.toggle('selected');
        if (this.classList.contains('selected')) {
            playerInput.value = 1;
        } else {
            playerInput.value = '';
        }
        
        // Передаем текущую кнопку в функцию updateSeats
        updateSeats(this, maxAvailableSeats);
    }
    

    function handleInput(event) {
        const input = event.target;
        const button = input.closest('.booking-button');
        const maxAvailableSeats = parseInt(input.getAttribute('max'), 10) || maxPlayers;
        updateSeats(button, maxAvailableSeats);
    }

    function updateButtonsState(availability) {
        bookingButtons.forEach(button => {
            const timeText = button.querySelector('.time').textContent.trim();
            const availableSlotData = availability.find(slot => slot.time_game === timeText);

            if (availableSlotData) {
                const availableSeats = availableSlotData.available_slots;
                updateSeats(button, availableSeats);
                updateButtonState(button); // Обновляем состояние кнопки после получения данных
            }
        });
    }

    function checkAvailability() {
        var date = document.getElementById('date').value;
        var namegame = 'QUEST';
        var placegame = 'ARENA';

        if (!date || !namegame) {
            console.error('Заполните все поля.');
            return;
        }

        const xhr = new XMLHttpRequest();
        xhr.open('GET', `http://localhost:8080/getBookingOpenArena?placegame=${encodeURIComponent(placegame)}&date=${encodeURIComponent(date)}&namegame=${encodeURIComponent(namegame)}`, true);
        xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

        xhr.onload = function () {
            if (xhr.status >= 200 && xhr.status < 300) {
                try {
                    const availability = JSON.parse(xhr.responseText);
                    updateButtonsState(availability);
                } catch (error) {
                    console.error('Ошибка при обработке ответа:', error);
                }
            } else {
                console.error('Ошибка запроса доступности. Статус:', xhr.status);
            }
        };

        xhr.onerror = function () {
            console.error('Ошибка сети.');
        };

        xhr.send();
    }

    // Инициализация кнопок
    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');
        button.addEventListener('click', handleClick);
        playerInput.setAttribute('maxlength', '2');
        playerInput.addEventListener('input', handleInput);

        // Сразу обновляем состояние кнопок на основе текущего времени
        updateButtonState(button);
    });

    // Вызов функции для проверки доступности
    checkAvailability();
});
