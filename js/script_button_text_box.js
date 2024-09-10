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

        // Обновляем количество свободных мест
        updateSeats(this);
    }

    bookingButtons.forEach(button => {
        const playerInput = button.querySelector('.player-input');
        updateButtonState(button);
        button.addEventListener('click', handleClick);
        playerInput.setAttribute('maxlength', '2');
        playerInput.addEventListener('input', () => updateSeats(button));
        updateSeats(button);
    });
});

document.addEventListener('DOMContentLoaded', function () {
    // Обработчик нажатия кнопки "Добавить"
    var addButton = document.getElementById('add');
    addButton.addEventListener('click', function (event) {
        //event.preventDefault(); // Предотвращаем стандартное поведение кнопки

        // Выполняем действия после нажатия на кнопку "Добавить"
        var selectedButtons = document.querySelectorAll('.booking-button.selected');
        var firstname = document.getElementById('firstName').value;
        var lastname = document.getElementById('lastName').value;
        var phone = document.getElementById('phone').value;
        var email = document.getElementById('email').value;
        var price = button.querySelector(".price").value;

        var typegame = 'OPEN';
        var namegame = 'ARENA QUEST';

        if (selectedButtons.length === 0) {
            alert('Пожалуйста, выберите хотя бы одно время бронирования.');
            return;
        }

       // Собираем все выбранные времена и количество игроков
        var selectedTimes = [];
        var selectedPlayersCount = [];
        selectedButtons.forEach(function (button) {
            var time = button.querySelector('.time').textContent;
            selectedTimes.push(time);

            // Получаем значение из текстового поля input
            var playersCount = button.querySelector('.player-input').value;
            selectedPlayersCount.push(playersCount);
        });

        // Обновляем поле комментария
        var commentField = document.getElementById('comment');
        commentField.value += selectedTimes.join(', ') + "\n";
        commentField.value += selectedPlayersCount.join(', ') + "\n";

        // Создаем объект данных для отправки
        var postData = {
            firstname: firstname,
            lastname: lastname,
            phone: phone,
            email: email,
            typegame: typegame,
            namegame: namegame,
            times: selectedTimes,
            playerCount: selectedPlayersCount,
            price: price
        };

        // Создаем объект XMLHttpRequest
        var xhr = new XMLHttpRequest();

        // Настраиваем запрос POST на сервер
        xhr.open('POST', 'http://localhost:8080/data', true);
        xhr.setRequestHeader('Content-Type', 'application/json'); // Устанавливаем заголовок для JSON

        // Отправляем данные в формате JSON
        xhr.send(JSON.stringify(postData));
    });
});



