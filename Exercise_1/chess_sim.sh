#!/bin/bash

declare -A chessboard

move_history=()

rows=("8" "7" "6" "5" "4" "3" "2" "1")
cols=("a" "b" "c" "d" "e" "f" "g" "h")

function init_board {

 for row in "${rows[@]}"; do
    for col in "${cols[@]}"; do
        cell="${col}${row}" 
        case "$cell" in
            a8|h8) chessboard[$cell]="r" ;;  # Rooks
            b8|g8) chessboard[$cell]="n" ;;  # Knights
            c8|f8) chessboard[$cell]="b" ;;  # Bishops
            d8) chessboard[$cell]="q" ;;     # Queen
            e8) chessboard[$cell]="k" ;;     # King
            a7|b7|c7|d7|e7|f7|g7|h7) chessboard[$cell]="p" ;;  # Black pawns
            a2|b2|c2|d2|e2|f2|g2|h2) chessboard[$cell]="P" ;;  # White pawns
            a1|h1) chessboard[$cell]="R" ;;  # Rooks
            b1|g1) chessboard[$cell]="N" ;;  # Knights
            c1|f1) chessboard[$cell]="B" ;;  # Bishops
            d1) chessboard[$cell]="Q" ;;     # Queen
            e1) chessboard[$cell]="K" ;;     # King
            *) chessboard[$cell]="." ;;      # Empty squares
        esac
    done
  done
}

function print_board {

  echo "  a b c d e f g h"
  for row in "${rows[@]}"; do
      echo -n "$row "
      for col in "${cols[@]}"; do
          cell="${col}${row}"
          echo -n "${chessboard[$cell]} "
      done
      echo "$row"
  done
  echo "  a b c d e f g h"
}

function move_forward {
    move=${moves[$index]}
    from=${move:0:2}  # starting position
    to=${move:2:4}    # final position 

    piece=${chessboard[$from]}

    move_history+=(${chessboard[$to]})

    chessboard[$to]=$piece
    chessboard[$from]="." 
}

function move_backward {

  captured_piece=${move_history[-1]}
  unset move_history[-1]  # Remove the last move from the stack

  local move=${moves[$index]} 

    local from=${move:2:4}  # starting position 
    local to=${move:0:2}    # final position 

    chessboard[$to]=${chessboard[$from]}  
    chessboard[$from]=$captured_piece  
}

function move_to_start {
  init_board
}

function move_to_end {
  while [[ $index -lt $((total_moves)) ]]; do
    move_forward
    ((index++))
  done
}


if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <source_pgn_file>"
  exit 1
fi

source_pgn_file=$1

if [[ ! -e "$source_pgn_file" ]]; then
  echo "Error: File '$source_pgn_file' does not exist."
  exit 1  
elif [[ !"$source_pgn_file" == *.pgn\' ]]; then
  echo "Error: The file '$source_pgn_file' is not in .pgn format."
  exit 1  

fi

raw_moves=$(grep -v '^\[' "$source_pgn_file" | sed -n '/^$/,$p' | tr '\n' ' ' | sed 's/  */ /g')

moves=($(python3 parse_moves.py "$raw_moves"))

# Extract metadata (lines starting with `[` and ending with `]`)
metadata=$(grep '^\[.*\]$' "$source_pgn_file")
echo "Metadata from PGN file:"
echo "$metadata"

index=0
total_moves=${#moves[@]}

init_board

echo
echo "Move $((index))/$total_moves"
print_board

while true; do

    echo "Press 'd' to move forward, 'a' to move back, 'w' to go to the start, 's' to go to the end, 'q' to quit:"
    read answer

    if [[ $answer = 'd' ]]; then
        if [[ $index -eq $((total_moves)) ]]; then
            echo "No more moves available."
            continue
        fi
        move_forward
        ((index++))
        

    elif [[ $answer = 'a' ]]; then
        if [[ $index -ne 0 ]]; then
            ((index--))
            move_backward
        fi

    elif [[ $answer = 'w' ]]; then
        move_to_start
        index=0
        

    elif [[ $answer = 's' ]]; then
        move_to_end
        index=$((total_moves))
        

    elif [[ $answer = 'q' ]]; then
        break  
    else
        echo "Invalid key pressed:" $answer
        continue    
    fi
        echo "Move $((index))/$total_moves"
        print_board

done
echo "Exiting."
echo 'End of game.'



