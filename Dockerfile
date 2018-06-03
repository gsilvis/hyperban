FROM trzeci/emscripten:sdk-tag-1.38.3-64bit

WORKDIR /src/

COPY package.json /src

RUN npm install --loglevel verbose

COPY Makefile /src/
COPY module /src/module
COPY levels /src/levels

RUN mkdir web && make build-module

COPY web /src/web
COPY ./webpack.config.js /src/

RUN make; ls -lh dist

EXPOSE 8080

CMD make run-web
