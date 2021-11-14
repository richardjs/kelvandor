FROM alpine
RUN apk update && \
    apk add g++ make python3 py3-pip && \
    pip install flask gunicorn
WORKDIR /opt/kelvandor
COPY ["src/", "."]
RUN ["make"]
ENTRYPOINT ["make", "httpapi"]
