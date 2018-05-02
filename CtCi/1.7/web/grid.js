(function() {


    const template = document.createElement('template');


    template.innerHTML = `
        <style>
            .no-spinners {
              -moz-appearance:textfield;
            }

            .no-spinners::-webkit-outer-spin-button,
            .no-spinners::-webkit-inner-spin-button {
                -webkit-appearance: none;
                margin: 0;
            }

            body {
                border: 0;
                margin: 0;
                padding: 0;
            }
            input {
                border: 0;
                margin: 0;
                padding: 0;
                color: cyan;
                background: gray;
                width:33%;
                height:33%;
                text-align: center;
            }
            div {
                border: 0;
                margin: 0;
                padding: 0;
                width: 100%;
                height: 100%;
                display:inline-block;
            }
        </style>
        <div id="container"></div>
    `;

    class DataGrid extends HTMLElement {
        constructor() {
            super();
			console.log('instantiate datagrid!');
            this.attachShadow({mode:'open'});
            this.shadowRoot.appendChild(template.content.cloneNode(true));
            this.containerDiv = this.shadowRoot.querySelector("#container");
        }

        setData(data) {

            this.initializeGridWithSize(data.gridSize);
            var count = data.gridSize * data.gridSize;
            for(var i = 0; i < count; ++i) {
                this.setInputValue(i, data.values[i]);
            }
        }

        getData() {
            var data = {
                gridSize:0,
                values:[]
            };

            data.gridSize = this.rows;
            var count = this.rows * this.rows;
            for(var i = 0; i < count; ++i) {
                data.values.push(this.inputs[i].value);
            }
            return data;
        }

        setInputValue(index, value) {
            this.inputs[index].value = value;
        }

        clearContainer() {
           while (this.containerDiv.firstChild) {
                this.containerDiv.removeChild(this.containerDiv.firstChild);
            }
        }

        initializeGridWithSize(rows) {
            this.clearContainer();

            this.rows = rows;
            this.setAttribute("rows",rows);

            var N = rows*rows;
            var inputSize = (100.0 * (1.0 / rows)) + "%";

            var odd = 1 - (rows % 2);

            var row = 0;
            for(var i = 0; i < N; ++i) {

                var row = Math.floor(i / rows);
                var oddRow = odd & (row % 2);
                console.log("i " + i + " ,row " + row + ", odd " + odd + ": " + (i % 2) + " == " + (row % 2));
                var inputElement = this.containerDiv.appendChild(document.createElement('input'));
                inputElement.setAttribute("type","number");
                inputElement.style.width = inputSize;
                inputElement.style.height = inputSize;
                inputElement.classList.add("no-spinners")
                inputElement.style.backgroundColor = ((i + odd) % 2) == oddRow ? "darkgray" : "gray";
            }

            this.inputs = this.shadowRoot.querySelectorAll("input");
            for(var i = 0; i < this.inputs.length; ++i) {
                this.setInputValue(i, i);
            }
        }

		connectedCallback() {
            console.log("connected!");

            this.containerDiv.style.width = this.style.width;
            this.containerDiv.style.height = this.style.height;

            var count = this.hasAttribute("rows") ? this.getAttribute("rows") : 3;

            this.initializeGridWithSize(count);
		}
    }

    window.customElements.define('data-grid',DataGrid);
    console.log("define grid!");

})();


