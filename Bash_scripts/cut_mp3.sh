#!/bin/bash
# Loop through all .mp3 files in the current directory
for file in *.mp3; do
    # Use ffmpeg to cut the first 15 seconds off each file and save it with a "cut_" prefix
    ffmpeg -i "$file" -ss 15 -acodec copy "cut_$file"
done

