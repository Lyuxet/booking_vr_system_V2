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
            button.removeEventListener('click', handleClick);
            button.querySelector('.player-input').disabled = true;
            button.querySelector('.player-input').value = '';
            button.querySelector('.seats span').textContent = 0;
        } else {
            // Если дата и время еще актуальны, и есть места — кнопка активна
            button.classList.remove('disabled');
            button.addEventListener('click', handleClick);
            button.querySelector('.player-input').disabled = false;
        }
    }
    

    function handleClick(event) {
        if (event.target.closest('.player-input') && this.classList.contains('selected')) return;
        if (this.classList.contains('disabled')) return;

        const playerInput = this.querySelector('.player-input');
        const maxAvailableSeats = parseInt(playerInput.getAttribute('max'), 10) || maxPlayers;

        this.classList.toggle('selected');
        if (this.classList.contains('selected')) {
            playerInput.value = 1;
        } else {
            playerInput.value = '';
        }

        updateSeats(this, maxAvailableSeats);
    }

    function handleInput(event) {
        const input = event.target;
        const button = input.closest('.booking-button');
        const maxAvailableSeats = parseInt(input.getAttribute('max'), 10) || maxPlayers;
        updateSeats(button, maxAvailableSeats);
    }

    // Если данные пустые, сделаем все кнопки доступными, если время актуально
    function makeAllButtonsAvailable() {
        bookingButtons.forEach(button => {
            const seatsCountElement = button.querySelector('.seats span');
            seatsCountElement.textContent = maxPlayers;
            updateSeats(button, maxPlayers);
            updateButtonState(button);
        });
    }

    function updateButtonsState(availability) {
        // Сначала сбросить состояние всех кнопок
        bookingButtons.forEach(button => {
            // Сбросить состояние кнопки и обновить ее внешний вид на основе текущего времени и даты
            updateSeats(button); // Обновить счетчик мест и состояние
            updateButtonState(button); // Обновить состояние кнопки (доступность)
        });
    
        // Затем обновить кнопки на основе данных из JSON
        if (Array.isArray(availability)) {
            bookingButtons.forEach(button => {
                const timeText = button.querySelector('.time').textContent.trim();
                const availableSlotData = availability.find(slot => slot.time_game === timeText);
                
                if (availableSlotData) {
                    const availableSeats = availableSlotData.available_slots;
                    updateSeats(button, availableSeats);
                    // После обновления мест, необходимо также обновить состояние кнопки
                    updateButtonState(button);
                }
            });
        }
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

        // Запретить ввод нецифровых символов
        playerInput.addEventListener('input', function() {
            this.value = this.value.replace(/[^0-9]/g, '');
        });

        // Запретить ввод нецифровых символов с клавиатуры, включая numpad
        playerInput.addEventListener('keydown', function(event) {
            // Разрешаем клавиши для цифр (0-9, numpad)
            const isNumberKey = (event.keyCode >= 48 && event.keyCode <= 57) || // цифры сверху
                                (event.keyCode >= 96 && event.keyCode <= 105) || // цифры numpad
                                event.keyCode === 8 || 
                                event.keyCode === 46; 
            
            if (!isNumberKey) {
                event.preventDefault();
            }
        });

        // Запретить вставку нецифровых символов
        playerInput.addEventListener('paste', function(event) {
            const pastedData = (event.clipboardData || window.clipboardData).getData('text');
            if (!/^\d*$/.test(pastedData)) {
                event.preventDefault();
            }
        });

        playerInput.addEventListener('input', handleInput);
        updateButtonState(button);
    });

    // Обновление состояния кнопок при выборе новой даты
    $('#date').on('change', function () {
        checkAvailability();
    });

    // Вызов функции для проверки доступности при загрузке страницы
    checkAvailability();
});


