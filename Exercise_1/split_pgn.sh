#!/bin/bash

# Check if two arguments are provided
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <source_pgn_file> <destination_directory>"
  exit 1
fi


source_pgn_file=$1
destination_directory=$2

if [[ ! -e "$source_pgn_file" ]]; then
  echo "Error: File '$source_pgn_file' does not exist."
  exit 1  
elif [[ !"$source_pgn_file" == *.pgn\' ]]; then
  echo "Error: The file '$source_pgn_file' is not in .pgn format."
  exit 1  

fi

# Check if  directory not exist
if [[ ! -d "$destination_directory" ]]; then
  mkdir -p "$destination_directory" || { echo "Failed to create directory '$destination_directory'."; exit 1; }
  echo "Created directory '$destination_directory'."
fi


file_name=$(basename "$source_pgn_file" .pgn)


awk -v out_dir="$destination_directory" '
  /^\[Event / {
      file = sprintf("%s/'$file_name'_%d.pgn", out_dir, ++i)
      system(sprintf("echo Saved game to '\''%s'\''", file))
  }
  {
      print > file
  }' "$source_pgn_file"
  
echo "All games have been split and saved to '$destination_directory'."