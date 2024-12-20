export function initModal() {
    const modal = document.getElementById('accountModal');
    const accountInfoBtn = document.getElementById('accountInfoBtn');
    const closeBtn = document.getElementsByClassName('close')[0];

    accountInfoBtn.onclick = function() {
        modal.style.display = 'block';
    }

    closeBtn.onclick = function() {
        modal.style.display = 'none';
    }

    window.onclick = function(event) {
        if (event.target == modal) {
            modal.style.display = 'none';
        }
    }
}

