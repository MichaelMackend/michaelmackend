FROM python:3.8-slim-buster

COPY requirements.txt /tmp/

RUN pip install -r /tmp/requirements.txt

RUN useradd --create-home appuser
WORKDIR /home/appuser
USER appuser

ADD src .

ENV FLASK_APP=app.py

EXPOSE 5000

CMD [ "flask", "run", "--host", "0.0.0.0" ]

