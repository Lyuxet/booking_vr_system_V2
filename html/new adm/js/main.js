import { initSidebar } from './sidebar.js';
import { initTheme } from './theme.js';
import { initCalendar } from './calendar.js';
import { initTable } from './table.js';
import { initModal } from './modal.js';
import { initButtons } from './buttons.js';

document.addEventListener('DOMContentLoaded', function() {
    initSidebar();
    initTheme();
    const calendar =initCalendar();
    initTable(calendar);
    initModal();
    initButtons(calendar);

    // Add event listener for the new "Забронировать" button
    const reserveBtn = document.getElementById('reserveBtn');
    reserveBtn.addEventListener('click', function() {
        alert('Функция бронирования будет добавлена в будущем обновлении.');
    });
});

