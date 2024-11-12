function showPopup() {
    const overlay = document.getElementById('overlay');
    const popup = document.getElementById('customPopup');
    
    overlay.style.display = 'block'; // Показываем overlay
    popup.classList.add('show');
    popup.classList.remove('hide');
}

    function closePopup() {
    const overlay = document.getElementById('overlay');
    const popup = document.getElementById('customPopup');

    overlay.style.display = 'none'; // Скрываем overlay
    popup.classList.add('hide');
    popup.classList.remove('show');
}