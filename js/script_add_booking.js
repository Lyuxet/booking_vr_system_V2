document.addEventListener('DOMContentLoaded', function () {
    // Получаем ссылки на все кнопки с классом 'booking-button'
    var bookingButtons = document.querySelectorAll('.booking-button');

    // Обработчик нажатия кнопки "Добавить"
    var addButton = document.getElementById('add');
    addButton.addEventListener('click', function (event) {
        event.preventDefault(); // Предотвращаем стандартное поведение кнопки

        // Получаем все кнопки с классом 'selected'
        var selectedButtons = document.querySelectorAll('.booking-button.selected'); // исправлено: добавлена точка перед классом
        
        if (selectedButtons.length === 0) {
            alert('Пожалуйста, выберите хотя бы одно время бронирования.');
            return;
        }

        // Собираем все выбранные времена
        var selectedTimes = [];
        selectedButtons.forEach(function (button) {
            var time = button.querySelector('.time').textContent;
            selectedTimes.push(time);
        });

        // Получаем поле комментария
        var commentField = document.getElementById('comment');

        // Вставляем все выбранные времена в поле комментария, разделяя их запятой
        commentField.value = "Выбранные времена: " + selectedTimes.join(', ');
    });
});
