export function SetDate(){
    var today = new Date();
    $('#date').datepicker({
        dateFormat: 'yy.mm.dd',
        minDate: 0
    }).datepicker("setDate", today);
}

