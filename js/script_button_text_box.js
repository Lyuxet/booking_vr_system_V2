document.addEventListener('DOMContentLoaded', function () {
    const datePicker = document.querySelector('.date-picker');

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

    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');
        //updateButtonState(button);
        button.addEventListener('click', handleClick);
        playerInput.setAttribute('maxlength', '2');
        playerInput.addEventListener('input', () => updateSeats(button));
        updateSeats(button);
    });

    
        const addButton = document.getElementById('add');
        if (!addButton) {
            console.error('Кнопка "Добавить" не найдена');
            return;
        }
    
        addButton.addEventListener('click', function (event) {
            event.preventDefault(); // Предотвращаем стандартное поведение кнопки
        
            const selectedButtons = document.querySelectorAll('.booking-button.selected');
            const firstname = document.getElementById('firstName').value;
            const lastname = document.getElementById('lastName').value;
            const phone = document.getElementById('phone').value;
            const email = document.getElementById('email').value;
            const date = document.getElementById('date').value;
            const comment = document.getElementById('comment').value;
        
            if (selectedButtons.length === 0) {
                alert('Пожалуйста, выберите хотя бы одно время бронирования.');
                return;
            }
        
            // Собираем все выбранные времена и количество игроков
            let selectedTimes = [];
            let selectedPlayersCount = [];
            let selectedPrice = [];
            selectedButtons.forEach(function (button) {
                const time = button.querySelector('.time').textContent;
                selectedTimes.push(time);
        
                const priceButton = parseInt(button.querySelector('.price').textContent);
                const playersCount = parseInt(button.querySelector('.player-input').value);
                selectedPlayersCount.push(playersCount);
                const price = priceButton * playersCount;
                selectedPrice.push(price);
            });
        
            // Создаем строку данных для отправки в формате x-www-form-urlencoded
            const postData = 'firstname=' + encodeURIComponent(firstname) +
                             '&lastname=' + encodeURIComponent(lastname) +
                             '&phone=' + encodeURIComponent(phone) +
                             '&email=' + encodeURIComponent(email) +
                             '&typegame=' + encodeURIComponent('OPEN') +
                             '&namegame=' + encodeURIComponent('ARENA QUEST') +
                             '&date=' + encodeURIComponent(date) +
                             '&times=' + encodeURIComponent(selectedTimes.join(',')) +
                             '&playerCount=' + encodeURIComponent(selectedPlayersCount.join(',')) +
                             '&price=' + encodeURIComponent(selectedPrice.join(',')) +
                             '&comment=' + encodeURIComponent(comment);
        
            console.log('Sending data:', postData);
        
            // Создаем запрос через XMLHttpRequest
            const xhr = new XMLHttpRequest();
            xhr.open('POST', 'http://localhost:8080/addBooking', true);
            xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
        
            // Обрабатываем ответ от сервера
            xhr.onload = function () {
                if (xhr.status >= 200 && xhr.status < 300) {
                    alert('Бронирование успешно отправлено.');
                    console.log('Response data:', xhr.responseText);
                } else {
                    alert(`Ошибка отправки бронирования. Статус: ${xhr.status}`);
                }
            };
        
            xhr.onerror = function () {
                alert('Ошибка сети.');
                console.error('Network Error');
            };
        
            // Отправляем данные на сервер
            xhr.send(postData);
        });
        
    
    
});
