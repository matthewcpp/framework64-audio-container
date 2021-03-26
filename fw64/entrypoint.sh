#!/usr/bin/env bash

if test "$#" -lt 1; then
    echo "No operation specified."
    exit 1
fi


case $1 in

    create_sequence_bank)
        node "fw64/src/create_sequence_bank.js"
        ;;

    create_sound_bank)
        node "fw64/src/create_sound_bank.js"
        ;;

    *)
        echo "Unknown command: $1"
        exit 1
        ;;
esac