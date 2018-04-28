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
    $.post("/ctci/1.5/", formstring, function(data) {
       console.log(data);
       if(data.isoneeditaway) {
           $('.results').html("ONE KEYSTROKE OR LESS!");
       } else {
           $('.results').html("I'm not sure how much but... more than one keystroke.  I'd give it up.");
       }
    });
});

});
