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
    var grid = document.querySelector('data-grid');
    var data = grid.getData();
    console.log(data);
    var formstring = JSON.stringify(data);
    console.log(formstring);
    $.post("/ctci/1.7/", formstring, function(response) {
        grid.setData(response);
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

function onGenerateTable() {
    var input = document.querySelector("input");
    var size = input.valueAsNumber;

    var grid = document.createElement("data-grid");
    grid.setAttribute("rows",0);
    grid.style.width = "400px";
    grid.style.height = "400px";


    var parent = document.querySelector("#gridHolder");
    while (parent.firstChild) {
        parent.removeChild(parent.firstChild);
    }
    parent.appendChild(grid);

    var data = {
        gridSize: size,
        values: []
    };

    var count = size * size;
    for(var i = 0; i < count; ++i) {
        data.values.push(getRandomInt(100));
    }

    console.log(data);

    grid.setData(data);
}



