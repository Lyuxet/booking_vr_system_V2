export function SetDate(){
    var today = new Date();
    $('#date').datepicker({
        dateFormat: 'yy.mm.dd',
        minDate: 0
    }).datepicker("setDate", today);
}

// Праздничные дни (укажите даты в формате 'yyyy.mm.dd')
const holidays = [
    '2024.01.01', // Новый год
    '2024.01.07', // Рождество
    '2024.05.01', // День труда
    // Добавьте другие праздники
];

 // Проверка на выходной или праздничный день
export function isWeekendOrHoliday(date) {
    const dayOfWeek = date.getDay(); // 0: воскресенье, 6: суббота
    const formattedDate = date.toISOString().split('T')[0].replace(/-/g, '.'); // Преобразование даты в 'yyyy.mm.dd'

    return dayOfWeek === 0 || dayOfWeek === 6 || holidays.includes(formattedDate);
}