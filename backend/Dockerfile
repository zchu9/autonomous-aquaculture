FROM python

WORKDIR /python-docker

COPY . .
RUN pip3 install -r requirements.txt
CMD [ "python3", "backend.py"]