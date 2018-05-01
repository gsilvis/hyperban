FROM apiaryio/emcc:1.37

RUN apt-get install -y jq

COPY . /src

WORKDIR /src

RUN make

EXPOSE 8080

CMD python -m SimpleHTTPServer 8080
