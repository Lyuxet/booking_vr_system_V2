document.addEventListener('DOMContentLoaded', function () {
    var today = new Date();
    $('#date').datepicker({
        dateFormat: 'yy.mm.dd',
        minDate: 0
    }).datepicker("setDate", today);


    // Праздничные дни (укажите даты в формате 'yyyy.mm.dd')
    const holidays = [
        '2024.01.01', // Новый год
        '2024.01.07', // Рождество
        '2024.05.01', // День труда
        // Добавьте другие праздники
    ];

     // Проверка на выходной или праздничный день
     function isWeekendOrHoliday(date) {
        const dayOfWeek = date.getDay(); // 0: воскресенье, 6: суббота
        const formattedDate = date.toISOString().split('T')[0].replace(/-/g, '.'); // Преобразование даты в 'yyyy.mm.dd'

        return dayOfWeek === 0 || dayOfWeek === 6 || holidays.includes(formattedDate);
    }

    // Определяем тип игры
    const isCloseType = document.querySelector('.booking-container-close') !== null;
    // Обновление цены в зависимости от даты
    function updatePrices(date) {
        const isSpecialDay = isWeekendOrHoliday(date);
        bookingButtons.forEach(button => {
            const priceElement = button.querySelector('.price'); // Получаем элемент цены
            let basePrice = priceElement.dataset.price; // Получаем базовую цену из data-атрибута
    
            // Устанавливаем цену в зависимости от условий
            basePrice = isSpecialDay ? (isCloseType ? 15000 : 1600) : basePrice;
            
    
            // Обновляем текстовое содержимое элемента с учётом символа ₽
            priceElement.textContent = `${basePrice} ₽`;
        });
    }
    

    const bookingButtons = document.querySelectorAll('.booking-button');
    const maxPlayers = 10;
    const minPlayers = 1;

    

    function updateSeats(button, availableSeats) {
        const seatsCount = button.querySelector('.seats span');
        if (isCloseType) {
            // Проверяем доступные места и блокируем кнопку, если меньше 10 свободных мест
            if (availableSeats < 10) {
                button.classList.add('disabled');
                seatsCount.textContent = 0;
                return; // Прекращаем выполнение функции
            } else {
                button.classList.remove('disabled');
                if (button.classList.contains('selected')){
                    seatsCount.textContent = 0;
                }
                else{
                    seatsCount.textContent = 10;
                }

            }
        }
        else{
            const playerInput = button.querySelector('.player-input');
            
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
            if (!isCloseType){
                button.querySelector('.player-input').disabled = true;
                button.querySelector('.player-input').value = '';
            }
            button.querySelector('.seats span').textContent = 0;
        } else {
            // Если дата и время еще актуальны, и есть места — кнопка активна
            button.classList.remove('disabled');
            button.addEventListener('click', handleClick);
            if (!isCloseType){
                button.querySelector('.player-input').disabled = false;

            }
        }
    }

    function calculateTotalPrice() {
        let totalPrice = 0;
    
        if (isCloseType) {

            document.querySelectorAll('.booking-button.selected').forEach(button => {
                const fixedPricePerSelection = parseInt(button.querySelector('.price').textContent);
                totalPrice += fixedPricePerSelection;
            });
    
            
        } else {
            document.querySelectorAll('.booking-button.selected').forEach(button => {
                const playerInput = button.querySelector('.player-input');
                const playerCount = parseInt(playerInput.value, 10) || 0;
                const pricePerPlayer = parseInt(button.querySelector('.price').textContent);
    
                // Добавляем к общей стоимости
                totalPrice += playerCount * pricePerPlayer;
            });
        }
    
        // Обновляем содержимое таблички
        const displayAmountElement = document.getElementById('displayAmount');
        if (displayAmountElement) {
            displayAmountElement.textContent = totalPrice;
        }
    
        // Проверяем, выбрана ли хотя бы одна кнопка
        const anyButtonSelected = document.querySelectorAll('.booking-button.selected').length > 0;
    
        const priceDisplayElement = document.querySelector('.price-display');
        if (priceDisplayElement) {
            // Показываем или скрываем табличку в зависимости от состояния кнопок
            if (anyButtonSelected) {
                showPriceDisplay();
            } else {
                window.hidePriceDisplay();
            }
        }
    }
    function showPriceDisplay() {
        const priceDisplay = document.querySelector('.price-display');
        priceDisplay.style.display = 'inline-flex'; // Показываем элемент
        setTimeout(() => {
            priceDisplay.classList.add('show'); // Активируем анимацию появления
        }, 5); // Небольшая задержка для срабатывания transition
    }
    
    window.hidePriceDisplay = function(){
        const priceDisplay = document.querySelector('.price-display');
        priceDisplay.classList.remove('show'); // Убираем класс для анимации скрытия
        setTimeout(() => {
            priceDisplay.style.display = 'none'; // Полностью скрываем элемент после анимации
        }, 100); // Время анимации соответствует transition (0.5s)
    }
    

    function handleClick(event) {
       
        if (this.classList.contains('disabled')) return;
    
        if (isCloseType) {
            // Для CLOSE типа игры, не использовать поле ввода
            this.classList.toggle('selected');
            updateSeats(this, maxPlayers); // Используем максимальное количество мест
        } else {
            if (event.target.closest('.player-input') && this.classList.contains('selected')) return;
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
    
        calculateTotalPrice();
    }
    

    function handleInput(event) {
        const input = event.target;
        const button = input.closest('.booking-button');
        const maxAvailableSeats = parseInt(input.getAttribute('max'), 10) || maxPlayers;
        updateSeats(button, maxAvailableSeats);
        calculateTotalPrice();
    }

    

    window.updateButtonsState = function(availability) {
    
        bookingButtons.forEach(button => {
            // Очистить поле ввода и снять выбор с кнопки
            const playerInput = button.querySelector('.player-input');
            if (playerInput) {
                playerInput.value = ''; // Очистить значение input
            }
            button.classList.remove('selected'); // Снять класс selected
    
            // Обновить состояние кнопки
            
            updateSeats(button);
            
            updateButtonState(button); 
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
    };
    
    

    function checkAvailability() {
        var date = document.getElementById('date').value;
        var placegame = 'ARENA';
        // Проверяем URL и задаем значение для namegame в зависимости от пути файла
        // Извлекаем название игры
        const gameTitleElement = document.querySelector('.navigation h1');
        const namegame = gameTitleElement ? gameTitleElement.textContent.trim() : '';

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
                    window.updateButtonsState(availability);
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
        button.addEventListener('click', handleClick);
        if (!isCloseType){
            const playerInput = button.querySelector('.player-input');
       
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
        }
        
        updateButtonState(button);
    });

    // Обновляем цены и состояния кнопок при изменении даты
    $('#date').on('change', function () {
        const selectedDate = $('#date').datepicker('getDate');
        updatePrices(selectedDate); // Обновление цены
        checkAvailability(); // Проверка доступности мест
    });

    // Проверка доступности при загрузке страницы
    checkAvailability();
    const initialDate = $('#date').datepicker('getDate');
    updatePrices(initialDate); // Обновление цены для начальной даты
});