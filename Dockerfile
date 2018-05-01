FROM apiaryio/emcc:1.37

COPY . /src

WORKDIR /src

RUN make

EXPOSE 8080

CMD python -m SimpleHTTPServer 8080
