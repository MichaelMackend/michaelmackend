(function() {

    const elemTemplate = document.createElement('template');
    elemTemplate.innerHTML = `
        <style>
           span {
             display:block;
             padding:1px;
           }
        </style>
        <span>
            <input id="input" type="number" pattern="[0-9]{1,5}"/>
            <button id="add">+</button>
            <button id="remove">-</button>
        <span>
    `;

    class DataElement extends HTMLElement {
        constructor() {
            super();
            console.log('instantiate <data-element>!');
            this.attachShadow({mode:'open'});
            this.shadowRoot.appendChild(elemTemplate.content.cloneNode(true));
            this.input = this.shadowRoot.querySelector("#input");
            this.input.value = 0;
            this.addButton = this.shadowRoot.querySelector("#add");
            this.removeButton = this.shadowRoot.querySelector("#remove");
        }

        setValue(value) {
            this.input.value = value;
        }

        getValue(value) {
            return this.input.valueAsNumber;
        }

        setAddAction(action) {
            this.addButton.onclick = action;
        }

        setRemoveAction(action) {
            this.removeButton.onclick = action;
        }

        connectedCallback() {
            console.log("connected element!");

        }

        disconnectedCallback() {
            console.log("disconnected element!");
        }

        attributeChangedCallback(name, oldVal, newVal) {
            console.log("Attribute " + name + " changed from " + oldVal + " to " + newVal);
        }
    }

    window.customElements.define('data-element',DataElement);


    const listTemplate = document.createElement('template');
    listTemplate.innerHTML = `
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
                margin: 0; padding: 0;
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
        <div id="elements"></div>
    `;

    class DataList extends HTMLElement {
        constructor() {
            super();
            console.log('instantiate <data-list>!');
            this.attachShadow({mode:'open'});
            this.shadowRoot.appendChild(listTemplate.content.cloneNode(true));
            this.elements = this.shadowRoot.querySelector("#elements");
            this.addElement(0);
        }

        addAction() {
            this.addElement(5);
        }

        addElement(value, beforeElement) {
            var element = document.createElement("data-element");
            element.setValue(value);
            var self = this;
            element.setAddAction(function() {
                self.addElement(element.getValue(), element);
            });
            element.setRemoveAction(function() {
                self.elements.removeChild(element);
            })
            this.elements.insertBefore(element, beforeElement);
        }

        clearElements() {
            while (this.elements.firstChild) {
                this.elements.removeChild(this.elements.firstChild);
            }
        }

        setData(data) {
            this.clearElements();
            var count = data.values.length;
            for(var i = 0; i < count; ++i) {
                this.addElement(data.values[i]);
            }
        }

        getData() {
            var data = {
                values:[]
            };

            var count = this.elements.children.length;
            for(var i = 0; i < count; ++i) {
                data.values.push(this.elements.children[i].getValue());
            }
            return data;
        }

		connectedCallback() {
            console.log("connected!");

            /*this.containerDiv.style.width = this.style.width;
            this.containerDiv.style.height = this.style.height;

            var count = this.hasAttribute("rows") ? this.getAttribute("rows") : 3;

            this.initializeGridWithSize(count);*/
		}
    }

    window.customElements.define('data-list',DataList);
    console.log("define list!");

})();


