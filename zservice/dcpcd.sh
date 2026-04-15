#!/bin/sh

SERVICES="/sbin/resolvd /sbin/dhcpcd"

trap 'kill 0; exit' INT TERM

for cmd in $SERVICES; do
  (
    while true; do
      $cmd
      sleep 1
    done
  ) &
done

wait
