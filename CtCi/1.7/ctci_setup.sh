#!/bin/bash

echo "Starting CtCI..."

$(/sbin/my_init)

$(/deploy/bin/app)
