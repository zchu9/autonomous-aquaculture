FROM ubuntu:jammy

RUN apt update && apt upgrade -y

RUN apt install -y mosquitto mosquitto-clients

COPY . .

CMD ["mosquitto", "-c", "mosquitto.conf", "-v"]