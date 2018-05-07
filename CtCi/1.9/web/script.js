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
    $.post("/ctci/1.9/", formstring, function(data) {
       console.log(data);
       if("rotated" in data && data.rotated == true) {
           $('.results').html("Your string has been rotated! I'm so sorry...");
       } else {
           $('.results').html("You're probably okay.  But stay vigilant.  It could happen to anyone!");
       }
    });
});

});

