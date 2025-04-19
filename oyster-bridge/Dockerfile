FROM python

WORKDIR /python-docker

# Set PYTHONPATH
ENV PYTHONPATH=/python-docker

COPY . .
RUN pip3 install -r requirements.txt
CMD [ "python3", "bridge.py"]