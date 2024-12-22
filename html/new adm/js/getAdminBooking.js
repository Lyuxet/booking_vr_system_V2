



export function GetAdminBooking(){

    const place_game = "VR Кубы";
    const date = "2025.01.01";

    const url = `http://localhost:8081/getAdminBooking?place_game=${encodeURIComponent(place_game)}&date=${encodeURIComponent(date)}`;

    const xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

    xhr.onload = function () {
        if (xhr.status >= 200 && xhr.status < 300) {
            // Обработка успешного ответа без вывода в консоль
            const availability = JSON.parse(xhr.responseText);
            console.log(availability);
        } else {
            console.error('Ошибка запроса доступности. Статус:', xhr.status);
        }
    };

    xhr.onerror = function() {
        // Обработка ошибки сети без вывода в консоль
        console.error('Произошла ошибка сети');
    };

    xhr.send();

}