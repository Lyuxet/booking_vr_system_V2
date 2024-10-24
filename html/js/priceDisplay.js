export function showPriceDisplay() {
    const priceDisplay = document.querySelector('.price-display');
    priceDisplay.style.display = 'inline-flex'; 
    setTimeout(() => {
        priceDisplay.classList.add('show');
    }, 5); 
}

export function hidePriceDisplay(){
    const priceDisplay = document.querySelector('.price-display');
    priceDisplay.classList.remove('show'); 
    setTimeout(() => {
        priceDisplay.style.display = 'none'; 
    }); 
}