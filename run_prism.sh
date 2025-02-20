#!/bin/bash

#variables
COMPILED_FILE=test2

echo ''
echo ''
echo 'Running Plouf based magnetics code'
echo ''

#need this in the environment for running the C code
export LD_LIBRARY_PATH=/home/jovyan/lib:$LD_LIBRARY_PATH


#looking to see if you need to make a simple shape file 
read -t 60 -p "Do you want to create a simple shape? (y/n) " response

if [[ $response == [yY] ]]; then # yes you do so run the python code for makeing simple shapes

    echo "Creating..."
    echo ''
    echo ''

    source activate notebook
    python ./src/make_parameter.py
    echo ''
    echo ''


elif [[ $response == [nN] ]]; then # no you have your own shape file

    #  adding file path
    read -t 60 -p "Filepath to shapefile json: " shape_path

    if [ -f "$shape_path" ]; then #checking that the file exits
        
        echo $shape_path 'exists'
        echo ''
        echo ''

    else #exiting if invaled
        echo $shape_path 'does not exist, exiting...'
        exit 1
    fi
 
else #exiting if invaled
    echo "Invalid input, exiting..."
    exit 1

fi







# if [ -f "$COMPILED_FILE" ]; then

#     echo "$COMPILED_FILE exists."

# else

#     echo "$COMPILED_FILE does not exist."
#     echo ''
#     echo 'Running make'
#     make

# fi