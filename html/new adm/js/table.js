export function initTable() {
    const tableBody = document.getElementById('tableBody');
    const rowsPerPageSelect = document.getElementById('rowsPerPage');
    const prevPageBtn = document.getElementById('prevPage');
    const nextPageBtn = document.getElementById('nextPage');
    const currentPageSpan = document.getElementById('currentPage');
    const columnHeaders = document.querySelectorAll('.column-header');

    let currentPage = 1;
    let rowsPerPage = 20;
    let filteredData = [];

    const data = [
        {
            service: 'Командная игра м. Новокосино',
            date: '29 Окт 2024',
            time: '11:15-12:00 x10',
            client: 'Елена',
            added: '23.10.2024',
            personnel: 'Андрей',
            status: 'Принят',
            price: '10000 RUB',
            comment: 'Через 6 суток'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '26 Окт 2024',
            time: '16:15-17:00 x10',
            client: 'Ольга',
            added: '21.10.2024',
            personnel: 'Михаил',
            status: 'Подтвержден',
            price: '15000 RUB',
            comment: 'Через 3 суток'
        },
        {
            service: 'Свободная игра м. Новокосино',
            date: '20 Окт 2024',
            time: '16:15-17:00 x2',
            client: 'Дарья',
            added: '19.10.2024',
            personnel: 'Сергей',
            status: 'Подтвержден',
            price: '6400 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        },
        {
            service: 'Командная игра м. Новокосино',
            date: '22 Окт 2024',
            time: '17:15-18:00 x10',
            client: 'Юлия',
            added: '16.10.2024',
            personnel: 'Андрей',
            status: 'Подтвержден',
            price: '27000 RUB',
            comment: 'Истёк'
        }
    ];

    function renderTable() {
        const start = (currentPage - 1) * rowsPerPage;
        const end = start + rowsPerPage;
        const pageData = filteredData.slice(start, end);

        tableBody.innerHTML = '';
        pageData.forEach(item => {
            const row = document.createElement('tr');
            row.innerHTML = `
                <td>${item.service}</td>
                <td>${item.date}</td>
                <td>${item.time}</td>
                <td>${item.client}</td>
                <td>${item.added}</td>
                <td>${item.personnel}</td>
                <td>${item.status}</td>
                <td>${item.price}</td>
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
        nextPageBtn.disabled = currentPage === Math.ceil(filteredData.length / rowsPerPage);
    }

    function filterData() {
        filteredData = data.filter(item => {
            return (
                (searchTerms.service === '' || item.service.toLowerCase().includes(searchTerms.service.toLowerCase())) &&
                (searchTerms.date === '' || item.date.toLowerCase().includes(searchTerms.date.toLowerCase())) &&
                (searchTerms.time === '' || item.time.toLowerCase().includes(searchTerms.time.toLowerCase())) &&
                (searchTerms.client === '' || item.client.toLowerCase().includes(searchTerms.client.toLowerCase())) &&
                (searchTerms.added === '' || item.added.toLowerCase().includes(searchTerms.added.toLowerCase())) &&
                (searchTerms.personnel === '' || item.personnel.toLowerCase().includes(searchTerms.personnel.toLowerCase())) &&
                (searchTerms.status === '' || item.status.toLowerCase().includes(searchTerms.status.toLowerCase())) &&
                (searchTerms.price === '' || item.price.toLowerCase().includes(searchTerms.price.toLowerCase()))
            );
        });
        currentPage = 1;
        renderTable();
    }

    rowsPerPageSelect.addEventListener('change', function() {
        rowsPerPage = parseInt(this.value);
        currentPage = 1;
        renderTable();
    });

    prevPageBtn.addEventListener('click', function() {
        if (currentPage > 1) {
            currentPage--;
            renderTable();
        }
    });

    nextPageBtn.addEventListener('click', function() {
        if (currentPage < Math.ceil(filteredData.length / rowsPerPage)) {
            currentPage++;
            renderTable();
        }
    });

    const searchTerms = {
        service: '', date: '', time: '', client: '', added: '', personnel: '', status: '', price: ''
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

    // Инициализация
    filteredData = [...data];
    renderTable();
}

