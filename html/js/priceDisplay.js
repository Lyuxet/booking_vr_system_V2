export function showPriceDisplay() {
    const priceDisplay = document.querySelector('.price-display');
    priceDisplay.style.display = 'inline-flex'; // Показываем элемент
    setTimeout(() => {
        priceDisplay.classList.add('show'); // Активируем анимацию появления
    }, 5); // Небольшая задержка для срабатывания transition
}

export function hidePriceDisplay(){
    const priceDisplay = document.querySelector('.price-display');
    priceDisplay.classList.remove('show'); // Убираем класс для анимации скрытия
    setTimeout(() => {
        priceDisplay.style.display = 'none'; // Полностью скрываем элемент после анимации
    }, 100); // Время анимации соответствует transition (0.5s)
}