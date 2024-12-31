export function initTable(calendar) {
    const tableBody = document.getElementById('tableBody');
    const rowsPerPageSelect = document.getElementById('rowsPerPage');
    const prevPageBtn = document.getElementById('prevPage');
    const nextPageBtn = document.getElementById('nextPage');
    const currentPageSpan = document.getElementById('currentPage');
    const columnHeaders = document.querySelectorAll('.column-header');

    let currentPage = 1;
    let rowsPerPage = 15;
    let filteredData = []; // Filtered data
    let originalData = []; // Add this line to store original data
    let isInitialLoad = true;
    let isShowingTodayOnly = false;
    let todayDate = '';

    // Определяем searchTerms на верхнем уровне
    const searchTerms = {
        place_game: '', 
        date_game: '', 
        time_game: '', 
        client_name: '', 
        type_game: '', 
        name_game: '',
        players_count: '', 
        client_phone: '', 
        price: '',
        client_email: '',
        book_status: '',
        who_reservation: '',
        date_add_book: ''
    };

    // Функция для форматирования даты в нужный формат
    function formatDate(date) {
        const year = date.getFullYear();
        const month = String(date.getMonth() + 1).padStart(2, '0');
        const day = String(date.getDate()).padStart(2, '0');
        return `${year}.${month}.${day}`;
    }

    // Функция для загрузки данных с сервера
    function loadTableData(date) {
        const place_game = document.querySelector('.main-header h1 span').textContent;
        const formattedDate = formatDate(date);

        const url = `http://localhost:8081/getAdminBooking?place_game=${encodeURIComponent(place_game)}&date=${encodeURIComponent(formattedDate)}`;

        const xhr = new XMLHttpRequest();
        xhr.open('GET', url, true);
        xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

        xhr.onload = function () {
            if (xhr.status >= 200 && xhr.status < 300) {
                const availability = JSON.parse(xhr.responseText);
                originalData = availability; 
                filteredData = [...originalData]; 
                renderTable(filteredData);
            } else {
                console.error('Ошибка запроса доступности. Статус:', xhr.status);
            }
        };

        xhr.onerror = function() {
            console.error('Произошла ошибка сети');
        };

        xhr.send();
    }

    // Подписываемся на изменения даты в календаре
    calendar.onDateChange((date) => {
        if (isInitialLoad) {
            isInitialLoad = false;
            return;
        }
        loadTableData(date);
    });

    // Загружаем данные для начальной даты
    const initialDate = calendar.getSelectedDate();
    loadTableData(initialDate);

    function renderTable(data) {
        if (!data) return;
        
        const start = (currentPage - 1) * rowsPerPage;
        const end = start + rowsPerPage;
        const pageData = data.slice(start, end);
        const place_game = document.querySelector('.main-header h1 span').textContent;
        const isVRArena = place_game === 'VR Арена';

        tableBody.innerHTML = '';
        pageData.forEach(item => {
            const dateAddBook = item.date_add_book ? item.date_add_book.replace(' ', '<br>') : '';
            
            const timeWithPlayers = item.players_count ? 
                `${item.time_game} × ${item.players_count}` : 
                item.time_game;

            const row = document.createElement('tr');
            let rowContent = `
                <td>${isVRArena ? item.type_game : item.place_game}</td>`;

            // Добавляем столбец с названием игры только для VR Арены
            if (isVRArena) {
                rowContent += `<td>${item.name_game || ''}</td>`;
            }

            rowContent += `
            <td>${item.date_game}</td>
                <td>${timeWithPlayers}</td>
                <td>${item.client_phone}</td>
                <td>${dateAddBook}</td>
                <td>${item.who_reservation || ''}</td>
                <td>${item.book_status || ''}</td>
                <td>${item.price || ''}</td>
                <td> 
                    <button class="action-btn message-btn" title="Сообщение">
                        <svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-message-circle"><path d="M21 11.5a8.38 8.38 0 0 1-.9 3.8 8.5 8.5 0 0 1-7.6 4.7 8.38 8.38 0 0 1-3.8-.9L3 21l1.9-5.7a8.38 8.38 0 0 1-.9-3.8 8.5 8.5 0 0 1 4.7-7.6 8.38 8.38 0 0 1 3.8-.9h.5a8.48 8.48 0 0 1 8 8v.5z"></path></svg>
                    </button>
                    <button class="action-btn edit-btn" title="Редактировать">
                        <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-edit-2"><path d="M17 3a2.828 2.828 0 1 1 4 4L7.5 20.5 2 22l1.5-5.5L17 3z"></path></svg>
                    </button>
                    <button class="action-btn delete-btn" title="Удалить">
                        <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="feather feather-x"><line x1="18" y1="6" x2="6" y2="18"></line><line x1="6" y1="6" x2="18" y2="18"></line></svg>
                    </button>
                </td>
            `;
            row.innerHTML = rowContent;
            tableBody.appendChild(row);
        });

        currentPageSpan.textContent = currentPage;
        prevPageBtn.disabled = currentPage === 1;
        nextPageBtn.disabled = currentPage === Math.ceil(data.length / rowsPerPage);
    }
    

    function filterData() {
        filteredData = originalData.filter(item => {
            // First check if we're showing today only
            if (isShowingTodayOnly && item.date_game !== todayDate) {
                return false;
            }

            return (
                (searchTerms.place_game === '' || (item.place_game || '').toLowerCase().includes(searchTerms.place_game.toLowerCase())) &&
                (searchTerms.date_game === '' || (item.date_game || '').toLowerCase().includes(searchTerms.date_game.toLowerCase())) &&
                (searchTerms.time_game === '' || (item.time_game || '').toLowerCase().includes(searchTerms.time_game.toLowerCase())) &&
                (searchTerms.name_game === '' || (item.name_game || '').toLowerCase().includes(searchTerms.name_game.toLowerCase())) &&
                (searchTerms.client_name === '' || (item.client_name || '').toLowerCase().includes(searchTerms.client_name.toLowerCase())) &&
                (searchTerms.type_game === '' || (item.type_game || '').toLowerCase().includes(searchTerms.type_game.toLowerCase())) &&
                (searchTerms.players_count === '' || String(item.players_count || '').includes(searchTerms.players_count)) &&
                (searchTerms.client_phone === '' || (item.client_phone || '').toLowerCase().includes(searchTerms.client_phone.toLowerCase())) &&
                (searchTerms.price === '' || String(item.price || '').includes(searchTerms.price)) &&
                (searchTerms.client_email === '' || (item.client_email || '').toLowerCase().includes(searchTerms.client_email.toLowerCase())) &&
                (searchTerms.book_status === '' || (item.book_status || '').toLowerCase().includes(searchTerms.book_status.toLowerCase())) &&
                (searchTerms.who_reservation === '' || (item.who_reservation || '').toLowerCase().includes(searchTerms.who_reservation.toLowerCase())) &&
                (searchTerms.date_add_book === '' || (item.date_add_book || '').toLowerCase().includes(searchTerms.date_add_book.toLowerCase()))
            );
        });
        currentPage = 1;
        renderTable(filteredData);
    }

    rowsPerPageSelect.addEventListener('change', function() {
        rowsPerPage = parseInt(this.value);
        currentPage = 1;
        renderTable(filteredData);
    });

    prevPageBtn.addEventListener('click', function() {
        if (currentPage > 1) {
            currentPage--;
            renderTable(filteredData);
        }
    });

    nextPageBtn.addEventListener('click', function() {
        if (currentPage < Math.ceil(filteredData.length / rowsPerPage)) {
            currentPage++;
            renderTable(filteredData);
        }
    });

    // Настройка обработчиков для заголовков колонок
    columnHeaders.forEach(header => {
        const field = header.getAttribute('data-field');
        if (!field) return; // Пропускаем заголовки без data-field

        header.addEventListener('focus', function() {
            this.value = '';
            this.placeholder = 'Поиск...';
        });

        header.addEventListener('blur', function() {
            if (!this.value) {
                this.value = this.getAttribute('data-original');
                this.placeholder = '';
            }
        });

        header.addEventListener('input', function() {
            searchTerms[field] = this.value;
            filterData();
        });

        header.setAttribute('data-original', header.value);
    });

    document.addEventListener('toggleTodayFilter', function(e) {
        isShowingTodayOnly = e.detail.isShowingTodayOnly;
        todayDate = e.detail.todayDate;
        filterData();
    });

    document.addEventListener('toggleActualFilter', function(event) {
        const { isShowingActualOnly, startDate, endDate } = event.detail;
        
        const rows = document.querySelectorAll('#tableBody tr');
        rows.forEach(row => {
            const dateCell = row.cells[2]; // Получаем третью ячейку, где находится date_game
            if (dateCell) {
                const rowDate = dateCell.textContent.trim(); // Получаем текст даты
                if (isShowingActualOnly) {
                    row.style.display = (rowDate >= startDate && rowDate <= endDate) ? '' : 'none';
                } else {
                    row.style.display = '';
                }
            }
        });
    });
}
