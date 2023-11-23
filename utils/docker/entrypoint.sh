#!/bin/sh

case $1 in

sh|bash)
  shift
  bash "$@"
  ;;

letheand)
  shift
  seed_node_ip=$(getent hosts $SEED_NODE | awk '{ print $1 }' | head -n 1)
  if tty | grep -q /dev/;
  then
    interactive=""
  else
    interactive="--non-interactive"
  fi
  echo letheand $interactive --confirm-external-bind --data-dir=/home/lethean/.letheand \
    --p2p-bind-ip=0.0.0.0 \
    --add-peer $seed_node_ip --seed-node $seed_node_ip \
    --log-level=$LOG_LEVEL "$@" >&2
  letheand $interactive --confirm-external-bind --data-dir=/home/lethean/.letheand \
    --p2p-bind-ip=0.0.0.0 \
    --add-peer $seed_node_ip --seed-node $seed_node_ip \
    --log-level=$LOG_LEVEL "$@" >&2
  ;;

wallet)
  shift
  echo lethean-wallet-cli "$@" >&2
  lethean-wallet-cli "$@"
  ;;
  
wallet-rpc)
  shift
  echo lethean-wallet-rpc "$@" >&2
  lethean-wallet-rpc "$@" 
  ;;

*)
  $0 letheand "$@"
  ;;

esac
