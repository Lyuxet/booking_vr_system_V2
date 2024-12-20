export function initCalendar() {
    const calendarBtn = document.getElementById('calendarBtn');
    const calendar = document.getElementById('calendar');
    const calendarGrid = document.getElementById('calendarGrid');
    const currentMonthElement = document.getElementById('currentMonth');
    const prevMonthBtn = document.getElementById('prevMonth');
    const nextMonthBtn = document.getElementById('nextMonth');

    let currentDate = new Date();
    let selectedDate = null;

    function isWeekend(date) {
        const day = date.getDay();
        return day === 0 || day === 6;
    }

    function updateCalendar() {
        const year = currentDate.getFullYear();
        const month = currentDate.getMonth();
        
        currentMonthElement.textContent = new Date(year, month).toLocaleString('ru', { month: 'long', year: 'numeric' });
        
        const firstDay = new Date(year, month, 1);
        const lastDay = new Date(year, month + 1, 0);
        const today = new Date();

        calendarGrid.innerHTML = '';

        // Add weekday headers
        ['Пн', 'Вт', 'Ср', 'Чт', 'Пт', 'Сб', 'Вс'].forEach(day => {
            const dayElement = document.createElement('div');
            dayElement.className = 'calendar-day weekday';
            dayElement.textContent = day;
            calendarGrid.appendChild(dayElement);
        });

        // Add empty cells for days before the first day of the month
        let firstDayIndex = firstDay.getDay() || 7;
        for (let i = 1; i < firstDayIndex; i++) {
            const emptyDay = document.createElement('div');
            emptyDay.className = 'calendar-day';
            calendarGrid.appendChild(emptyDay);
        }

        // Add days of the month
        for (let day = 1; day <= lastDay.getDate(); day++) {
            const dayElement = document.createElement('div');
            dayElement.className = 'calendar-day';
            const currentDayDate = new Date(year, month, day);
            
            if (isWeekend(currentDayDate)) {
                dayElement.classList.add('weekend');
            }

            if (currentDayDate < today) {
                dayElement.classList.add('past-date');
            }

            dayElement.textContent = day;
            dayElement.addEventListener('click', () => selectDate(new Date(year, month, day)));

            if (selectedDate && 
                selectedDate.getDate() === day && 
                selectedDate.getMonth() === month && 
                selectedDate.getFullYear() === year) {
                dayElement.classList.add('selected');
            }

            calendarGrid.appendChild(dayElement);
        }
    }

    function selectDate(date) {
        selectedDate = date;
        calendarBtn.textContent = date.toLocaleDateString('ru');
        calendar.style.display = 'none';
        updateCalendar();
    }

    calendarBtn.addEventListener('click', (e) => {
        e.stopPropagation();
        calendar.style.display = calendar.style.display === 'none' ? 'block' : 'none';
    });

    prevMonthBtn.addEventListener('click', () => {
        currentDate.setMonth(currentDate.getMonth() - 1);
        updateCalendar();
    });

    nextMonthBtn.addEventListener('click', () => {
        currentDate.setMonth(currentDate.getMonth() + 1);
        updateCalendar();
    });

    // Close calendar when clicking outside
    document.addEventListener('click', (e) => {
        if (!calendar.contains(e.target) && e.target !== calendarBtn) {
            calendar.style.display = 'none';
        }
    });

    updateCalendar();
}