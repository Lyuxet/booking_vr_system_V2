document.addEventListener('DOMContentLoaded', () => {
    // Получаем все текстовые поля и счетчики
    const playerInputs = document.querySelectorAll('.player-input');
    const seatsCounts = document.querySelectorAll('.seats span');

    const maxPlayers = 10;
    const totalSeats = 10; // Общее количество мест

    function updateSeats() {
        playerInputs.forEach((playerInput, index) => {
            const seatsCount = seatsCounts[index];
            const currentPlayers = parseInt(playerInput.value, 10) || 0;
            if (currentPlayers > maxPlayers) {
                playerInput.value = maxPlayers;
            }
            const availableSeats = Math.max(0, totalSeats - currentPlayers); // Расчет свободных мест
            seatsCount.textContent = availableSeats; // Обновление отображаемого количества свободных мест
        });
    }

    // Добавляем обработчик события 'input' для всех текстовых полей
    playerInputs.forEach(input => {
        input.addEventListener('input', updateSeats);
    });

    // Инициализация начального значения
    updateSeats();
});
