export function initMobileEnhancements() {
    const tableWrapper = document.querySelector('.table-wrapper');
    const scrollHint = document.querySelector('.table-scroll-hint');

    if (scrollHint) {
        // Show scroll hint only if table is wider than viewport
        const checkScrollHint = () => {
            if (tableWrapper.scrollWidth > tableWrapper.clientWidth) {
                scrollHint.style.display = 'block';
                // Hide hint after first scroll
                const hideHint = () => {
                    scrollHint.style.display = 'none';
                    tableWrapper.removeEventListener('scroll', hideHint);
                };
                tableWrapper.addEventListener('scroll', hideHint);
            } else {
                scrollHint.style.display = 'none';
            }
        };

        // Check on load and resize
        checkScrollHint();
        window.addEventListener('resize', checkScrollHint);
    }

    // Add touch event handlers for better mobile experience
    if ('ontouchstart' in window) {
        const buttons = document.querySelectorAll('button');
        buttons.forEach(button => {
            button.addEventListener('touchstart', function(e) {
                this.style.opacity = '0.7';
            });
            button.addEventListener('touchend', function(e) {
                this.style.opacity = '1';
            });
        });
    }

    // Prevent double-tap zoom on buttons and controls
    document.querySelectorAll('button, select, .calendar-day').forEach(element => {
        element.addEventListener('touchend', function(e) {
            e.preventDefault();
            // Trigger click after a small delay to prevent ghost clicks
            setTimeout(() => {
                if (document.activeElement !== this) {
                    this.click();
                }
            }, 100);
        });
    });
}

