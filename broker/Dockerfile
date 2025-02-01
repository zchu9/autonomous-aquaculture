FROM ubuntu:jammy

RUN apt update && apt upgrade -y

RUN apt install -y mosquitto mosquitto-clients

COPY . .

EXPOSE 5173

CMD ["mosquitto", "-c", "mosquitto.conf", "-v"]