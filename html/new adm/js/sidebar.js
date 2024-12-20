export function initSidebar() {
    const sidebar = document.getElementById('sidebar');
    const toggleBtn = document.getElementById('toggleSidebar');
    const mainContent = document.querySelector('.main-content');

    function toggleSidebar() {
        sidebar.classList.toggle('collapsed');
        if (sidebar.classList.contains('collapsed')) {
            mainContent.style.marginLeft = '0';
            toggleBtn.style.left = '20px';
        } else {
            mainContent.style.marginLeft = '250px';
            toggleBtn.style.left = '270px';
        }
    }

    toggleBtn.addEventListener('click', toggleSidebar);

    // Initial setup
    if (window.innerWidth > 768) {
        toggleBtn.style.left = '270px';
    }

    // Handle window resize
    window.addEventListener('resize', function() {
        if (window.innerWidth <= 768) {
            mainContent.style.marginLeft = '0';
            toggleBtn.style.left = '20px';
            sidebar.classList.add('collapsed');
        } else {
            if (!sidebar.classList.contains('collapsed')) {
                mainContent.style.marginLeft = '250px';
                toggleBtn.style.left = '270px';
            }
        }
    });
}

