#!/usr/bin/env bash

if test "$#" -lt 1; then
    echo "No operation specified."
    exit 1
fi


case $1 in

    create_sequence_bank)
        node "$FW64_DIR/scripts/create_sequence_bank.js" $2
        ;;

    create_sound_bank)
        node "$FW64_DIR/scripts/create_sound_bank.js" $2
        ;;

    *)
        echo "Unknown command: $1"
        exit 1
        ;;
esac