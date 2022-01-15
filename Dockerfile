FROM ubuntu:16.04 as build

#ARG BRANCH=release-3.1
ARG BRANCH=dev
ARG RELEASE_TYPE=release-static

RUN apt-get -qq update \
  && apt-get -qq --no-install-recommends install ca-certificates libboost-all-dev cmake g++ git libssl-dev make pkg-config libunbound-dev

WORKDIR /usr/local/src/lethean
COPY . .

RUN make -j$(nproc) ${RELEASE_TYPE}

FROM debian:bullseye-slim as container

COPY --from=build /usr/local/src/lethean/build/release/bin/ /usr/local/bin
RUN apt-get install -v ca-certificates

# Contains the blockchain
VOLUME /root/Lethean/data

# Generate your wallet via accessing the container and run:
# cd /wallet
# lethean-wallet-cli
VOLUME /wallet

ENV LOG_LEVEL 0
ENV P2P_BIND_IP 0.0.0.0
ENV P2P_BIND_PORT 38772
ENV RPC_BIND_IP 0.0.0.0
ENV RPC_BIND_PORT 38782

EXPOSE 38772
EXPOSE 38782

CMD letheand --testnet --confirm-external-bind --data-dir=/root/Lethean/data --log-level=$LOG_LEVEL --p2p-bind-ip=$P2P_BIND_IP --p2p-bind-port=$P2P_BIND_PORT --rpc-bind-ip=$RPC_BIND_IP --rpc-bind-port=$RPC_BIND_PORT

