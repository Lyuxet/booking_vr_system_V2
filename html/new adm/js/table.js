export function initTable() {
    const tableBody = document.getElementById('tableBody');
    const rowsPerPageSelect = document.getElementById('rowsPerPage');
    const prevPageBtn = document.getElementById('prevPage');
    const nextPageBtn = document.getElementById('nextPage');
    const currentPageSpan = document.getElementById('currentPage');
    const columnHeaders = document.querySelectorAll('.column-header');

    let currentPage = 1;
let rowsPerPage = 20;
let filteredData = []; // Filtered data
let originalData = []; // Add this line to store original data


    const place_game = "VR Кубы";
    const date = "2025.01.01";

    const url = `http://localhost:8081/getAdminBooking?place_game=${encodeURIComponent(place_game)}&date=${encodeURIComponent(date)}`;

    const xhr = new XMLHttpRequest();
    xhr.open('GET', url, true);
    xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');

    xhr.onload = function () {
        if (xhr.status >= 200 && xhr.status < 300) {
            const availability = JSON.parse(xhr.responseText);
            originalData = availability; // Store original data
            filteredData = [...originalData]; // Create a copy for filtering
            console.log(filteredData);
            renderTable(filteredData);
        } else {
            console.error('Ошибка запроса доступности. Статус:', xhr.status);
        }
    };

    xhr.onerror = function() {
        // Обработка ошибки сети без вывода в консоль
        console.error('Произошла ошибка сети');
    };

    xhr.send();

    function renderTable(data) {
        if (!data) return; // Проверяем наличие данных
        
        const start = (currentPage - 1) * rowsPerPage;
        const end = start + rowsPerPage;
        const pageData = data.slice(start, end);
    
        tableBody.innerHTML = '';
        pageData.forEach(item => {
            const row = document.createElement('tr');
            row.innerHTML = `
                <td>${item.place_game}</td>
                <td>${item.date_game}</td>
                <td>${item.time_game}</td>
                <td>${item.client_name || ''}</td>
                <td>${item.date_add_book || ''}</td>
                <td>${item.who_reservation || ''}</td>
                <td>${item.book_status || ''}</td>
                <td>${item.price || ''}</td>
                <td class="actions">
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
            tableBody.appendChild(row);
        });
    
        currentPageSpan.textContent = currentPage;
        prevPageBtn.disabled = currentPage === 1;
        nextPageBtn.disabled = currentPage === Math.ceil(data.length / rowsPerPage);
    }
    

    function filterData() {
        filteredData = originalData.filter(item => { // Filter from original data
            return (
                (searchTerms.place_game === '' || (item.place_game || '').toLowerCase().includes(searchTerms.place_game.toLowerCase())) &&
                (searchTerms.date_game === '' || (item.date_game || '').toLowerCase().includes(searchTerms.date_game.toLowerCase())) &&
                (searchTerms.time_game === '' || (item.time_game || '').toLowerCase().includes(searchTerms.time_game.toLowerCase())) &&
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

    const searchTerms = {
        place_game: '', 
        date_game: '', 
        time_game: '', 
        client_name: '', 
        type_game: '', 
        players_count: '', 
        client_phone: '', 
        price: '',
        client_email: '',     // Added missing fields
        book_status: '',      // Added missing fields
        who_reservation: '',  // Added missing fields
        date_add_book: ''    // Added missing fields
    };
    columnHeaders.forEach(header => {
        const field = header.getAttribute('data-field');

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

        // Сохраняем оригинальное значение
        header.setAttribute('data-original', header.value);
    });
}
