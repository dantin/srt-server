#!/bin/bash
set -e

DEPLOY_DIR=/home/devops/srt-server

cd "${DEPLOY_DIR}"

exec LD_LIBRARY_PATH=/usr/local/lib srt-server \
     -config=/usr/local/etc/srt-server.cfg
