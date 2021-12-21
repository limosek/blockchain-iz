FROM ubuntu:16.04 as build


ARG RELEASE_TYPE=release-static

RUN set -x \
  && buildDeps=' \
      ca-certificates \
      cmake \
      g++ \
      git \
      libboost1.58-all-dev \
      libssl-dev \
      make \
      pkg-config \
  ' \
  && apt-get -qq update \
  && apt-get -qq --no-install-recommends install $buildDeps

RUN git clone https://github.com/letheanVPN/blockchain.git /usr/local/src/lethean
WORKDIR /usr/local/src/lethean

RUN make -j$(nproc) ${RELEASE_TYPE}

FROM scratch as build-result
COPY --from=build /usr/local/src/lethean/build/release/bin/ /

FROM debian:bullseye-slim as container

RUN apt-get install -v ca-certificates

COPY --from=build-result --chmod=0777 / /usr/local/bin
# Contains the blockchain
VOLUME /root/.lethean

# Generate your wallet via accessing the container and run:
# cd /wallet
# lethean-wallet-cli
VOLUME /wallet

ENV LOG_LEVEL 0
ENV P2P_BIND_IP 0.0.0.0
ENV P2P_BIND_PORT 48772
ENV RPC_BIND_IP 127.0.0.1
ENV RPC_BIND_PORT 48782

EXPOSE 48782
EXPOSE 48772

CMD letheand --log-level=$LOG_LEVEL --p2p-bind-ip=$P2P_BIND_IP --p2p-bind-port=$P2P_BIND_PORT --rpc-bind-ip=$RPC_BIND_IP --rpc-bind-port=$RPC_BIND_PORT
