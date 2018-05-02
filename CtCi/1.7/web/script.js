(function ($) {
    $.fn.serializeFormJSON = function () {

        var o = {};
        var a = this.serializeArray();
        $.each(a, function () {
            if (o[this.name]) {
                if (!o[this.name].push) {
                    o[this.name] = [o[this.name]];
                }
                o[this.name].push(this.value || '');
            } else {
                o[this.name] = this.value || '';
            }
        });
        return o;
    };
})(jQuery);

$(document).ready(function() {

$('form').submit(function(event) {
    event.preventDefault();
    var form = $( this ).serializeFormJSON();
    var formstring = JSON.stringify(form);
    $.post("/ctci/1.1/", formstring, function(data) {
       console.log(data);
       if(data.isUnique) {
           $('.results').html("UNIQUE!");
       } else {
           $('.results').html("Not unique.");
       }
    });
});

});

function onRotateMatrixClicked() {
    console.log("Rotate Me Damn You!");
    var data = document.querySelector('data-grid').getData();
    console.log(data);
    var formstring = JSON.stringify(data);
    console.log(formstring);
    $.post("/ctci/1.7/", formstring, function(response) {
        console.log(response);
    });
}
