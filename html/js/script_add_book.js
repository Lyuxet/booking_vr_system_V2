document.addEventListener('DOMContentLoaded', function () {
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

        // Проверяем URL и задаем значение для namegame в зависимости от пути файла
        let namegame = ''; // Значение по умолчанию

        const currentURL = window.location.href;
        if (currentURL.includes('OpenArenaQuest.html')) {
            namegame = 'QUEST';
        } else if (currentURL.includes('OpenArenaShooter.html')) {
            namegame = 'SHOOTER';
        } else {
            // Дополнительные условия можно добавить здесь
        }

        // Создаем строку данных для отправки в формате x-www-form-urlencoded
        const postData = 'firstname=' + encodeURIComponent(firstname) +
                         '&lastname=' + encodeURIComponent(lastname) +
                         '&phone=' + encodeURIComponent(phone) +
                         '&email=' + encodeURIComponent(email) +
                         '&placegame=' + encodeURIComponent('ARENA') +
                         '&typegame=' + encodeURIComponent('OPEN') +
                         '&namegame=' + encodeURIComponent(namegame) +  // Используем динамическое значение namegame
                         '&date=' + encodeURIComponent(date) +
                         '&times=' + encodeURIComponent(selectedTimes.join(',')) +
                         '&playerCount=' + encodeURIComponent(selectedPlayersCount.join(',')) +
                         '&price=' + encodeURIComponent(selectedPrice.join(',')) +
                         '&comment=' + encodeURIComponent(comment);

        console.log('Sending data:', postData);

        // Создаем запрос через XMLHttpRequest
        const xhr = new XMLHttpRequest();
        xhr.open('POST', 'http://localhost:8080/addBookingOpenArena', true);
        xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

        // Обрабатываем ответ от сервера
        xhr.onload = function () {
            if (xhr.status >= 200 && xhr.status < 300) {
                alert('Бронирование успешно отправлено.');
                console.log('Response data:', xhr.responseText);
                location.reload();
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
