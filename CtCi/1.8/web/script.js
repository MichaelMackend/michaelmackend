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


function onZeroMatrixClicked() {
    var grid = document.querySelector('data-grid');
    var data = grid.getData();
    var formstring = JSON.stringify(data);
    $.post("/ctci/1.8/", formstring, function(response) {
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

    if(size > 20) {
        size = 20;
    }

    if(size < 1) {
        size = 1;
    }

    input.value = size;

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
        data.values.push(getRandomInt(10));
    }

    grid.setData(data);
}



