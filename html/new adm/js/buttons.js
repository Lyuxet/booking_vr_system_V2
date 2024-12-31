export function initButtons(calendar) {
    const todayButton = document.querySelector('.today .icon-button');
    const actualButton = document.querySelector('.actual .icon-button');

    const months = [
        'января', 'февраля', 'марта', 'апреля', 'мая', 'июня',
        'июля', 'августа', 'сентября', 'октября', 'ноября', 'декабря'
    ];

    function formatDateToString(date) {
        const year = date.getFullYear();
        const monthIndex = date.getMonth();
        const day = String(date.getDate()).padStart(2, '0');
        return `${day} ${months[monthIndex]} ${year}`;
    }

    function getTodayDate() {
        return formatDateToString(new Date());
    }

    function getEndOfMonth() {
        const today = new Date();
        const lastDay = new Date(today.getFullYear(), today.getMonth() + 1, 0);
        return formatDateToString(lastDay);
    }

    todayButton.addEventListener('click', function() {
        if (actualButton.classList.contains('active')) {
            actualButton.classList.remove('active');
        }
        todayButton.classList.toggle('active');
        
        const event = new CustomEvent('toggleTodayFilter', {
            detail: { 
                isShowingTodayOnly: todayButton.classList.contains('active'),
                todayDate: getTodayDate()
            }
        });
        document.dispatchEvent(event);
    });

    actualButton.addEventListener('click', function() {
        if (todayButton.classList.contains('active')) {
            todayButton.classList.remove('active');
        }
        actualButton.classList.toggle('active');
        
        const event = new CustomEvent('toggleActualFilter', {
            detail: {
                isShowingActualOnly: actualButton.classList.contains('active'),
                startDate: getTodayDate(),
                endDate: getEndOfMonth()
            }
        });
        document.dispatchEvent(event);
    });
}
