import { initSidebar } from './sidebar.js';
import { initTheme } from './theme.js';
import { initCalendar } from './calendar.js';
import { initTable } from './table.js';
import { initModal } from './modal.js';

document.addEventListener('DOMContentLoaded', function() {
    initSidebar();
    initTheme();
    initCalendar();
    initTable();
    initModal();

    // Add event listener for the new "Забронировать" button
    const reserveBtn = document.getElementById('reserveBtn');
    reserveBtn.addEventListener('click', function() {
        alert('Функция бронирования будет добавлена в будущем обновлении.');
    });
});

