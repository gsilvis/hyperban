FROM apiaryio/emcc:1.37

RUN apt-get install -y jq

COPY Makefile /src
COPY module /src/module
COPY levels /src/levels

WORKDIR /src/

RUN make

COPY glue.js /src/
COPY index.html /src/

EXPOSE 8080

CMD python -m SimpleHTTPServer 8080
