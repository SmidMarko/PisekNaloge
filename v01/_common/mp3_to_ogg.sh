#!/bin/bash

# Script to convert all MP3 files into OGG files

if ! which sox > /dev/null
then
  echo "Converter 'sox' not found. Please install 'sox'."
  exit 1
fi
IFS="
"
for FILE in `find . -type f -name \*.mp3`
do
    NEWFILE="${FILE%.mp3}.ogg"
    echo "Converting $FILE to $NEWFILE..."
    sox "$FILE" "$NEWFILE"
done
