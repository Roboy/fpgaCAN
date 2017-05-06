#!/bin/bash
sopcinfo2swinfo --input="$PWD/soc_system.sopcinfo"
swinfo2header \
--swinfo "$PWD/soc_system.swinfo" \
--single hps_0.h \
--module hps_0
