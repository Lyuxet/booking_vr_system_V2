export function SetDate(){
    var today = new Date();
    $('#date').datepicker({
        dateFormat: 'yy-mm-dd',
        minDate: 0
    }).datepicker("setDate", today);
}

 // Проверка на выходной или праздничный день
export function isWeekendOrHoliday(date) {
    const dayOfWeek = date.getDay(); // 0: воскресенье, 6: суббота
    const formattedDate = `${String(date.getMonth() + 1).padStart(2, '0')}.${String(date.getDate()).padStart(2, '0')}`; // Преобразование даты в 'mm.dd'

    return dayOfWeek === 0 || dayOfWeek === 6 || holidays.includes(formattedDate);
}

const holidays = [
    '01.01', // Новый год
    '01.07', // Рождество
    '05.01', // День труда
    // Добавьте другие праздники
];