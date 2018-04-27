#!/bin/bash

set -eu

scp -P 54321 $1 avr@127.0.0.1:/home/avr
