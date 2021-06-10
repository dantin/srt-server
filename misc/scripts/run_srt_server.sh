#!/bin/bash
set -e

DEPLOY_DIR=/home/devops/Documents/deploy/srt-server

cd "${DEPLOY_DIR}"

exec srt-server \
     -c /usr/local/etc/srt-server.cfg
