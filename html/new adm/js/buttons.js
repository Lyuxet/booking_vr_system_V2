export function initButtons(calendar) {
    const todayButton = document.querySelector('.today .icon-button');
    let isShowingTodayOnly = false;

    const months = [
        'января', 'февраля', 'марта', 'апреля', 'мая', 'июня',
        'июля', 'августа', 'сентября', 'октября', 'ноября', 'декабря'
    ];

    function formatDateToString(date) {
        const year = date.getFullYear();
        const monthIndex = date.getMonth();
        const day = String(date.getDate()).padStart(2, '0');
        console.log(year, months[monthIndex], day);
        return `${day} ${months[monthIndex]} ${year}`;
    }

    function getTodayDate() {
        return formatDateToString(new Date());
    }

    todayButton.addEventListener('click', function() {
        isShowingTodayOnly = !isShowingTodayOnly;
        todayButton.classList.toggle('active');
        
        const event = new CustomEvent('toggleTodayFilter', {
            detail: { 
                isShowingTodayOnly: isShowingTodayOnly,
                todayDate: getTodayDate()
            }
        });
        document.dispatchEvent(event);
    });
}
