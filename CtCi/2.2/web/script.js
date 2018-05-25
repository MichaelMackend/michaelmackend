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
    $.post("/ctci/2.1/", formstring, function(data) {
       console.log(data);
       if("return" in data) {
           $('.results').html(data.return);
       } else {
           $('.results').html("Something went wrong.");
       }
    });
});

});

function onGetKthToLast() {
    console.log("On Get Kth To Last");
    var input = document.querySelector('input');
    var k = input.value;
    var list = document.querySelector('data-list');
    var data = list.getData();
    data.k = Number(k);
    var formstring = JSON.stringify(data);
    console.log(formstring);
    $.post("/ctci/2.2/", formstring, function(response) {
        var results = document.querySelector('#result');
        results.innerHTML = response.return;
    })
    .done(function() {
        console.log("second success" );
    })
    .fail(function() {
        console.log( "error" );
    })
    .always(function() {
        console.log( "finished" );
    });
}

function getRandomInt(max) {
  return Math.floor(Math.random() * Math.floor(max));
}

