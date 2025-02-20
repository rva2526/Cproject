#!/bin/bash

COMPILED_FILE=test2

echo ''
echo ''
echo 'Running Plouf based magnetics code'
echo ''

read -t 60 -p "Do you want to create a simple shape? (y/n) " response

if [[ $response == [yY] ]]; then

    echo "Creating..."
    echo ''
    echo ''

    source activate notebook
    python ./src/make_parameter.py
    echo ''
    echo ''


elif [[ $response == [nN] ]]; then

    echo "You chose no."
    echo ''
    echo ''

else
    echo "Invalid input."
    echo ''
    echo ''

fi

if [ -f "$COMPILED_FILE" ]; then

    echo "$COMPILED_FILE exists."

else

    echo "$COMPILED_FILE does not exist."
    echo ''
    echo 'Running make'
    export LD_LIBRARY_PATH=/home/jovyan/lib:$LD_LIBRARY_PATH
    make

fi