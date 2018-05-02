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
                color: blue;
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

            initializeGridWithSize(data.gridSize);

            for(var i = 0; i < data.values; ++i) {
                setInputValue(i, data.values[i]);
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

            for(var i = 0; i < N; ++i) {
                var inputElement = this.containerDiv.appendChild(document.createElement('input'));
                inputElement.setAttribute("type","number");
                inputElement.style.width = inputSize;
                inputElement.style.height = inputSize;
                inputElement.classList.add("no-spinners")
                inputElement.style.backgroundColor = (i % 2) == 0 ? "lightgray" : "gray";
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


