#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
export LD_LIBRARY_PATH="$DIR/libs:$DIR:$LD_LIBRARY_PATH"
"$DIR/client"
